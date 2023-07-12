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
    uint32_t BLOCK_SIZE;
    uint32_t BLOCK_COUNT;
} t_config_superbloque;

typedef struct
{
    char* NOMBRE_ARCHIVO;
    uint32_t TAMANIO_ARCHIVO;
    uint32_t PUNTERO_DIRECTO;
    uint32_t PUNTERO_INDIRECTO;
} t_config_fcb;

bool existeFcbConNombre(char* nombreArchivo);
t_config_fcb* buscarFCBporNombre(char* nombre);
void realizarCreacionArchivo(char* nombreArchivo);
void realizarTruncacionArchivo(char* nombreArchivo);
void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t  tamanio);
void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos);

int numeroDeBloque(uint32_t punteroArchivo);
int buscarPosicionDentroDelBloque(uint32_t puntero, uint32_t numeroBloque);
void* leer_archivo(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb);
void escribirBloque(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* pcb);

uint32_t cantidadDisponibleDelBloque(uint32_t puntero);
uint32_t cantidadBytesQueFaltaOperar(uint32_t nuevoTamanioALeer);
uint32_t buscarPunteroInicioDelBloque(int numeroBloque, t_config_fcb* fcb);

#endif //FILESYSTEM_GESTION_FILESYSTEM_H
