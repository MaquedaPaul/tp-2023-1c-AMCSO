//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_COMUNICACION_H
#define TPSO_COMUNICACION_H

#include <sockets.h>
#include <protocolo.h>
#include <loggers_configs.h>
#include <pthread.h>
#include <cpu.h>
#include <commons/string.h>

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_cpu;
extern t_config_cpu *cfg_cpu;
extern char* path_config;

extern int fd_cpu;
extern char* ip_cpu;
extern char* puerto_cpu;
extern int fd_memoria;

void procesar_conexion(void *void_args);
int server_escuchar(t_log *logger, char *server_name, int server_socket);
bool generar_conexiones();
void* crearServidor();

void* conectarConMemoria();
bool generarConexionesConMemoria();
bool atenderMemoria();

#endif //TEMPLATE_COMUNICACION_H
