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
extern t_config* file_cfg_kernel;
extern t_config_kernel *cfg_kernel;
extern char* path_config;

static void procesar_conexion(void *void_args);
void* crearServidor();
int server_escuchar(t_log *logger, char *server_name, int server_socket);
bool generar_conexiones();

void* conectarConCPU();
bool generarConexionesConCPU();
bool atenderCpu();

void* conectarConMemoria();
bool generarConexionesConMemoria();
bool atenderMemoria();

void* conectarConFileSystem();
bool generarConexionesConFilesystem();
bool atenderFilesystem();


#endif //TEMPLATE_COMUNICACION_H
