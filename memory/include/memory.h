//
// Created by utnso on 4/17/23.
//

#ifndef MEMORY_MEMORY_H
#define MEMORY_MEMORY_H
#include <stdint.h>
#include <loggers_configs.h>
#include <protocolo.h>
#include <gestion_memoria.h>
#include <algoritmos.h>
extern void* espacio_contiguo;
extern t_list* tablasSegmentos;
extern t_list* huecosUsados;
extern t_list* huecosLibres;
extern t_segmento* segmento0;
extern int cantidadMaximaSegmentos;
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_memory;
extern t_config_memory *cfg_memory;
extern char* path_config;



void inicializarProceso(int cliente_socket);


void finalizarProceso(int cliente_socket);
int finalizarProcesoConPid(uint32_t pid);

void realizarPedidoLectura(int cliente_socket);
int buscarValorEnPosicion(uint32_t posicion);

void realizarPedidoEscritura(int cliente_socket);

void crearSegmento(int cliente_socket);

void eliminarSegmento(int cliente_socket);

void compactacionSegmentos(int cliente_socket);
#endif //MEMORY_MEMORY_H
