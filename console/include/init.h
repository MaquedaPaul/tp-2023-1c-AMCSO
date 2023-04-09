//
// Created by utnso on 4/7/23.
//

#ifndef TEMPLATE_INIT_H
#define TEMPLATE_INIT_H
#include <loggers_configs.h>
#include <string.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_console;
extern t_config_console *cfg_console;
extern char* path_config;

int argumentosInvalidos(int argc, char *argv[]);
void cargar_configuracion(char *path);

#endif //TEMPLATE_INIT_H
