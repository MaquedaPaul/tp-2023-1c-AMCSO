//
// Created by utnso on 5/1/23.
//

#ifndef CONSOLE_CLEAN_CONSOLE_H
#define CONSOLE_CLEAN_CONSOLE_H
#include <signal.h>
#include <pthread.h>
#include <init.h>
#include <loggers_configs.h>
void cerrar_programa();
extern bool logsCreados;
extern bool configCreado;
extern bool cfgCreado;
extern bool conexionesHechas;

void destruirLoggers();
void destruirConfig();
void destruirCfg();

#endif //CONSOLE_CLEAN_CONSOLE_H
