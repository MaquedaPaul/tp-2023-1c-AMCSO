//
// Created by utnso on 4/29/23.
//

#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H
#include <stdbool.h>
#include <protocolo.h>
#include <loggers_configs.h>
#include <gestion_filesystem.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;

extern t_list* archivosUsados;

void* abrirArchivo(void* cliente_socket);
void* crearArchivo(void* cliente_socket);
void* truncarArchivo(void*  cliente_socket);
void* leerArchivo(void*  cliente_socket);
void* escribirArchivo(void*  cliente_socket);

void* finalizarEscrituraArchivo(void*  cliente_socket);
void* finalizarLecturaArchivo(void*  cliente);
char* obtenerUltimoArchivoUsado();
#endif //FILESYSTEM_FILESYSTEM_H
