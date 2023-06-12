//
// Created by utnso on 5/5/23.
//

#ifndef CPU_GESTION_CPU_H
#define CPU_GESTION_CPU_H
#include <stdbool.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <mmu.h>
#include <string.h>
#include <estructuras.h>

extern t_pcb* pcb_actual;
extern char* nombre_instruccion_actual;
extern t_segmento* segmento;

//extern int cliente_servidor;
extern int fd_memoria;
extern int fd_kernel;

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
extern int num_segmento;

char* ip_cpu;
char* puerto_cpu;
int fd_memoria;
int fd_kernel;
int fd_cpu;

//Instruccion a ejecutar actual

bool cicloInstruccionesDebeEjecutarse = true;

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

void cambiar_valor_del_registroCPU(char* registro, char* valor);
int calcular_bytes_segun_registro(char* registro);
void copiar_registrosCPU_a_los_registroPCB(registros_cpu* registro);
void obtener_valor_registroCPU(char* registro,char valor[]);

char* leer_valor_de_memoria(int direccion_fisica, int cantidad_bytes);
void escribir_valor_en_memoria(int , int, char*);
char* recibir_valor_de_memoria();
void esperar_orden();
#endif
