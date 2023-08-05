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
#include <filesystem.h>

extern pthread_mutex_t mutex_cliente_socket;
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_filesystem;
extern t_config_filesystem *cfg_filesystem;
extern char* path_config;
extern int fd_memoria;
extern int fd_kernel;


bool generar_conexiones();
void* crearServidor();

int server_escuchar(t_log *logger, char *server_name, int server_socket);
void procesar_conexion(void *void_args);

void* conectarConMemoria();
bool generarConexionesConMemoria();

bool atenderMemoria();
void atender_peticiones();


void manejar_peticion(t_peticion* peticion);
t_peticion* crear_peticion(t_operacion_fs operacion, char* nombre, uint32_t tamanio, uint32_t dir_fisica, uint32_t puntero, uint32_t pid, void* datos);
void agregarPeticionAPendientes(t_peticion* peticion);
t_peticion* sacoPeticionDePendientes();


#endif //TEMPLATE_COMUNICACION_H
