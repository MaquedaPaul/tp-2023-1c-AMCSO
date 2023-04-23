#ifndef KERNEL_CONFIG_H
#define KERNEL_CONFIG_H

#include <protocolo.h>

typedef struct KernelConfig
{
    char *IP;
    char *IP_CPU;
    char *PUERTO_CPU;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *IP_FILESYSTEM;
    char *PUERTO_FILESYSTEM;
    char *PUERTO_ESCUCHA;
    char *ALGORITMO_PLANIFICACION;
    char *GRADO_MAX_MULTIPROGRAMACION;
    char *ESTIMACION_INICIAL;
    char *HRRN_ALFA;
    char **RECURSOS;
    char **INSTANCIAS_RECURSOS;
} KernelConfig;

extern KernelConfig KERNEL_CONFIG;

void rellenar_configuracion_kernel(Config *config);

#endif