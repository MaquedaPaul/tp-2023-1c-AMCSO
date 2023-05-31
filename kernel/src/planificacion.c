#include <planificacion.h>

int fd_cpu;
int fd_memoria;

void planificador_largo_plazo(){

  log_info(logger_kernel, "Kernel - PLANIFICADOR LARGO PLAZO INICIADO.\n");

  while (1)
  {
    if(procesosEnNew > 0){
    aumentarGradoMP();
    pthread_mutex_lock(&mutex_PlanLP);
    procesosEnNew--;

    moverProceso_NewReady();
    pthread_mutex_unlock(&mutex_PlanLP);
    }
  }
  
}


void decrementarGradoMP(){
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP--;
    pthread_mutex_unlock(&mutex_PlanLP);
    pthread_mutex_unlock(&mutex_MP);

    log_info(logger_kernel, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}

void aumentarGradoMP(){
    if(procesosTotales_MP>=atoi(cfg_kernel->GRADO_MAX_MULTIPROGRAMACION)){
        pthread_mutex_lock(&mutex_PlanLP);
    }
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP++;
    pthread_mutex_unlock(&mutex_MP);

    log_info(logger_kernel, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}


void agregarProceso_New(pcb *pcbNew){

    queue_push(colaNew, (void *) pcbNew);
    log_info(logger_kernel, "Se crea el proceso [%d] en NEW.", pcbNew->pid);
    procesosEnNew++;

}


void moverProceso_NewPreReady(){ //TENER ENC CUENTA MP

    procesosEnNew--;
    pcb *pcbNew = queue_pop(colaNew);
    /*FALTA TERMINAR (COORDINAR) se envia mensaje a memoria, hilo memoria cuando obtenga rta continua el flujo asignando a ready
        1. Hilo dedicado para atender el envio y rta a memoria
        2. Enviar el mensaje y esperar el mensaje al hilo de memoria. COORDINAR pcb.campo e id a enviar
        3. Enviar el mensaje y esperar el mensaje al hilo de memoria. enviar PCB (abstrae contruccion del paquete y modificaion) RECOMENDADA
    */

    enviar_paquete_pcb(pcbNew, fd_memoria,INICIAR_ESTRUCTURA_PCB_NUEVO, logger_kernel);

    list_add(listaEsperaMemoria,pcbNew);

    log_info(logger_kernel, "Mensaje enviado a %d (MEMORIA) con motivo proceso INICIAR_ESTRUCTURA_PCB_NUEVO", fd_memoria);

    //liberar_pcb(pcbNew);
}


void moverProceso_MemoriaReady(){
    
}


void moverProceso_readyExec(){


        pthread_mutex_lock(&mutex_ColaReady);
        pthread_mutex_lock(&mutex_colaExec);

        if(strcmp(cfg_kernel->ALGORITMO_PLANIFICACION, "HRRN") == 0){
            int posicion = seleccionar_segunHRRN();
            pcb *pcbReady = list_get(colaReady,posicion);
            pcbReady->tiempoEnvioExec = time(NULL) ;
            list_add(colaExec, (void *) pcbReady);
            list_remove(colaReady,posicion);
            pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);
       
            enviar_paquete_pcb(pcbReady, fd_cpu,PCB, logger_kernel);
            log_info(logger_kernel, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->pid);

        }
        else{
            pcb *pcbReady = list_get(colaReady,0);
            list_add(colaExec, (void *) pcbReady);
            list_remove(colaReady,0);
            pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);
       
            enviar_paquete_pcb(pcbReady, fd_cpu,PCB, logger_kernel);
            log_info(logger_kernel, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->pid);

        }

}

void moverProceso_ExecBloq(pcb *pcbBuscado){ 

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_lock(&mutex_colaBloq);
    list_add(colaBloq, (void *) pcbBuscado);

    pthread_mutex_unlock(&mutex_colaExec);
    pthread_mutex_unlock(&mutex_colaBloq);

    log_info(logger_kernel, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: BLOQ.", pcbBuscado->pid);

}

/*
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
*/

void moverProceso_BloqReady(pcb *pcbBuscado){ 

    pthread_mutex_lock(&mutex_colaBloq);
    eliminarElementoLista(pcbBuscado, colaBloq);
    pthread_mutex_lock(&mutex_ColaReady);

    //HHRN
    pcbBuscado->tiempoLlegadaReady = time(NULL);

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

int seleccionar_segunHRRN(){ 
    
    /* ::REQUIERE::: 
                    -setear tiempoLlegadaReady cuando pcb llega a ready
                    -setear rafagaAnterior cuando vuelve de exec
                    -actualizar rafagaEstProx en pcb.rafagaEstimada cuando se selecciona para HRRN
    */

    int mejorOpcion = 0; // index mejor posicion en lista

    double tEspera = 0;
    double rafagaEstProx = 0;
    double rafagaEstProxElegida = 0;
    time_t tFin = time(NULL) ;
    double resultado = 0;
    double mejorRR = 0;
    pcb *pcbBuscado;


    for (size_t i = 0; i < list_size(colaReady); i++)
    {
        pcbBuscado = list_get(colaReady,i);

       tEspera =  tFin - pcbBuscado->tiempoLlegadaReady;

       if (pcbBuscado->estimacionRafaga == 0) //se puede mover al crear PCB
       {
            pcbBuscado->estimacionRafaga = cfg_kernel->ESTIMACION_INICIAL;
            pcbBuscado->rafagaAnterior = 0;
       }
       
       rafagaEstProx = (cfg_kernel->HRRN_ALFA * pcbBuscado->rafagaAnterior)+ ((1-cfg_kernel->HRRN_ALFA )*pcbBuscado->estimacionRafaga);

       
       resultado = 1 + (tEspera / rafagaEstProx);

       if(mejorRR == 0){
        mejorRR = resultado;
        mejorOpcion = i;
        rafagaEstProxElegida = rafagaEstProx;
       }

       if(mejorRR > resultado){
        mejorRR = resultado;
        mejorOpcion = i;
        rafagaEstProxElegida = rafagaEstProx;
       }

    }

    //Actualizo la rafaga estimada del pcb elegido    
    pcbBuscado = list_get(colaReady,mejorOpcion);
    pcbBuscado->estimacionRafaga = rafagaEstProxElegida;
    
    return mejorOpcion;
}


