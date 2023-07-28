//
// Created by utnso on 7/27/23.
//

#ifndef MEMORY_DEBUG_MEMORY_H
#define MEMORY_DEBUG_MEMORY_H
#include <gestion_memoria.h>
extern void* espacio_contiguo;
extern t_list* huecosUsados;
extern t_list* huecosLibres;
extern t_list* tablasSegmentos;
void mostrarSegmentoLibre(t_segmento* unSegmento);
void mostrarSegmentoUsado(t_segmento* unSegmento);
void mostrarListaSegmentos(t_list* unSegmento);
void mostrarMemoria();
void mostrarListaLibres();
void mostrarListaUsados();
void mostrarPosicionMemoria(uint32_t posicion, int cantidad);
void mostrarTablasDeSegmentos();

#endif //MEMORY_DEBUG_MEMORY_H
