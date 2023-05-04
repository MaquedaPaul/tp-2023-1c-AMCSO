//
// Created by utnso on 5/4/23.
//

#ifndef FILESYSTEM_CLEAN_FILESYSTEM_H
#define FILESYSTEM_CLEAN_FILESYSTEM_H
#include <init.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_filesystem;
extern t_config_filesystem *cfg_filesystem;
extern char* path_config;
extern int fd_memoria;

extern bool logsCreados;
extern bool configCreado;
extern bool cfgCreado;
extern bool conexionesHechas;
extern bool semaforosCreados;

void destruirLoggers();
void destruirConfig();
void destruirCfg();
void destruirConexiones();
void destruirSemaforos();

#endif //FILESYSTEM_CLEAN_FILESYSTEM_H
