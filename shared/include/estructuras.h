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
#include <string.h>

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
    uint8_t cantidad_parametros;
    uint8_t param1Length;
    char* param1;
    uint8_t param2Length;
    char* param2;
    uint8_t param3Length;
    char* param3;
} t_instr;


//Registros de CPU
typedef struct{

    char registro_AX[5];
    char registro_BX[5];
    char registro_CX[5];
    char registro_DX[5];
    char registro_EAX[9];
    char registro_EBX[9];
    char registro_ECX[9];
    char registro_EDX[9];
    char registro_RAX[17];
    char registro_RCX[17];
    char registro_RBX[17];
    char registro_RDX[17];
}registros_cpu;
//Tabla de Segmentos

typedef struct
{
    uint32_t pid;
    t_list* segmentos;
} t_tablaSegmentos;

typedef struct
{
    uint32_t base;
    uint32_t limite;
    uint32_t id;
} t_segmento;

typedef struct
{
    uint32_t tamanio;
    void* datos;
} t_datos;



typedef struct {
    int fd;
    char *server_name;
} t_procesar_conexion_args;

typedef struct {
    t_queue* cola;
    char* nombreRecurso;
    int indiceSemaforo;
    int instanciasRecurso;
} t_recurso;




//Estructura del pcb
typedef struct{
	uint32_t id;
	uint32_t programCounter;
    uint32_t estimacionRafaga;
    uint32_t rafagaAnterior;
    uint32_t tiempoLlegadaReady;
    uint32_t tiempoEnvioExec;
    registros_cpu* registrosCpu;
    t_list* instr;
    t_list* tablaSegmentos;
    //t_list* tablaArchivosAbiertos; TODO AGREGAR A LA PCB CUANDO LO USEMOS
} t_pcb;


t_proceso *crearNuevoProceso();
void closure_mostrarListaInstrucciones(t_instr * element);
void liberarInstruccion(t_instr * instruccion);

bool esInstruccionSinParametros(t_instr * instruccion);
bool esInstruccionConUnParametro(t_instr * instruccion);
bool esInstruccionConDosParametros(t_instr * instruccion);
bool esInstruccionConTresParametros(t_instr * instruccion);

void mostrarIntArray(uint32_t *array, char*message, t_log* logger);

void simularRetardoSinMensaje(int retardo);
void simularRetardo(char* message,char* messageFinal, t_log* logger, int retardo);
t_list * convertirIntArrayALista(uint32_t * array);
uint32_t* convertirIntListaToArray(t_list* lista);

#endif //SHARED_ESTRUCTURAS_H
