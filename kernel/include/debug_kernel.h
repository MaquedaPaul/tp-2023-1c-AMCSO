//
// Created by utnso on 7/25/23.
//

#ifndef KERNEL_DEBUG_KERNEL_H
#define KERNEL_DEBUG_KERNEL_H
#include <init.h>
//COLAS
extern t_queue* colaNew;
extern t_list* colaExec;
extern t_queue* colaExit;
extern t_list* colaBloq;
extern t_list* estadoBlockRecursos;
extern t_queue* colaReady_FIFO; //en caso de FIFO
extern t_list* colaReady; //en caso de HRRN
extern t_list* listaEsperaMemoria;
extern pthread_mutex_t mutex_debug_logger;

void mostrarEstadoColas();
void mostrarEstadoRecursos();
#endif //KERNEL_DEBUG_KERNEL_H
