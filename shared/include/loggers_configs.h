#ifndef LOGGERS_CONFIGS_H_
#define LOGGERS_CONFIGS_H_
#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <stdint.h> //Distintos tipos de INT
#include <stdio.h>  //Para incluir NULL
#include <errno.h>
extern t_log *logger_console;
extern t_config *file_cfg_console;

extern t_log *logger_kernel;
extern t_config *file_cfg_kernel;
extern int enteroCualquiera;

extern t_log *logger_cpu;
extern t_config *file_cfg_cpu;

extern t_log *logger_memory;
extern t_config *file_cfg_memory;

typedef struct
{
    char *IP_KERNEL;
    char *PUERTO_KERNEL;
    int *TIEMPO_PANTALLA; // es entero pero mejor leerlo como char*
    char **SEGMENTOS;
    int TAMANIO_LISTA_SEGMENTOS;
} t_config_console;
extern t_config_console *cfg_console;

typedef struct
{
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *IP_CPU;
    char *PUERTO_CPU_DISPATCH;
    char *PUERTO_CPU_INTERRUPT;
    char *PUERTO_ESCUCHA;
    char *ALGORITMO_PLANIFICACION;
    int GRADO_MAX_MULTIPROGRAMACION;
    int QUANTUM_RR;
    char** DISPOSITIVOS_IO;
    char** TIEMPOS_IO;
} t_config_kernel;
extern t_config_kernel *cfg_kernel;

typedef struct
{
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_ESCUCHA_DISPATCH;
    char *PUERTO_ESCUCHA_INTERRUPT;
    int RETARDO_INSTRUCCION;
    int ENTRADAS_TLB;
    char *REEMPLAZO_TLB;
} t_config_cpu;
extern t_config_cpu *cfg_cpu;

typedef struct
{
    char *PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_PAGINA;
    int ENTRADAS_POR_TABLA;
    int RETARDO_MEMORIA;
    char *ALGORITMO_REEMPLAZO;
    int MARCOS_POR_PROCESO;
    int RETARDO_SWAP;
    char *PATH_SWAP;
    int TAMANIO_SWAP;
} t_config_memory;
extern t_config_memory *cfg_memory;

static t_config_console *cfg_console_start()
// Inicializo en NULL para no tener basura
{
    t_config_console *cfg = malloc(sizeof(t_config_console));
    return cfg;
}

static t_config_kernel *cfg_kernel_start()
{
    t_config_kernel *cfg = malloc(sizeof(t_config_kernel));
    return cfg;
}

static t_config_cpu *cfg_cpu_start()
{
    t_config_cpu *cfg = malloc(sizeof(t_config_cpu));
    return cfg;
}

static t_config_memory *cfg_memory_start()
{
    t_config_memory *cfg = malloc(sizeof(t_config_memory));
    return cfg;
}

bool config_has_all_properties(t_config *cfg, char **properties);
t_config* iniciar_config(char* path_config);

#endif