//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_INIT_H
#define TPSO_INIT_H
#include <loggers_configs.h>

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_filesystem;
extern t_config_filesystem *cfg_filesystem;
extern char* path_config;
int cargar_configuracion(char *path);

#endif //TEMPLATE_INIT_H
