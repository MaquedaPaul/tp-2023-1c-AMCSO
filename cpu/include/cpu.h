#include <commons/collections/list.h>
#include <estructuras.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void escribir_en_registro_de_4_bytes(char* registro, uint32_t valor);

void escribir_en_registro_de_8_bytes(char* registro, uint64_t valor);

void escribir_en_registro_de_16_bytes(char* registro, char valor[16]);
int obtener_direccion_logica();