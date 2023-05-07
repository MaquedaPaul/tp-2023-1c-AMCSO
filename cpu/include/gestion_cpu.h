//
// Created by utnso on 5/5/23.
//

#ifndef CPU_GESTION_CPU_H
#define CPU_GESTION_CPU_H
#include <stdbool.h>
#include <protocolo.h>
#include <mmu.h>
#include <string.h>
extern uint32_t tam_max_segmento;
extern t_pcb *pcb_actual;
extern t_segmento* segmento;
extern int cliente_servidor;

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

extern uint32_t dir_logica_actual;
extern uint32_t dir_fisica_actual;

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

void ejecutar_CREATE_SEGMENT(int id_del_segmento, int tamanio) ;
void ejecutar_DELETE_SEGMENT(int id_del_segmento);

void ejecutar_YIELD();
void ejecutar_EXIT();

void cambiar_valor_registro(char* registro,char* valor);
int buscar_registro(char* registro);
void copiar_registros(registros_cpu* registros_PCB);
void obtener_valor_registro(char* registro,char valor[]);


char* leer_valor_de_memoria(int direccion_fisica);

#endif //CPU_GESTION_CPU_H
