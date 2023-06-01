//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_ALGORITMOS_H
#define TPSO_ALGORITMOS_H
#include <stdint.h>
#include <loggers_configs.h>
#include <protocolo.h>
#include <gestion_memoria.h>
extern t_config_memory *cfg_memory;
extern t_list* tablasSegmentos;
extern t_list* huecosUsados;
extern t_list* huecosLibres;

t_segmento* firstFit(uint32_t tamanioSegmento);
t_segmento* bestFit(uint32_t tamanioSegmento);
t_segmento* worstFit(uint32_t tamanioSegmento);
t_segmento* buscarHuecoLibre(uint32_t tamanioSegmento);
#endif //TEMPLATE_ALGORITMOS_H
