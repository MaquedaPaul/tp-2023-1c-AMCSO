//
// Created by utnso on 10/6/22.
//

#ifndef SHARED_ESTRUCTURAS_H
#define SHARED_ESTRUCTURAS_H
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <semaphore.h>
#include <malloc.h>
#include <config_utils.h>
#include <stdint.h>
#include <unistd.h>

typedef struct
{
    uint32_t PID; // 4 bytes
    uint8_t cantidad_instrucciones;
    t_list* instrucciones; //12 bytes
    uint8_t cantidad_segmentos;
    t_list* tam_segmentos;
}t_proceso; //Puede ser que haya que agregarle mas parametros como el segmento

typedef struct {
    uint8_t idLength;
    char* id; // el id seria el nombre de la instruccion
	uint8_t param1Length;
    char* param1;
	uint8_t param2Length;
    char* param2;
    uint8_t param3Length;
    char* param3;
} instr_t;

//Registros de CPU
typedef struct{
    char[4] AX;
    char[4] BX;
    char[4] CX;
    char[4] DX;
    char[8] EAX;
    char[8] EBX;
    char[8] ECX;
    char[8] EDX;
    char[16] RAX;
    char[16] RBX;
    char[16] RCX;
    char[16] RDX;
}registros_cpu;

//Tabla de Segmentos


typedef struct{
    uint16_t tamanioSegmento;
    uint32_t indiceTablaPaginas;
}segmento;


typedef struct {
    int fd;
    char *server_name;
} t_procesar_conexion_args;

typedef struct {
    t_queue cola;
    char* nombre;
    int indiceSemaforo;
    int tiempoRetardo;
} t_atenderColaDispositivo_args;




//Estructura del pcb
typedef struct{
	uint32_t id;
	t_list* instr;
    registros_cpu* registrosCpu;
	uint32_t programCounter;
    t_list* tablaSegmentos;
} pcb;

typedef struct{ //Usada cuando hay PF
    uint32_t  segmento;
    uint32_t pagina;
    pcb* pcb;
}pcb_page_fault;

typedef struct {
    char* nombreDispositivo;
    int tiempoRetardo;
    t_queue* cola; //Aca guardo las PCB de los procesos
    int indiceSemaforo;
}cola_dispositivo;


// ESTOS SON LOS TIPOS QUE VAN EN LA PCB {SANTI}

t_proceso *crearNuevoProceso();
void closure_mostrarListaInstrucciones(instr_t* element);


bool esInstruccionSinParametros(instr_t* instruccion);
bool esInstruccionConUnParametro(instr_t* instruccion);
bool esInstruccionConDosParametros(instr_t* instruccion);
bool esInstruccionConTresParametros(instr_t* instruccion);

void mostrarPcb(pcb* pcbProceso);
void mostrarIntArray(uint32_t *array, char*message, t_log* logger);
int retornarEnSegundos(int milisegundos);
void simularRetardo(char* message,char* messageFinal, t_log* logger, int retardo);
void mostrarSegmentos(pcb* pcbProceso);
void liberarPcb(pcb* pcbALiberar);
t_list * convertirIntArrayALista(uint32_t * array);
uint32_t* convertirIntListaToArray(t_list* lista);

#endif //SHARED_ESTRUCTURAS_H
