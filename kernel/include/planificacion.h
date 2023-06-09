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

void planificador_largo_plazo();

//Funciones cambio de estado
void agregarProceso_New(t_pcb*);
void moverProceso_NewReady(t_list* tablaDeSegmentosMemoria);
void moverProceso_NewPreReady(); //Saca de la lista NEW, la envia a Memoria y la deja en una lista intermedia
void moverProceso_MemoriaReady(); //Saca el pcb de la lista intermedia, y la pasa a la cola ready
void moverProceso_readyExec(); 
void moverProceso_ExecBloq(t_pcb*);
void moverProceso_BloqReady(t_pcb*);
void moverProceso_ExecExit(t_pcb*);
void moverProceso_ExecReady(t_pcb*);

void aumentarGradoMP();
void decrementarGradoMP();

int seleccionar_segunHRRN();
/*
-Se setean los campos HRRN en 0 al crear pcb
-FALTA: Se setea tiempoLlegadaReady al recibir rta de creacion de estructura desde memoria
-Se setea tiempoLlegadaReady al volver a lista ready desde bloqueado
-Se actualiza tiempoEnvioExec antes de enviar a EXEC (en planificaion)
-Se actualiza rafagaAnterior cuando vuelve pcb desde CPU (en comunicacion)
-Se actualiza estimacionRafaga al ser seleccionado por seleccionar_segunHRRN (en planificacion)
*/