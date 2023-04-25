#ifndef TPSO_LOGGERS_INIT_H
#define TPSO_LOGGERS_INIT_H

#include <loggers_configs.h>
#include <semaphore.h>
#include <pthread.h>
#include <comunicacion.h>
#include <planificacion.h>
#include <commons/log.h>



extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_kernel;
extern t_config_kernel *cfg_kernel;
extern char* path_config;

extern t_log* logger_kernel;

//COLAS
extern t_queue* colaNew;
extern t_list* colaExec;
extern t_queue* colaExit;
extern t_list* colaBloq;
extern t_list* estadoBlockRecursos;
extern t_queue* colaReady_FIFO;
extern t_list* colaReady; 

//CONTADORES Y MUTEX
extern int procesosEnNew;
extern pthread_mutex_t mutex_colaNew;
extern pthread_mutex_t mutex_ColaReady; //en caso de FEEDBACK cola 1(RR)
extern pthread_mutex_t mutex_colaExec;
extern pthread_mutex_t mutex_colaBloq;

//HILOS
extern pthread_t conexion_con_consola;
extern pthread_t conexion_con_cpu;
extern pthread_t conexion_con_memoria;
extern pthread_t conexion_con_filesystem;

//Manejo de recursos
extern t_list* listaRecursos;
extern sem_t* semaforos_io;

int cargar_configuracion(char *path);
void inicializar_kernel();
Logger *iniciar_logger_kernel();
int tamanioArray(char** array);
void iniciarSemaforoDinamico(sem_t* semaforo, int dim);
int cargarRecursos();
void cerrar_programa();


#endif