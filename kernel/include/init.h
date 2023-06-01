//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_INIT_H
#define TPSO_INIT_H
#include <loggers_configs.h>
#include <semaphore.h>
#include <pthread.h>
#include <comunicacion.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_kernel;
extern t_config_kernel *cfg_kernel;
extern char* path_config;

int cargar_configuracion(char *path);
bool activar_kernel();
#endif //TEMPLATE_INIT_H
