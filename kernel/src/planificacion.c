#include <planificacion.h>

int fd_cpu;
int fd_memoria;


void agregarProceso_New(pcb *pcbNew){

    queue_push(colaNew, (void *) pcbNew);
    log_info(logger_kernel, "Se crea el proceso [%d] en NEW.", pcbNew->pid);
    procesosEnNew++;

}


void moverProceso_NewReady(){

    procesosEnNew--;
    pcb *pcbNew = queue_pop(colaNew);
    //FALTA TERMINAR se envia mensaje a memoria, hilo memoria cuando obtenga rta continua el flujo asignando a ready

    enviar_paquete_pcb(pcbNew, fd_memoria,INICIAR_ESTRUCTURA_PCB_NUEVO, logger_kernel);
    log_info(logger_kernel, "Mensaje enviado a %d (MEMORIA) con motivo proceso NUEVO", fd_memoria);

    liberar_pcb(pcbNew);
}


void moverProceso_readyExec(){


        pthread_mutex_lock(&mutex_ColaReady);
        pthread_mutex_lock(&mutex_colaExec);

        if(strcmp(KERNEL_CONFIG.ALGORITMO_PLANIFICACION, "HRRN") == 0){
            int posicion = seleccionar_segunHRRN(); //FALTA DECLARAR METODO
            pcb *pcbReady = list_get(colaReady,posicion);
            list_add(colaExec, (void *) pcbReady);
            list_remove(colaReady,posicion);

        }
        else{
            pcb *pcbReady = list_get(colaReady,0);
            list_add(colaExec, (void *) pcbReady);
            list_remove(colaReady,0);

        }

        pthread_mutex_unlock(&mutex_ColaReady);
        pthread_mutex_unlock(&mutex_colaExec);

        enviar_paquete_pcb(pcbReady, fd_cpu,PCB, logger_kernel);
        log_info(logger_kernel, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->pid);

}

void moverProceso_ExecBloq(Pcb *pcbBuscado){ 

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_lock(&mutex_colaBloq);
    list_add(colaBloq, (void *) pcbBuscado);

    pthread_mutex_unlock(&mutex_colaExec);
    pthread_mutex_unlock(&mutex_colaBloq);

    log_info(logger_kernel, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: BLOQ.", pcbBuscado->pid);

}

void moverProceso_ExecReady(pcb *pcbBuscado){ 

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);

    enviar_paquete_pcb(pcbReady, fd_cpu,INTERRUPCION, logger_kernel);

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady, (void *) pcbBuscado);

    log_info(logger_kernel, "PID: [%d] - Estado Anterior: EXEC- Estado Actual: READY.", pcbBuscado->pid);

    pthread_mutex_unlock(&mutex_ColaReady1);
    pthread_mutex_unlock(&mutex_colaExec);

}


void moverProceso_BloqReady(pcb *pcbBuscado){ 

    pthread_mutex_lock(&mutex_colaBloq);
    eliminarElementoLista(pcbBuscado, colaBloq);

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady, (void *) pcbBuscado);
    log_info(logger_kernel, "PID: [%d] - Estado Anterior: BLOQ - Estado Actual: READY.", pcbBuscado->pid);

    pthread_mutex_unlock(&mutex_ColaReady);
    pthread_mutex_unlock(&mutex_colaBloq);

}

void moverProceso_ExecExit(pcb *pcbBuscado){ 

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);

    log_info(logger_kernel, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: EXIT", pcbBuscado->pid);

    pthread_mutex_unlock(&mutex_colaExec);
    decrementarGradoMP();
    queue_push(colaExit,(void *) pcbBuscado);

}