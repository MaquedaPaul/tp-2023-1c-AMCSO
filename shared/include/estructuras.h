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

/*                            // use este formato de registros_cpu 
typedef struct{
    char* registro_AX;
    char* registro_BX;
    char* registro_CX;
    char* registro_DX;
    char* registro_EAX;
    char* registro_EBX;
    char* registro_ECX;
    char* registro_EDX;
    char* registro_RAX;
    char* registro_RCX;
    char* registro_RBX;
    char* registro_RDX;
}registros_cpu;
*/

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
    t_list* cola; //La usamos como una cola.Por el unico motivo que es una lista es por el tema de actualizacion de tablas de segmentos.
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
    uint32_t fd_consola;
    registros_cpu* registrosCpu;
    t_list* instr;
    t_tablaSegmentos* tablaSegmentos;
    t_list* tablaArchivosAbiertos;//Seria una t_list de archivoLocal. No la serializamos
    t_list* recursosTomados; //seria una t_list de t_recurso. No lo serializamos
} t_pcb;


typedef struct {
    char* nombreArchivo;
    pthread_mutex_t mutex; // Mutex para proteger el acceso concurrente al archivo
} t_archivo;

typedef struct {
    t_archivo* archivo;
    uint32_t ptro; //La pos del puntero es local de cada Proceso, no del archivo
}t_archivoLocal;

typedef struct {
    t_archivo* archivo;
    t_pcb* pcb;
} t_archivoPeticion;

typedef struct {
    //length nombreArchivo,nombreArchivo,tamNew
    uint32_t nombreArchivoLength;
    char* nombreArchivo;
    uint32_t nuevoTamanio;
}t_archivoTruncate;

typedef struct {
    uint32_t nombreArchivoLength;
    char* nombreArchivo;
    uint32_t posPuntero;
    uint32_t direcFisica;
    uint32_t cantidadBytes;
    uint32_t pid;
}t_archivoRW; //Se usa para F_READ Y F_WRITE. Estructura aux para mandarselo a FS la info

t_proceso *crearNuevoProceso();
void closure_mostrarListaInstrucciones(t_instr * element);
void liberarInstruccion(t_instr * instruccion);
void liberarSegmento(t_segmento* segmento);
void liberarPcb(t_pcb* pcb);
void liberarPcbCpu(t_pcb* pcb);

bool esInstruccionSinParametros(t_instr * instruccion);
bool esInstruccionConUnParametro(t_instr * instruccion);
bool esInstruccionConDosParametros(t_instr * instruccion);
bool esInstruccionConTresParametros(t_instr * instruccion);
void establecerCantidadParametrosInstrucciones(t_list* listaInstrucciones);
void mostrarIntArray(uint32_t *array, char*message, t_log* logger);

void simularRetardoSinMensaje(int retardo);
void simularRetardo(char* message,char* messageFinal, t_log* logger, int retardo);
t_list * convertirIntArrayALista(uint32_t * array);
uint32_t* convertirIntListaToArray(t_list* lista);

#endif //SHARED_ESTRUCTURAS_H
