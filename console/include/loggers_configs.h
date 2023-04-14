//
// Created by utnso on 4/8/23.
//

#ifndef TEMPLATE_LOGGERS_CONFIGS_H
#define TEMPLATE_LOGGERS_CONFIGS_H
#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>
#include <config_utils.h>
typedef struct
{
    char *IP_KERNEL;
    char *PUERTO_KERNEL;
} t_config_console;

t_config_console *cfg_console_start();
bool init_logs_configs(char* path_config);
bool checkProperties(char *path);



#endif //TEMPLATE_LOGGERS_CONFIGS_H
