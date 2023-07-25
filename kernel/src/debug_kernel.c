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
    list_iterate(cola, guardarIds);
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
    list_iterate(recursos, guardarNombres);
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
    mostrarEstadoRecursosAux("Lista Recursos:", listaRecursos);
    mostrarEstadoRecursosAux("Estado Block Recursos:", estadoBlockRecursos);
    log_debug(debug_logger,"TERMINO DE IMPRIMIR");
    pthread_mutex_unlock(&mutex_debug_logger);
}