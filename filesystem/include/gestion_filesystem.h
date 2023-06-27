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
void* realizarLecturaArchivo(char* nombreArchivo, uint32_t puntero, uint32_t  tamanio);
void realizarEscrituraArchivo(void* datos, uint32_t tamanioDatos);

uint32_t numero_bloque_a_escribir(uint32_t tamanioArchivo, uint32_t puntero);
int buscar_posicion_dentro_del_bloque(uint32_t tamanioArchivo, uint32_t puntero);
void* leer_archivo(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb);
uint32_t cantidadDisponibleParaLeer(uint32_t puntero);

uint32_t cantidadFaltanteDelDato(uint32_t nuevoTamanioALeer);
uint32_t buscarPunteroInicioDelBloque(int numeroBloque, t_config_fcb* fcb);
uint32_t cantidadOcupadaDentroDelBloque(uint32_t cantidadDisponibles);

#endif //FILESYSTEM_GESTION_FILESYSTEM_H
