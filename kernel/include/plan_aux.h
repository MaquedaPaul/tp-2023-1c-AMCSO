#ifndef TPSO_LOGGERS_PLAN_AUX_H
#define TPSO_LOGGERS_PLAN_AUX_H

#include <planificacion.h>
#include <init.h>
#include <comunicacion.h>


extern int fd_filesystem;
extern int fd_cpu;


void eliminarElementoLista(t_pcb* , t_list*);

t_pcb *crearPcb (t_list* listaInstrucciones);
registros_cpu *crearRegistroCPU ();
void actualizarTiempoRafaga(t_pcb*);


//FILESYSTEM
void ejecutar_FOPEN(t_pcb* ,char* );
void ejecutar_FOPEN_socket(int);
void ejecutar_FCLOSE(int);
void ejecutar_FSEEK(int );
void ejecutar_FTRUNCATE( int );
void ejecutar_FREAD(int);
void ejecutar_FWRITE(int);
int buscarArch_TablaGlobalArchivo(char* );
t_pcb* buscarPcb_enTablaPeticionesFS(char* );
t_TablaArchivos* buscarEntrada_TablaGlobalArchivo(char* );
void agregarEntrada_TablaGlobalArchivosAbiertos(char* );

#endif