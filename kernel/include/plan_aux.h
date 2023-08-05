#ifndef TPSO_LOGGERS_PLAN_AUX_H
#define TPSO_LOGGERS_PLAN_AUX_H

#include <planificacion.h>
#include <init.h>



extern int fd_filesystem;
extern int fd_cpu;
extern bool hayCompactacionPendiente;



void eliminarElementoLista(t_pcb* , t_list*);

t_pcb *crearPcb (t_list* listaInstrucciones);
registros_cpu *crearRegistroCPU ();



//FILESYSTEM
void ejecutar_FOPEN(t_pcb*);
void ejecutar_FCLOSE(t_pcb*);
void ejecutar_FSEEK(t_pcb* );
void ejecutar_FTRUNCATE( t_pcb* );
void ejecutar_FREAD(t_pcb*,uint32_t direccionFisica);
void ejecutar_FWRITE(t_pcb*, uint32_t direccionFisica);
int buscarArch_TablaGlobalArchivo(char* );
void agregarEntrada_TablaGlobalArchivosAbiertos(char* );
void desbloquearPcb_porNombreArchivo (char*);
char* obtenerNombreArchivo(t_pcb* pcb);
void actualizarDuenioTGAA(char* nombreArchivo, t_pcb* pcbNuevoDuenio);
void eliminarArchivoTablaLocal(char* nombreArchivo, t_pcb* pcb);
void actualizarTGAALuegoDeLiberacionDeArchivo(char* nombreArchivo);
void eliminarPcbTGAA_Y_actualizarTGAA(t_pcb* pcb);
void aumentarContadorPeticionesFs();
char* reciboYActualizoContadorPeticionesFs(int cliente_socket);




#endif