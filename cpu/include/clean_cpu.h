//
// Created by utnso on 5/5/23.
//

#ifndef CPU_CLEAN_CPU_H
#define CPU_CLEAN_CPU_H
#include <commons/collections/list.h>
#include <estructuras.h>
#include <comunicacion.h>
#include <loggers_configs.h>
#include <signal.h>
extern bool logsCreados;
extern bool configCreado;
extern bool cfgCreado;
extern bool conexionesHechas;
extern bool semaforosCreados;

void cerrar_programa();

void destruirLoggers();
void destruirConfig();
void destruirCfg();
void destruirConexiones();
void destruirSemaforos();

#endif //CPU_CLEAN_CPU_H
