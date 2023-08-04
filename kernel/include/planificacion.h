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
void planificador_corto_plazo();
void liberar_procesos();

//Funciones cambio de estado
void agregarProceso_New(t_pcb*);
void moverProceso_NewReady(t_tablaSegmentos* tablaDeSegmentosMemoria);
void moverProceso_BloqrecursoReady(t_recurso* recurso);
void moverProceso_ExecReady(t_pcb*);
void moverProceso_readyExec(); 
void moverProceso_ExecBloq(t_pcb*);
void moverProceso_BloqReady(t_pcb*);
void moverProceso_ExecExit(t_pcb*);
void bloquearProcesoPorRecurso(t_recurso* recurso);
void liberarRecursosTomados(t_pcb* pcb);

void aumentarGradoMP();
void decrementarGradoMP();

//Funciones de HRNN
double response_ratio(t_pcb* pcb);
void calcular_estimacion(t_pcb* pcb);
bool criterio_hrrn(t_pcb* pcb1, t_pcb* pcb2);

/*
-Se setean los campos HRRN en 0 al crear pcb
-FALTA: Se setea tiempoLlegadaReady al recibir rta de creacion de estructura desde memoria
-Se setea tiempoLlegadaReady al volver a lista ready desde bloqueado
-Se actualiza tiempoEnvioExec antes de enviar a EXEC (en planificaion)
-Se actualiza rafagaAnterior cuando vuelve pcb desde CPU (en comunicacion)
-Se actualiza estimacionRafaga al ser seleccionado por seleccionar_segunHRRN (en planificacion)
*/