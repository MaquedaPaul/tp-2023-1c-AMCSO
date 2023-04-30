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
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PUERTO_ESCUCHA;
    char *PATH_SUPERBLOQUE;
    char *PATH_BITMAP;
    char *PATH_BLOQUES;
    char *PATH_FCB;
    int RETARDO_ACCESO_BLOQUE;
} t_config_filesystem;


int init_logs_configs(char *path_config);
t_config_filesystem *cfg_filesystem_start();
bool checkProperties(char *path);

void creacionArchivo(char* nombreArchivo);
void aperturaArchivo(char* nombreArchivo);
void truncacionArchivo(char* nombreArchivo, uint32_t tamanioArchivo);
void accesoABitmap(uint32_t numeroBloque, uint8_t estado);
void lecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t direccionMemoria, uint32_t tamanioArchivo);
void escrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t direccionMemoria, uint32_t tamanioArchivo);
void accesoABloqueArchivo(char* nombreArchivo, uint32_t numeroBloqueArchivo, uint32_t numeroBloqueFs);

#endif //TPSO_LOGGERS_CONFIGS_H
