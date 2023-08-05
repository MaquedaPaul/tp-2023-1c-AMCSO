#include <planificacion.h>
bool hayCompactacionPendiente = false;

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
        sem_wait(&sem_cpuLibre);
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
        enviarOrden(PROCESO_TERMINADO,pcbALiberar->fd_consola,info_logger);
        liberarPcb(pcbALiberar);
        //TODO Esta bien notificar a la consola aca? pq pierdo el motivo del fin

        //El decremento del grado de MP se hace cuando recibo la confimacion de memoria de la finalizacion
    }
}

void moverProceso_NewReady(t_tablaSegmentos* tablaDeSegmentosMemoria){
    pthread_mutex_lock(&mutex_colaNew);
    t_pcb* pcbAReady = queue_pop(colaNew);
    pthread_mutex_unlock(&mutex_colaNew);

    pcbAReady->tablaSegmentos = tablaDeSegmentosMemoria;

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbAReady);
    pthread_mutex_unlock(&mutex_ColaReady);
    pthread_mutex_lock(&m_pcb);
    pcbAReady->tiempoIngresoReady = time(NULL);
    pthread_mutex_unlock(&m_pcb);
    sem_post(&sem_procesosReady);
    mostrarEstadoColas();
}

void bloquearProcesoPorRecurso(t_recurso* recurso){
    pthread_mutex_lock(&mutex_colaExec);
    t_pcb* pcbABlockedRecurso = list_remove(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);

    calcular_estimacion(pcbABlockedRecurso);
    sem_post(&sem_cpuLibre);

    pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
    list_add(recurso->cola,pcbABlockedRecurso);
    pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);

    log_info(info_logger,"PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED_RECURSO[%s]>", pcbABlockedRecurso->id, recurso->nombreRecurso);
}

void moverProceso_BloqrecursoReady(t_recurso* recurso){
    pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
    t_pcb* pcbLiberada = list_remove(recurso->cola,0);
    pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);
    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbLiberada);
    pthread_mutex_unlock(&mutex_ColaReady);
    pthread_mutex_lock(&m_pcb);
    pcbLiberada->tiempoIngresoReady = time(NULL);
    pthread_mutex_unlock(&m_pcb);
    sem_post(&sem_procesosReady);
    log_info(info_logger,"PID: <%d> - Estado Anterior: <BLOCKED_RECURSO[%s]> - Estado Actual: <READY>",pcbLiberada->id,recurso->nombreRecurso);
    mostrarEstadoColas();
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

//R.R. = (S + W) / S
//Donde S = Ráfaga estimada y W = Tiempo de espera.

double response_ratio(t_pcb* pcb){
    pthread_mutex_lock(&m_pcb);
    time_t tiempo_actual = time(NULL);
    double espera = difftime(tiempo_actual, pcb->tiempoIngresoReady);
    double rr = (pcb->estimacionRafaga + espera * 1000) / pcb->estimacionRafaga;
    pthread_mutex_unlock(&m_pcb);
    log_info(info_logger, "El response ratio del proceso <%d> es:  %f",pcb->id, rr);
    return rr;
}

void calcular_estimacion(t_pcb* pcb){
    if(strcmp(cfg_kernel->ALGORITMO_PLANIFICACION, "HRRN") == 0){
        pthread_mutex_lock(&m_pcb);
        time_t tiempo_actual = time(NULL);
        char* tiempoString = ctime(&tiempo_actual);
        log_debug(debug_logger,"tiempo actual: {%s}", tiempoString);
        char* tiempoInicialExecString =  ctime(&pcb->tiempoInicialExec);
        log_debug(debug_logger,"tiempo inicial exec: {%s}", tiempoInicialExecString);
        double rafaga = difftime(tiempo_actual, pcb->tiempoInicialExec);
        log_debug(debug_logger,"rafaga: {%f]", rafaga);
        //S = α . estimadoAnterior + (1 - α) . ráfagaAnterior
        uint32_t nueva_estimacion = cfg_kernel->HRRN_ALFA * pcb->estimacionRafaga + (1-cfg_kernel->HRRN_ALFA) * rafaga * 1000;
        pcb->estimacionRafaga = nueva_estimacion;
        log_debug(debug_logger,"estimacion_pcb: {%d}", pcb->estimacionRafaga);
        pthread_mutex_unlock(&m_pcb);
    }
}

bool criterio_hrrn(t_pcb* pcb1, t_pcb* pcb2){
    return response_ratio(pcb1) >= response_ratio(pcb2);
}

void moverProceso_readyExec(){
        pthread_mutex_lock(&mutex_ColaReady);
        pthread_mutex_lock(&mutex_colaExec);

        if(strcmp(cfg_kernel->ALGORITMO_PLANIFICACION, "HRRN") == 0){
            log_trace(trace_logger,"--------------Colas sin ordenar------------------");
            mostrarEstadoColas();
            list_sort(colaReady,criterio_hrrn);
            log_trace(trace_logger,"--------------FIN Colas sin ordenar------------------");
            log_trace(trace_logger,"--------------Colas ordenadas------------------");
            mostrarEstadoColas();
            log_trace(trace_logger,"--------------FIN Colas ordenadas------------------");
            t_pcb* pcb = list_remove(colaReady,0);
            list_add(colaExec,pcb);
            pthread_mutex_lock(&m_pcb);
            pcb->tiempoInicialExec = time(NULL) ;
            pthread_mutex_unlock(&m_pcb);
            pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);

            log_debug(debug_logger,"antes de enviar pcb es, estimacion{%d}", pcb->estimacionRafaga);
            enviar_paquete_pcb(pcb, fd_cpu,PCB, info_logger);
            log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcb->id);

        }
        else{
            //ALGORITMO FIFO
            t_pcb *pcbReady = list_remove(colaReady,0);
            list_add(colaExec,pcbReady);
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

    calcular_estimacion(pcbBuscado);
    sem_post(&sem_cpuLibre);

}


void moverProceso_BloqReady(t_pcb* pcbBuscado){

    pthread_mutex_lock(&mutex_colaBloq);
    eliminarElementoLista(pcbBuscado, colaBloq);
    pthread_mutex_unlock(&mutex_colaBloq);


    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady, pcbBuscado);
    pthread_mutex_unlock(&mutex_ColaReady);
    log_info(info_logger, "PID: [%d] - Estado Anterior: BLOQ - Estado Actual: READY.", pcbBuscado->id);
    pthread_mutex_lock(&m_pcb);
    pcbBuscado->tiempoIngresoReady = time(NULL);
    pthread_mutex_unlock(&m_pcb);
    sem_post(&sem_procesosReady);
    mostrarEstadoColas();


}

void liberarRecursosTomados(t_pcb* pcb){
    for(int i = 0; i < list_size(pcb->recursosTomados); i++){
        t_recurso* recursoTomado = list_get(pcb->recursosTomados,i);
        pthread_mutex_lock(&semaforos_io[recursoTomado->indiceSemaforo]);
        recursoTomado->instanciasRecurso++;
        pthread_mutex_unlock(&semaforos_io[recursoTomado->indiceSemaforo]);
        log_info(info_logger,"PID:<%d> - libera Recurso:<%s> - Instancias <%d>", pcb->id, recursoTomado->nombreRecurso, recursoTomado->instanciasRecurso);
    }
    list_clean(pcb->recursosTomados);
    //No se hace la liberacion ya que no es correcto, los recursos pertenecen al sistema
    //la liberacion de los recursos se tiene que hacer en liberarPrograma
}

void moverProceso_ExecExit(t_pcb *pcbBuscado){

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: EXIT", pcbBuscado->id);
    sem_post(&sem_cpuLibre);

    pthread_mutex_lock(&mutex_colaExit);
    queue_push(colaExit,pcbBuscado);
    pthread_mutex_unlock(&mutex_colaExit);

    if(!list_is_empty(pcbBuscado->recursosTomados)){
        liberarRecursosTomados(pcbBuscado);
    }

    if(!list_is_empty(pcbBuscado->tablaArchivosAbiertos)){
        eliminarPcbTGAA_Y_actualizarTGAA(pcbBuscado);
    }

    sem_post(&sem_procesosExit);


}

void moverProceso_ExecReady(t_pcb* pcbBuscado){
    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    pthread_mutex_lock(&m_pcb);
    pcbBuscado->tiempoIngresoReady = time(NULL);
    pthread_mutex_unlock(&m_pcb);


    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbBuscado);
    pthread_mutex_unlock(&mutex_ColaReady);


    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: READY", pcbBuscado->id);
    calcular_estimacion(pcbBuscado);
    sem_post(&sem_procesosReady);
    sem_post(&sem_cpuLibre);
    mostrarEstadoColas();

}
