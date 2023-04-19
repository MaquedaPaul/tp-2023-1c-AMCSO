//
// Created by utnso on 4/17/23.
//

#ifndef MEMORY_GESTION_MEMORIA_H
#define MEMORY_GESTION_MEMORIA_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <loggers_configs.h>

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_memory;
extern t_config_memory *cfg_memory;
typedef struct
{
    int pid;
    t_list* segmentos;
} t_tablaSegmentos;

typedef struct
{
    uint32_t base;
    uint32_t limite;
    uint8_t nro_segmento;
} t_segmento;

void escribirEnPosicion(uint32_t direccion);
bool hayDisponibilidadDeEspacio();
bool elEspacioSeEncuentraEnDiferentesHuecos();
uint32_t realizarCreacionSegmento(uint32_t pid, t_segmento* huecoLibre, uint32_t tamanio);
bool removerDeHuecosLibres(t_segmento* huecoLibre);
bool agregarAHuecosUsados(t_segmento* huecoLibre);
t_segmento* dividirEnDos(t_segmento* huecoLibre,uint32_t tamanio);
void realizarEliminacionSegmento(uint32_t direccion);
uint32_t realizarCompactacion();
void informarTablasActualizadas(uint32_t tablasActualizadas,int cliente_socket);

#endif //MEMORY_GESTION_MEMORIA_H