#include <commons/collections/list.h>
#include <commons/log.h>

#include <estructuras.h>
#include <string.h>

#include <protocolo.h>
#include <mmu.h>
#include <init.h>

extern pcb* pcb_actual;
extern int fd_memoria;
extern int fd_kernel;

void escribir_en_registro(char* registro, char valor[16]);
char* leer_valor_de_registro(char* registro);
int tamaño_a_escribir(char* tipo_registro);
void ejecutar_lectura();
void ejecutar_escritura();
void terminar_ejecucion_lectura(char* leido);
void terminar_ejecucion_escritura();



