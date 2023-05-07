#include <commons/collections/list.h>
#include <commons/log.h>

#include <estructuras.h>
#include <string.h>

#include <protocolo.h>
#include <mmu.h>
#include <init.h>
#include <gestion_cpu.h>

extern int fd_memoria;
extern int fd_kernel;

//Instruccion a ejecutar actual
extern instr_t* instruccion;

extern bool cicloInstruccionesDebeEjecutarse;
extern char* nombre_instruccion_actual;
extern t_pcb *pcb_actual;
extern uint32_t tam_max_segmento;
extern char registroCPU_AX[4];
extern char registroCPU_BX[4];
extern char registroCPU_CX[4];
extern char registroCPU_DX[4];
extern char registroCPU_EAX[8];
extern char registroCPU_EBX[8];
extern char registroCPU_ECX[8];
extern char registroCPU_EDX[8];
extern char registroCPU_RAX[16];
extern char registroCPU_RBX[16];
extern char registroCPU_RCX[16];
extern char registroCPU_RDX[16];

extern int cliente_servidor;

extern t_segmento* segmento;

//Registros temporales
extern uint32_t dir_logica_actual;
extern uint32_t dir_fisica_actual;



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


int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes);
bool error_segmentationFault(int numero_segmento, int desplazamiento_segmento, int cantidad_bytes);

/*
void ejecutar_SET(char* registro, char* valor);
void ejecutar_MOV_IN(char* registro, int direccion_logica);
void ejecutar_MOV_OUT(int direccion_logica, char* registro);
void ejecutar_IO(int tiempo);
void ejecutar_F_OPEN(char* nombre_archivo);
void ejecutar_F_CLOSE(char* nombre_archivo);
void ejecutar_F_SEEK(char* nombre_archivo, int posicion);
void ejecutar_F_READ(char* nombre_archivo, int direccion_logica, int cantidad_bytes);
void ejecutar_F_WRITE(char* nombre_archivo, int direccion_logica, int cantidad_bytes);
void ejecutar_F_TRUNCATE(char* nombre_archivo, int tamanio);
void ejecutar_WAIT(char* nombre_recurso);
void ejecutar_SIGNAL(char* nombre_recurso);
void ejecutar_CREATE_SEGMENT(int id_del_segmento, int tamanio);
void ejecutar_DELETE_SEGMENT(int id_del_segmento);
void ejecutar_YIELD();
void ejecutar_EXIT();
 */