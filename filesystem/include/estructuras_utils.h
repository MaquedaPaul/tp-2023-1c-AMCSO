//
// Created by utnso on 5/4/23.
//

#ifndef FILESYSTEM_ESTRUCTURAS_UTILS_H
#define FILESYSTEM_ESTRUCTURAS_UTILS_H
#include <stdint.h>
#include <commons/config.h>
#include <stdlib.h>

uint32_t obtenerBlockSize(char* superbloque);
uint32_t obtenerBlockCount(char* superbloque);
char* obtenerNombreFcb(char* unFcb);
uint32_t obtenerTamanioArchvio(char* unFcb);
uint32_t obtenerPunteroDirecto(char* unFcb);
uint32_t obtenerPunteroIndirecto(char* unFcb);
char* obtenerKey(char* path, char* key);

#endif //FILESYSTEM_ESTRUCTURAS_UTILS_H
