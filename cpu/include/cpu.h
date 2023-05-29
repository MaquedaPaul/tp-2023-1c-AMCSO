#include <commons/collections/list.h>
#include <commons/log.h>

#include <estructuras.h>
#include <string.h>

#include <protocolo.h>
#include <mmu.h>
#include <init.h>
#include <gestion_cpu.h>

//extern t_segmento* segmento; nose si va aca

extern int fd_memoria;
extern int fd_kernel;

//definido en comunicacion.h
extern t_instr* instruccion;
extern t_pcb* pcb_actual;

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


void iniciar_registros (registros_cpu* registro);

void ciclo_instrucciones();
t_instr* fetch();
char* decode();
void execute();