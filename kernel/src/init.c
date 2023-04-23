//
// Created by utnso on 4/7/23.
//

#include <init.h>


//COLAS
t_queue* colaNew;
t_list* colaExec;
t_queue* colaExit;
t_list* colaBloq;
t_queue* colaReady_FIFO; //en caso de FIFO
t_list* colaReady; //en caso de HRRN


//CONTADORES Y MUTEX
int procesosEnNew = 0;
pthread_mutex_t mutex_colaNew;
pthread_mutex_t mutex_ColaReady; 
pthread_mutex_t mutex_colaExec;
pthread_mutex_t mutex_colaBloq;

int cargar_configuracion(char *path) {

    file_cfg_kernel = config_create(path);
    cfg_kernel->IP_MEMORIA = strdup(config_get_string_value(file_cfg_kernel, "IP_MEMORIA"));
    log_trace(trace_logger, "IP_MEMORIA Cargada Correctamente: %s", cfg_kernel->IP_MEMORIA);

    cfg_kernel->PUERTO_MEMORIA = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_MEMORIA"));
    log_trace(trace_logger, "PUERTO_MEMORIA Cargada Correctamente: %s", cfg_kernel->PUERTO_MEMORIA);

    cfg_kernel->IP_FILESYSTEM = strdup(config_get_string_value(file_cfg_kernel, "IP_FILESYSTEM"));
    log_trace(trace_logger, "IP_FILESYSTEM Cargada Correctamente: %s", cfg_kernel->IP_FILESYSTEM);

    cfg_kernel->PUERTO_FILESYSTEM = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_FILESYSTEM"));
    log_trace(trace_logger, "PUERTO_FILESYSTEM Cargada Correctamente: %s", cfg_kernel->PUERTO_FILESYSTEM);


    cfg_kernel->IP_CPU = strdup(config_get_string_value(file_cfg_kernel, "IP_CPU"));
    log_trace(trace_logger, "IP_CPU Cargada Correctamente: %s", cfg_kernel->IP_CPU);

    cfg_kernel->PUERTO_CPU = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_CPU"));
    log_trace(trace_logger, "PUERTO_CPU Cargada Correctamente: %s", cfg_kernel->PUERTO_CPU);

    cfg_kernel->PUERTO_ESCUCHA = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_ESCUCHA"));
    log_trace(trace_logger, "PUERTO_ESCUCHA Cargada Correctamente: %s", cfg_kernel->PUERTO_ESCUCHA);

    cfg_kernel->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(file_cfg_kernel, "ALGORITMO_PLANIFICACION"));
    log_trace(trace_logger, "ALGORITMO_PLANIFICACION Cargada Correctamente: %s", cfg_kernel->ALGORITMO_PLANIFICACION);

    cfg_kernel->ESTIMACION_INICIAL = config_get_int_value(file_cfg_kernel, "ESTIMACION_INICIAL");
    log_trace(trace_logger, "ESTIMACION_INICIAL Cargada Correctamente: %d",
             cfg_kernel->ESTIMACION_INICIAL);

    cfg_kernel->HRRN_ALFA = config_get_double_value(file_cfg_kernel, "HRRN_ALFA");
    log_trace(trace_logger, "HRRN_ALFA Cargada Correctamente: %f",
             cfg_kernel->HRRN_ALFA);


    cfg_kernel->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(file_cfg_kernel, "GRADO_MAX_MULTIPROGRAMACION");
    log_trace(trace_logger, "GRADO_MAX_MULTIPROGRAMACION Cargada Correctamente: %d",
             cfg_kernel->GRADO_MAX_MULTIPROGRAMACION);


    cfg_kernel->RECURSOS = config_get_array_value(file_cfg_kernel, "RECURSOS");
    //log_trace(logger_kernel, "DISPOSITIVOS_IO Cargada Correctamente: %s", cfg_kernel->DISPOSITIVOS_IO);

    cfg_kernel->INSTANCIAS_RECURSOS = config_get_array_value(file_cfg_kernel, "INSTANCIAS_RECURSOS");
    //log_trace(logger_kernel, "TIEMPOS_IO Cargada Correctamente: %s", cfg_kernel->TIEMPOS_IO);


    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");

    config_destroy(file_cfg_kernel);
    return true;
}

void inicializar_kernel(){

//COLAS
    colaNew = queue_create();
    colaReady = list_create();
    colaExec = list_create();
    colaBloq = list_create();
    colaExit = queue_create();

//CONTADORES Y MUTEX

    pthread_mutex_init(&mutex_colaNew, NULL);
    pthread_mutex_init(&mutex_ColaReady, NULL);
    pthread_mutex_init(&mutex_colaExec, NULL);
    pthread_mutex_init(&mutex_colaBloq, NULL);

 //HILOS
    pthread_create(&conexion_con_consola, NULL,(void*)crearServidor, NULL);
    pthread_create(&conexion_con_cpu, NULL, (void*)conectarConCPU, NULL);
    pthread_create(&conexion_con_memoria, NULL, (void*)conectarConMemoria, NULL);
    pthread_create(&conexion_con_filesystem, NULL, (void*)conectarConFileSystem, NULL);

    pthread_join(conexion_con_consola, NULL);
    pthread_join(conexion_con_cpu, NULL);
    pthread_join(conexion_con_memoria, NULL);
    pthread_join(conexion_con_filesystem, NULL);


return true;

}

Logger *iniciar_logger_kernel()
{
  return log_create("Kernel.log", "Kernel", 1, LOG_LEVEL_INFO);
}
