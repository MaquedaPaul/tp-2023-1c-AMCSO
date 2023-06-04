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
#include <init.h>
#include <plan_aux.h>

extern t_log* logger_kernel;
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

void* conectarConCPU();
bool generarConexionesConCPU();
bool atenderCpu();

void* conectarConMemoria();
bool generarConexionesConMemoria();
bool atenderMemoria();

void* conectarConFileSystem();
bool generarConexionesConFilesystem();
bool atenderFilesystem();

void cerrar_servers();
void cortar_conexiones();

void waitRecursoPcb(t_recurso* recurso, t_pcb* unaPcb);
void signalRecursoPcb(t_recurso* recurso,t_pcb* unaPcb);
void manejoDeRecursos(t_pcb* unaPcb,char* orden);

t_list* recibirListaInstrucciones(int socket_cliente);
void creacionSegmentoExitoso(t_pcb* unaPcb, uint32_t* array);


#endif //TEMPLATE_COMUNICACION_H
