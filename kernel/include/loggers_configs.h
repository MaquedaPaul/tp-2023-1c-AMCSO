//
// Created by utnso on 4/9/23.
//

#ifndef TPSO_LOGGERS_CONFIGS_H
#define TPSO_LOGGERS_CONFIGS_H

typedef struct
{
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *IP_FILESYSTEM;
    char *PUERTO_FILESYSTEM;
    char *IP_CPU;
    char *PUERTO_CPU;
    char *PUERTO_ESCUCHA;
    char *ALGORITMO_PLANIFICACION;
    int ESTIMACION_INICIAL;
    double HRRN_ALFA;
    int GRADO_MAX_MULTIPROGRAMACION;
    char** RECURSOS; //podemos usar LIST
    char** INSTANCIAS_RECURSOS; //podemos usar LIST
} t_config_kernel;
t_config_kernel *cfg_kernel;

int init_logs_configs(char *path_config);
t_config_kernel *cfg_kernel_start();



#endif //TPSO_LOGGERS_CONFIGS_H
