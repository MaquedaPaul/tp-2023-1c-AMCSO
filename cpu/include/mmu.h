#include <stdlib.h>

#include <math.h>
#include <string.h>

#include <loggers_configs.h>
#include <estructuras.h>
#include <protocolo.h>
#include <stdint.h>
//#include <gestion_cpu.h> //lo agregue pq copiar_registros() lo defini en gestion_cpu

extern t_pcb* pcb_actual;
extern t_log* info_logger;
extern bool cicloInstruccionesDebeEjecutarse; //nose si va aca





extern t_config_cpu *cfg_cpu;
int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes);
bool error_segmentationFault(int desplazamiento_segmento, int cantidad_bytes);
void copiar_registrosCPU_a_los_registroPCB(registros_cpu* registro);