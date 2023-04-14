//
// Created by utnso on 4/9/23.
//

#ifndef TPSO_LOGGERS_CONFIGS_H
#define TPSO_LOGGERS_CONFIGS_H
#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <config_utils.h>

typedef struct
{
    char *PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_SEGMENTO_0;
    int CANT_SEGMENTOS;
    int RETARDO_MEMORIA;
    int RETARDO_COMPACTACION;
    char *ALGORITMO_ASIGNACION;
} t_config_memory;


int init_logs_configs(char *path_config);
t_config_memory *cfg_memory_start();
bool checkProperties(char *path);


#endif //TPSO_LOGGERS_CONFIGS_H
