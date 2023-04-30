//
// Created by utnso on 4/29/23.
//

#ifndef FILESYSTEM_GESTION_FILESYSTEM_H
#define FILESYSTEM_GESTION_FILESYSTEM_H
#include <stdbool.h>
#include <stdint.h>



bool existeFcbConNombre(char* nombreArchivo);
void realizarCreacionArchivo(char* nombreArchivo);
void realizarTruncacionArchivo(char* nombreArchivo);
void* realizarLecturaArchivo(char* nombreArchivo, uint32_t puntero, uint32_t  tamanio);
void realizarEscrituraArchivo(void* datos, uint32_t tamanioDatos);


#endif //FILESYSTEM_GESTION_FILESYSTEM_H
