//
// Created by utnso on 4/9/23.
//

#ifndef TPSO_LOGGERS_CONFIGS_H
#define TPSO_LOGGERS_CONFIGS_H
#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <config_utils.h>

typedef struct
{
    char *PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_SEGMENTO_0;
    int CANT_SEGMENTOS;
    int RETARDO_MEMORIA;
    int RETARDO_COMPACTACION;
    char *ALGORITMO_ASIGNACION;
} t_config_memory;


int init_logs_configs(char *path_config);
t_config_memory *cfg_memory_start();
bool checkProperties(char *path);

void creacionProceso(int pid);
void eliminacionProceso(int pid);
void creacionSegmento(int pid, int idSegmento, uint32_t direccionBase, uint32_t tamanio);
void eliminacionSegmento(int pid, int idSegmento, uint32_t direccionBase, uint32_t tamanio);
void inicioCompactacion();
void resultadoCompactacion(uint32_t pid, uint32_t idSegmento, uint32_t direccionBase, uint32_t tamanio);
void accesoEspacioUsuarioEscrituraCPU(int pid, uint32_t direccionFisica, uint32_t tamanio);
void accesoEspacioUsuarioLecturaCPU(int pid, uint32_t direccionFisica, uint32_t tamanio);
void accesoEspacioUsuarioEscrituraFS(int pid, uint32_t direccionFisica, uint32_t tamanio);
void accesoEspacioUsuarioLecturaFS(int pid, uint32_t direccionFisica, uint32_t tamanio);
void accesoEspacioUsuarioEscrituraRetardoPrevio(uint32_t direccion, uint32_t pid);
void accesoEspacioUsuarioEscrituraRetardoConcedido();
void accesoEspacioUsuarioLecturaRetardoPrevio(uint32_t direccion, uint32_t tamanio, uint32_t pid);
void accesoEspacioUsuarioLecturaRetardoConcedido();

void compactacionRetardoPrevio(uint32_t tiempo);
void compactacionRetardoTerminada();
#endif //TPSO_LOGGERS_CONFIGS_H
