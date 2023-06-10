#include <planificacion.h>

/*
void planificador_largo_plazo(){

  log_info(info_logger, "Kernel - PLANIFICADOR LARGO PLAZO INICIADO.\n");

  while (1)
  {
    if(procesosEnNew > 0){
    aumentarGradoMP();
    pthread_mutex_lock(&mutex_PlanLP);
    procesosEnNew--;

    moverProceso_NewPreReady();
    pthread_mutex_unlock(&mutex_PlanLP);
    }
  }
  
}
 */

void planificador_largo_plazo(){
    log_info(info_logger, "Kernel - PLANIFICADOR LARGO PLAZO INICIADO.\n");
    while(1){
        sem_wait(&sem_procesosEnNew);
        if(procesosTotales_MP < cfg_kernel->GRADO_MAX_MULTIPROGRAMACION && queue_size(colaNew) > 0) {
            pthread_mutex_lock(&mutex_colaNew);
            t_pcb* pcbAReady = queue_peek(colaNew);
            pthread_mutex_unlock(&mutex_colaNew);
            aumentarGradoMP();
            //Enviamos el proceso a memoria para que lo cargue(pero hasta que memoria no nos confirma NO LO SACAMOS DE LA COLA NEW)
            enviarValor_uint32(pcbAReady->id,fd_memoria,INICIALIZAR_PROCESO_MEMORIA,info_logger);
            log_info(info_logger,"PID <%d> Enviado a memoria para ser cargado", pcbAReady->id);
        }

    }
}

void planificador_corto_plazo(){
    log_info(info_logger, "Kernel - PLANIFICADOR CORTO PLAZO INICIADO.");
    while(1){
        sem_wait(&sem_procesosReady);
        moverProceso_readyExec();
    }
}

void liberar_procesos(){
    while (1){
        sem_wait(&sem_procesosExit);
        pthread_mutex_lock(&mutex_colaExit);
        t_pcb* pcbALiberar = queue_pop(colaExit);
        pthread_mutex_unlock(&mutex_colaExit);

        enviarValor_uint32(pcbALiberar->id,fd_memoria,FINALIZAR_PROCESO_MEMORIA,info_logger);
        liberarPcb(pcbALiberar);
        //TODO FALTA NOTIFICAR A LA CONSOLA EL FIN DEL PROCESO
        //El decremento del grado de MP se hace cuando recibo la confimacion de memoria de la finalizacion
    }
}

void moverProceso_NewReady(t_list* tablaDeSegmentosMemoria){
    pthread_mutex_lock(&mutex_colaNew);
    t_pcb* pcbAReady = queue_pop(colaNew);
    pthread_mutex_unlock(&mutex_colaNew);

    pcbAReady->tablaSegmentos = tablaDeSegmentosMemoria;

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbAReady);
    pthread_mutex_unlock(&mutex_ColaReady);
    sem_post(&sem_procesosReady);
}

void moverProceso_BloqrecursoReady(t_recurso* recurso){
    pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
    t_pcb* pcbLiberada = queue_pop(recurso->cola);
    pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);
    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbLiberada);
    pthread_mutex_unlock(&mutex_ColaReady);
    log_info(info_logger,"PID: <%d> - Estado Anterior: <BLOCKED_RECURSO[%s]> - Estado Actual: <READY>",pcbLiberada->id,recurso->nombreRecurso);
}


void decrementarGradoMP(){
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP--;
    pthread_mutex_unlock(&mutex_MP);
    log_info(info_logger, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}

void aumentarGradoMP(){
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP++;
    pthread_mutex_unlock(&mutex_MP);
    log_info(info_logger, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}


void agregarProceso_New(t_pcb *pcbNew){
    pthread_mutex_lock(&mutex_colaNew);
    queue_push(colaNew, pcbNew);
    pthread_mutex_unlock(&mutex_colaNew);
    log_info(info_logger, "Se crea el proceso <%d> en NEW.", pcbNew->id);
    sem_post(&sem_procesosEnNew);
}

void moverProceso_readyExec(){
        pthread_mutex_lock(&mutex_ColaReady);
        pthread_mutex_lock(&mutex_colaExec);

        if(strcmp(cfg_kernel->ALGORITMO_PLANIFICACION, "HRRN") == 0){
            int posicion = seleccionar_segunHRRN();
            t_pcb *pcbReady = list_get(colaReady,posicion);
            pcbReady->tiempoEnvioExec = time(NULL) ;
            list_add(colaExec, pcbReady);
            list_remove(colaReady,posicion);
            pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);
       
            enviar_paquete_pcb(pcbReady, fd_cpu,PCB, info_logger);
            log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->id);

        }
        else{
            //ALGORITMO FIFO
            t_pcb *pcbReady = list_get(colaReady,0);
            list_add(colaExec,pcbReady);
            list_remove(colaReady,0);
            pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);
       
            enviar_paquete_pcb(pcbReady, fd_cpu,PCB, info_logger);
            log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->id);

        }

}

void moverProceso_ExecBloq(t_pcb *pcbBuscado){

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_lock(&mutex_colaBloq);
    list_add(colaBloq,pcbBuscado);

    pthread_mutex_unlock(&mutex_colaExec);
    pthread_mutex_unlock(&mutex_colaBloq);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: BLOQ.", pcbBuscado->id);

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

void moverProceso_BloqReady(t_pcb* pcbBuscado){

    pthread_mutex_lock(&mutex_colaBloq);
    eliminarElementoLista(pcbBuscado, colaBloq);
    pthread_mutex_lock(&mutex_ColaReady);

    //HHRN
    //pcbBuscado->tiempoLlegadaReady = time(NULL);

    list_add(colaReady, pcbBuscado);
    log_info(info_logger, "PID: [%d] - Estado Anterior: BLOQ - Estado Actual: READY.", pcbBuscado->id);

    pthread_mutex_unlock(&mutex_ColaReady);
    pthread_mutex_unlock(&mutex_colaBloq);

}

void moverProceso_ExecExit(t_pcb *pcbBuscado){

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: EXIT", pcbBuscado->id);

    pthread_mutex_lock(&mutex_colaExit);
    queue_push(colaExit,pcbBuscado);
    pthread_mutex_unlock(&mutex_colaExit);

    sem_post(&sem_procesosExit);

}

void moverProceso_ExecReady(t_pcb* pcbBuscado){
    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: READY", pcbBuscado->id);
    pthread_mutex_unlock(&mutex_colaExec);

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbBuscado);
    pthread_mutex_unlock(&mutex_ColaReady);


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
    t_pcb *pcbBuscado;


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


