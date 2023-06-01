//
// Created by utnso on 4/27/23.
//

#ifndef MEMORY_CLEAN_MEMORY_H
#define MEMORY_CLEAN_MEMORY_H
#include <commons/collections/list.h>
#include <estructuras.h>
#include <gestion_memoria.h>
#include <comunicacion.h>
#include <loggers_configs.h>
#include <clean_memory_utils.h>

extern bool logsCreados;
extern bool configCreado;
extern bool cfgCreado;
extern bool conexionesHechas;
extern bool memoriaInicializada;
extern bool listaDisponiblesCreada;
extern bool listaUsadosCreada;
extern bool tablaSegmentosCreada;
extern bool segmento0Creado;
extern bool semaforosCreados;

void cerrar_programa();

void destruirLoggers();
void destruirConfig();
void destruirCfg();
void destruirConexiones();
void destruirMemoria();
void destruirDisponibles();
void destruirUsados();
void destruirTablaSegmentos();
void destruirSegmento0();
void destruirSemaforos();
#endif //MEMORY_CLEAN_MEMORY_H
