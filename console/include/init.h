//
// Created by utnso on 4/7/23.
//

#ifndef TEMPLATE_INIT_H
#define TEMPLATE_INIT_H
#include <loggers_configs.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <sys/stat.h>
#include <estructuras.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_console;
extern t_config_console *cfg_console;
extern char* path_config;


bool cargar_configuracion(char *path);
t_list * crear_lista_de_instrucciones(char *path);
void closure_instrucciones(char *line);


#endif //TEMPLATE_INIT_H
