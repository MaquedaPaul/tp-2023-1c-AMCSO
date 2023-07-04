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

void abrirArchivo(int cliente_socket);

void crearArchivo(int cliente_socket);

void truncarArchivo(int cliente_socket);

void leerArchivo(int cliente_socket);

void escribirArchivo(int cliente_socket);

void finalizarEscrituraArchivo(int cliente_socket);
void  finalizarLecturaArchivo(int cliente);
#endif //FILESYSTEM_FILESYSTEM_H
