//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_INIT_H
#define TPSO_INIT_H
#include <loggers_configs.h>
#include <memory.h>
#include <pthread.h>
extern void* espacio_contiguo;
extern t_list* tablasSegmentos;
extern t_list* huecosUsados;
extern t_list* huecosLibres;
extern t_segmento * segmento0;
extern int cantidadMaximaSegmentos;
extern uint32_t espacioDisponible;
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_memory;
extern t_config_memory *cfg_memory;
extern uint32_t idDisponible;
extern char* path_config;

extern pthread_mutex_t mutex_espacioContiguo;
extern pthread_mutex_t mutex_tablasSegmentos;
extern pthread_mutex_t mutex_huecosUsados;
extern pthread_mutex_t mutex_huecosDisponibles;
extern pthread_mutex_t mutex_idSegmento;
extern pthread_mutex_t mutex_espacioDisponible;



int cargar_configuracion(char *path);
void crearEstructurasAdministrativas();

bool crearEspacioContiguoDeMemoria();
bool crearTablasSegmentos();
bool crearListaHuecosLibres();
bool crearListaHuecosUsados();
bool crearSegmento0();
bool crearSemaforos();

#endif //TEMPLATE_INIT_H
