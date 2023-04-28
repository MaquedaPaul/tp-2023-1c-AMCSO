#include <commons/collections/list.h>
#include <estructuras.h>
#include <string.h>

#include <protocolo.h>
#include <mmu.h>


void escribir_en_registro(char* registro, char valor[16]);

void escribir_en_registro_de_4_bytes(char* registro, char valor[4]);

void escribir_en_registro_de_8_bytes(char* registro, char valor[8]);

void escribir_en_registro_de_16_bytes(char* registro, char valor[16]);

int tamaño_a_escribir(char* tipo_registro);

int obtener_valor_en_memoria();


