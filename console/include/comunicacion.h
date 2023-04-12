//
// Created by utnso on 4/7/23.
//

#ifndef TEMPLATE_COMUNICACION_H
#define TEMPLATE_COMUNICACION_H
#include <stdbool.h>
#include <sockets.h>
#include <protocolo.h>
#include <estructuras.h>
#include <loggers_configs.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_console;
extern t_config_console *cfg_console;
extern char* path_config;

bool generar_conexiones();

#endif //TEMPLATE_COMUNICACION_H
