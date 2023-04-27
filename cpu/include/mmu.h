#include <stdlib.h>

#include <math.h>
#include <string.h>

#include "loggers_configs.h"
#include <estructuras.h>

#include <stdint.h>


extern t_config_cpu *cfg_cpu;


int obtener_direccion_logica();
int obtener_direccion_fisica(int direccion_logica);
int es_segmetation_fault(int direccion_fisica, int direccion_logica, int tamaño_a_leer_o_escribir);
