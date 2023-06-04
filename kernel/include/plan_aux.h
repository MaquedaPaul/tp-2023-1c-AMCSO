#ifndef TPSO_LOGGERS_PLAN_AUX_H
#define TPSO_LOGGERS_PLAN_AUX_H

#include <planificacion.h>

void eliminarElementoLista(t_pcb* , t_list*);

t_pcb *crearPcb (t_list* listaInstrucciones);
registros_cpu *crearRegistroCPU ();
void actualizarTiempoRafaga(t_pcb*);

#endif