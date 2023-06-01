//
// Created by utnso on 4/29/23.
//

#ifndef FILESYSTEM_GESTION_FILESYSTEM_H
#define FILESYSTEM_GESTION_FILESYSTEM_H
#include <stdbool.h>
#include <stdint.h>
#include <commons/bitarray.h>
typedef struct
{
    void* archivo;
    int fd;
}t_bitmapBloques;

typedef struct
{
    void* archivo;
    int fd;
}t_bloques;


typedef struct
{
    int BLOCK_SIZE;
    int BLOCK_COUNT;
} t_config_superbloque;

typedef struct
{
    char* NOMBRE_ARCHIVO;
    int TAMANIO_ARCHIVO;
    int PUNTERO_DIRECTO;
    int PUNTERO_INDIRECTO;
} t_config_fcb;

bool existeFcbConNombre(char* nombreArchivo);
void realizarCreacionArchivo(char* nombreArchivo);
void realizarTruncacionArchivo(char* nombreArchivo);
void* realizarLecturaArchivo(char* nombreArchivo, uint32_t puntero, uint32_t  tamanio);
void realizarEscrituraArchivo(void* datos, uint32_t tamanioDatos);


#endif //FILESYSTEM_GESTION_FILESYSTEM_H
