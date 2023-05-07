#include <stdlib.h>

#include <math.h>
#include <string.h>

#include <loggers_configs.h>
#include <estructuras.h>
#include <protocolo.h>
#include <stdint.h>
#include <gestion_cpu.h> //lo agregue pq copiar_registros() lo defini en gestion_cpu

extern t_log* info_logger;
extern bool cicloInstruccionesDebeEjecutarse; //nose si va aca

extern t_config_cpu *cfg_cpu;
int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes );
int obtener_direccion_logica();
int obtener_direccion_fisica(int direccion_logica);
int es_segmetation_fault(int direccion_fisica, int direccion_logica, int tamaño_a_leer_o_escribir);
bool error_segmentationFault(int desplazamiento_segmento, int cantidad_bytes);
