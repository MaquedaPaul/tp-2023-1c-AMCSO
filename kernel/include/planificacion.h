#ifndef TEMPLATE_PLANIFICACION_H
#define TEMPLATE_PLANIFICACION_H

#endif //TEMPLATE_PLANIFICACION_H

#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <comunicacion.h>
#include <init.h>


extern int fd_cpu;
extern int fd_memoria;


//Funciones cambio de estado
void agregarProceso_New(pcb*);
void moverProceso_NewReady();
void moverProceso_readyExec(); 
void moverProceso_ExecBloq(pcb*);
void moverProceso_ExecReady(pcb*);
void moverProceso_BloqReady(pcb*);
void moverProceso_ExecExit(pcb*);