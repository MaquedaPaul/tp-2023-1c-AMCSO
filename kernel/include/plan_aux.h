#ifndef TPSO_LOGGERS_PLAN_AUX_H
#define TPSO_LOGGERS_PLAN_AUX_H

#include <planificacion.h>

void eliminarElementoLista(pcb, t_list*);

pcb *crearPcb (int);
registros_cpu *crearRegistroCPU ();
void actualizarTiempoRafaga(pcb*);

#endif