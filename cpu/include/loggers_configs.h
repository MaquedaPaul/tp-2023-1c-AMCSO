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
    int RETARDO_INSTRUCCION;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_ESCUCHA;
    int TAM_MAX_SEGMENTO;
}t_config_cpu;


int init_logs_configs(char *path_config);
t_config_cpu *cfg_cpu_start();
bool checkProperties(char *path);


#endif //TPSO_LOGGERS_CONFIGS_H
