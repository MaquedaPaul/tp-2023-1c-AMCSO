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
extern uint32_t  ipCpu;
extern pthread_mutex_t mutex_espacioContiguo;
extern pthread_mutex_t mutex_tablasSegmentos;
extern pthread_mutex_t mutex_huecosUsados;
extern pthread_mutex_t mutex_huecosDisponibles;
extern pthread_mutex_t mutex_idSegmento;
extern pthread_mutex_t mutex_espacioDisponible;



void inicializarProceso(int cliente_socket);


void finalizarProceso(int cliente_socket);
int finalizarProcesoConPid(uint32_t pid);

void realizarPedidoLectura(int cliente_socket);


void realizarPedidoEscritura(int cliente_socket);

void crearSegmento(int cliente_socket);

void eliminarSegmento(int cliente_socket);

void compactacionSegmentos(int cliente_socket);
#endif //MEMORY_MEMORY_H
