#include <stdlib.h>

#include <math.h>
#include <string.h>

#include <loggers_configs.h>
#include <estructuras.h>
#include <protocolo.h>
#include <stdint.h>
#include <gestion_cpu.h> //lo agregue pq copiar_registros() lo defini en gestion_cpu

extern t_pcb* pcb_actual;
extern t_log* info_logger;
extern bool cicloInstruccionesDebeEjecutarse; //nose si va aca


t_segmento* segmento;
int cliente_servidor;
int num_segmento;


char registroCPU_AX[4];
char registroCPU_BX[4];
char registroCPU_CX[4];
char registroCPU_DX[4];
char registroCPU_EAX[8];
char registroCPU_EBX[8];
char registroCPU_ECX[8];
char registroCPU_EDX[8];
char registroCPU_RAX[16];
char registroCPU_RBX[16];
char registroCPU_RCX[16];
char registroCPU_RDX[16];


extern t_config_cpu *cfg_cpu;
int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes );
bool error_segmentationFault(int desplazamiento_segmento, int cantidad_bytes);
