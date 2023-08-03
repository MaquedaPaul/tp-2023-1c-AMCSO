//
// Created by utnso on 7/25/23.
//

#ifndef KERNEL_DEBUG_KERNEL_H
#define KERNEL_DEBUG_KERNEL_H
#include <init.h>

extern bool recursosCargados;
extern bool estadosCargados;
extern bool semaforosCargados;
extern bool tablasFsCargadas;
extern bool semaforoDinamicoCargado;
extern bool logsCreados;
extern bool configCreado;
extern bool cfgCreado;

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


extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_kernel;
extern t_config_kernel *cfg_kernel;
extern char* path_config;

void mostrarEstadoColas();
void mostrarEstadoRecursos();

void liberarRecurso(t_recurso* recurso);
void liberarRecursosKernel();
void liberarEstadosKernel();
void liberarSemaforos();
void liberarManejoFs();
void liberarSemaforoDinamico();
void destruirConfig();
void destruirCfg();
void destruirLoggers();

#endif //KERNEL_DEBUG_KERNEL_H
