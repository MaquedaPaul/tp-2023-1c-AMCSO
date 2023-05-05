#include <commons/collections/list.h>
#include <commons/log.h>

#include <estructuras.h>
#include <string.h>

#include <protocolo.h>
#include <mmu.h>
#include <init.h>


extern int fd_memoria;
extern int fd_kernel;



int traducir_direccion_logica_a_fisica(int logica);

instr_t* fetch();
char* decode();
void execute();

void escribir_en_registro(char* registro, char valor[16]);
char* leer_valor_de_registro(char* registro);
int tamaño_a_escribir(char* tipo_registro);
void ejecutar_lectura();
void ejecutar_escritura();
void terminar_ejecucion_lectura(char* leido);
void terminar_ejecucion_escritura();



void ejecutar_SET(char* registro, char* valor);
void ejecutar_MOV_IN(char* registro, int direccion_logica);
void ejecutar_MOV_OUT(char* nombre_archivo, int direccion_logica);
void ejecutar_IO(int tiempo);
void ejecutar_F_OPEN(char* nombre_archivo);
void ejecutar_F_CLOSE(char* nombre_archivo);
void ejecutar_F_SEEK(char* nombre_archivo, int posicion);

int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes);
bool error_segmentationFault(int numero_segmento, int desplazamiento_segmento, int cantidad_bytes);

void ejecutar_F_READ(char* nombre_archivo, int direccion_logica, int cantidad_bytes);
void ejecutar_F_WRITE(char* nombre_archivo, int direccion_logica, int cantidad_bytes);
void ejecutar_F_TRUNCATE(char* nombre_archivo, int tamanio);
void ejecutar_WAIT(char* nombre_recurso);
void ejecutar_SIGNAL(char* nombre_recurso);
void ejecutar_CREATE_SEGMENT(int id_del_segmento, int tamanio);
void ejecutar_DELETE_SEGMENT(int id_del_segmento);
void ejecutar_YIELD();
void ejecutar_EXIT();