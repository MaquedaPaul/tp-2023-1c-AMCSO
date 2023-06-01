//
// Created by utnso on 4/7/23.
//

#ifndef TEMPLATE_COMUNICACION_H
#define TEMPLATE_COMUNICACION_H
#include <sockets.h>
#include <protocolo.h>
#include <estructuras.h>
#include <loggers_configs.h>
#include <pthread.h>
#include <memory.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_memory;
extern t_config_memory *cfg_memory;
extern char* path_config;
extern uint32_t ipCpu;
extern uint32_t ipFs;
bool generar_conexiones();
void* crearServidor();
void* conectarConCPU();
void* conectarConMemoria();
int server_escuchar(t_log *logger, char *server_name, int server_socket);
void procesar_conexion(void *void_args);

#endif //TEMPLATE_COMUNICACION_H
