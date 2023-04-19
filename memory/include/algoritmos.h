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

t_segmento* firstFit();
t_segmento* bestFit();
t_segmento* worstFit();
t_segmento* buscarHuecoLibre(uint32_t tamanioSegmento);
#endif //TEMPLATE_ALGORITMOS_H
