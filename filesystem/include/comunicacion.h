//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_COMUNICACION_H
#define TPSO_COMUNICACION_H
#include <sockets.h>
#include <protocolo.h>
#include <estructuras.h>
#include <loggers_configs.h>
#include <pthread.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_filesystem;
extern t_config_filesystem *cfg_filesystem;
extern char* path_config;

bool generar_conexiones();
void* crearServidor();
//void* conectarConCPU();
//void* conectarConMemoria();
int server_escuchar(t_log *logger, char *server_name, int server_socket);
void procesar_conexion(void *void_args);

#endif //TEMPLATE_COMUNICACION_H
