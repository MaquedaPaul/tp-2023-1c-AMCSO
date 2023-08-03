//
// Created by utnso on 7/25/23.
//

#include <debug_kernel.h>
#include <commons/string.h>

void mostrarEstadoColasAux(char* colaMsg, t_list* cola){
    char* idsPcb = string_new();
    void guardarIds(t_pcb* unaPcb){
        char* unId = string_itoa(unaPcb->id);
        string_append(&idsPcb, unId);
        string_append(&idsPcb, ", ");
    }

    log_debug(debug_logger,"%s", colaMsg);
    string_append(&idsPcb, "[");
    if(!list_is_empty(cola)){
        list_iterate(cola, guardarIds);
    }
    string_append(&idsPcb, "]");
    log_debug(debug_logger,"%s", idsPcb);
    free(idsPcb);

}


void mostrarEstadoColas(){


    pthread_mutex_lock(&mutex_debug_logger);
    log_debug(debug_logger,"COMIENZO A IMPRIMIR");
    mostrarEstadoColasAux("COLA BLOQ", colaBloq);
    mostrarEstadoColasAux("COLA EXEC", colaExec);
    mostrarEstadoColasAux("COLA READY", colaReady);
    log_debug(debug_logger,"TERMINO DE IMPRIMIR");
    pthread_mutex_unlock(&mutex_debug_logger);

}

void mostrarEstadoRecursosAux(char* msg, t_list* recursos){
    char* nombresRecursos = string_new();
    void guardarNombres(t_recurso* unRecurso){
        char* unNombre = unRecurso->nombreRecurso;
        string_append(&nombresRecursos, unNombre);
        string_append(&nombresRecursos, ", ");
    }

    log_debug(debug_logger,"%s", msg);
    string_append(&nombresRecursos, "[");
    if(!list_is_empty(recursos)){
        list_iterate(recursos, guardarNombres);
    }
    string_append(&nombresRecursos, "]");
    log_debug(debug_logger,"%s", nombresRecursos);
    free(nombresRecursos);

}


void mostrarEstadoRecursos(){

    void mostrarRecursos(t_recurso* unRecurso){
        log_debug(debug_logger, "RECURSO: %s", unRecurso->nombreRecurso);
    }
    pthread_mutex_lock(&mutex_debug_logger);
    log_debug(debug_logger,"COMIENZO A IMPRIMIR");
    mostrarEstadoRecursosAux("Estado Block Recursos:", estadoBlockRecursos);
    log_debug(debug_logger,"TERMINO DE IMPRIMIR");
    pthread_mutex_unlock(&mutex_debug_logger);
}

void liberarRecurso(t_recurso* recurso){
    list_destroy_and_destroy_elements(recurso->cola,liberarPcb);
    free(recurso);
}

void liberarRecursosKernel(){
    list_destroy_and_destroy_elements(estadoBlockRecursos,liberarRecurso);
    log_trace(trace_logger,"Se eliminaron los recursos de kernel");
}

void liberarEstadosKernel(){
    pthread_mutex_lock(&mutex_colaNew);
    queue_destroy_and_destroy_elements(colaNew,liberarPcb);
    pthread_mutex_unlock(&mutex_colaNew);

    pthread_mutex_lock(&mutex_ColaReady);
    list_destroy_and_destroy_elements(colaReady,liberarPcb);
    pthread_mutex_unlock(&mutex_ColaReady);

    pthread_mutex_lock(&mutex_colaBloq);
    list_destroy_and_destroy_elements(colaBloq,liberarPcb);
    pthread_mutex_unlock(&mutex_colaBloq);

    //La liberacion de la ColaEstadoBlockRecurso la hacemos cuando liberamos los recursos

    pthread_mutex_lock(&mutex_colaExec);
    list_destroy_and_destroy_elements(colaExec,liberarPcb);
    pthread_mutex_unlock(&mutex_colaExec);

    pthread_mutex_lock(&mutex_colaExit);
    queue_destroy_and_destroy_elements(colaExit,liberarPcb);
    pthread_mutex_unlock(&mutex_colaExit);

    log_trace(trace_logger,"Se eliminaron los estados de kernel");
}

void liberarSemaforos(){

    pthread_mutex_destroy(&mutex_TGAA);
    pthread_mutex_destroy(&mutex_listaPeticionesArchivos);

    pthread_mutex_destroy(&mutex_colaNew);
    pthread_mutex_destroy(&mutex_ColaReady);
    pthread_mutex_destroy(&mutex_colaExec);
    pthread_mutex_destroy(&mutex_colaBloq);
    pthread_mutex_destroy(&mutex_colaExit);
    pthread_mutex_destroy(&mutex_MP);
    pthread_mutex_destroy(&mutex_debug_logger);

    sem_destroy(&sem_procesosEnNew);
    sem_destroy(&sem_procesosReady);
    sem_destroy(&sem_procesosExit);

    log_trace(trace_logger,"Se eliminaron los semaforos");
}

void liberarArchivoPeticion(t_archivoPeticion* archivoPeticion){
    free(archivoPeticion->archivo);
    free(archivoPeticion);
}

void liberarManejoFs(){
    list_destroy_and_destroy_elements(tablaGlobal_ArchivosAbiertos,liberarArchivoPeticion);
    list_destroy_and_destroy_elements(listaPeticionesArchivos,free); //solo free pq el archivo ya se libera arriba

    log_trace(trace_logger,"Se elimino las tablas para el manejo de FS");
}

void liberarSemaforoDinamico(){
    int dim = tamanioArray(cfg_kernel->RECURSOS);
    for(int i = 0; i < dim; i++){
        pthread_mutex_destroy(&semaforos_io[i]);
    }
    log_trace(trace_logger,"Se elimino el semaforo dinamico");
}

void destruirConfig(){
    config_destroy(file_cfg_kernel);
    log_trace(trace_logger,"Se libera el file config");
}

void destruirCfg(){
    log_trace(trace_logger,"Se libera la struct cfg");
    free(cfg_kernel->IP_MEMORIA);
    free(cfg_kernel->PUERTO_MEMORIA);
    free(cfg_kernel->IP_FILESYSTEM);
    free(cfg_kernel->PUERTO_FILESYSTEM);
    free(cfg_kernel->IP_CPU);
    free(cfg_kernel->PUERTO_CPU);
    free(cfg_kernel->PUERTO_ESCUCHA);
    free(cfg_kernel->ALGORITMO_PLANIFICACION);
    string_array_destroy(cfg_kernel->RECURSOS);
    string_array_destroy(cfg_kernel->INSTANCIAS_RECURSOS);
    free(cfg_kernel);

}

void destruirLoggers(){
    log_destroy(info_logger);
    log_destroy(warning_logger);
    log_destroy(error_logger);
    log_destroy(debug_logger);
    log_trace(trace_logger,"Se liberan loggers");
    log_destroy(trace_logger);
}