//
// Created by utnso on 4/29/23.
//

#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H
#include <stdbool.h>
#include <protocolo.h>
#include <loggers_configs.h>
#include <gestion_filesystem.h>


extern pthread_mutex_t mutex_cliente_socket;
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_list* peticiones_pendientes;
extern  t_list* archivosUsados;
void* abrirArchivo(void* cliente_socket);
void* crearArchivo(void* cliente_socket);
void* truncarArchivo(void*  cliente_socket);
void* leerArchivo(void*  cliente_socket);
void* escribirArchivo(void*  cliente_socket);

void* finalizarEscrituraArchivo(void*  cliente_socket);
void* finalizarLecturaArchivo(void*  cliente);
char* obtenerPrimerArchivoUsado();


void ejecutar_fopen(char* nombreArchivo);
void ejecutarCreacionArchivo(char* nombreArchivo);
void ejecutarFwrite(char* nombreArchivo, uint32_t pid, uint32_t puntero, uint32_t tamanio, uint32_t direcFisica);
void ejecutarFread(char* nombreArchivo, uint32_t pid, uint32_t puntero, uint32_t tamanio, uint32_t direcFisica);
void ejecutarFtruncate(char* nombreArchivo, uint32_t tamanioNuevo);
void ejecutar_finalizarEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, void* datos);
void ejecutar_finalizarLecturaArchivo(char* nombreArchivo);

        typedef enum{
            EJECUTAR_OPEN,
            EJECUTAR_CREACION,
            EJECUTAR_TRUNCATE,
            EJECUTAR_READ,
            EJECUTAR_WRITE,
            EJECUTAR_RESPUESTA_LECTURA,
            EJECUTAR_RESPUESTA_ESCRITURA
        }t_operacion_fs;

        typedef struct{
            t_operacion_fs operacion;
            char* nombre;
            uint32_t tamanio;
            uint32_t direccionFisica;
            uint32_t puntero;
            uint32_t pid;
            void* datos;
        }t_peticion;





#endif //FILESYSTEM_FILESYSTEM_H
