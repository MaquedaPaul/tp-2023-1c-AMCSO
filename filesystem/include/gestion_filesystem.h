//
// Created by utnso on 4/29/23.
//

#ifndef FILESYSTEM_GESTION_FILESYSTEM_H
#define FILESYSTEM_GESTION_FILESYSTEM_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <loggers_configs.h>
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
    t_config* fcb_config;
} t_config_fcb;

typedef struct
{
    void* archivo;
    int fd;
    int tamanio;
}t_bloques;

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config_filesystem *cfg_filesystem;
extern t_list* lista_FCBs;
extern t_bloques* archivoBloques;
extern t_bitarray* bitmap;
extern t_config_superbloque* cfg_superbloque;




int existe_archivoFCB(char *nombre_archivo);
t_config_fcb* buscarFCBporNombre(char* nombre);
void realizarCreacionArchivo(char* nombreArchivo);
void realizarTruncacionArchivo(char* nombreArchivo, uint32_t nuevo_tamanio_del_archivo);
void ampliar_o_reducir_tamanio(t_config_fcb *aux_FCB, uint32_t nuevo_tamanio, uint32_t tamanio_archivo);
uint32_t obtener_bloque_libre(t_bitarray* auxBitArray);
void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t  tamanio);
void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos);

int numeroDeBloque(uint32_t punteroArchivo);
int buscarPosicionDentroDelBloque(uint32_t puntero, uint32_t numeroBloque);
void* leer_archivo(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb);
void escribirBloque(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* pcb);

uint32_t cantidadDisponibleDelBloque(uint32_t puntero);
uint32_t cantidadBytesQueNoSePuedeLeerEnUnBloque(uint32_t nuevoTamanioALeer);
uint32_t buscarNumeroDeBloqueDelArchivoDeBloque(int numero_bloque, t_config_fcb* fcb);




#endif //FILESYSTEM_GESTION_FILESYSTEM_H
