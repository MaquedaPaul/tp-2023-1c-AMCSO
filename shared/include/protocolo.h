#ifndef SHARED_PROTOCOLO_H_
#define SHARED_PROTOCOLO_H_
#undef DEBUG
#define DEBUG 69
#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <estructuras.h>
#include <fcntl.h>
typedef enum {
    //-------------MENSAJES ENTRE CONSOLA-KERNEL------------------------------------
    GESTIONAR_CONSOLA_NUEVA = 0,
    //------------------------------------------------------------------------------
    //-----------MENSAJES CPU-KERNEL---------------------------------------------
    PROCESO_TERMINADO = 2,
    PROCESO_DESALOJADO,
    //---------------------------------------------------------------------------------
    //----------------MENSAJES DE KERNEL-CPU--------------------------------------------
    DESALOJAR_PROCESO,
    //----------------------------------------------------------------------------------
    //------------MENSAJES DE MEMORIA-KERNEL---------------------------
    OUT_OF_MEMORY,
    COMPACTACION_FINALIZADA,
    SEGMENTO_ELIMINADO,
    //-----------MENSAJES DE FILE SYSTEM-KERNEL--------------------------------------
    PUEDO_COMPACTAR,
    ESPERAR_PARA_COMPACTACION,
    /////////////////////////////////////CPU///////////////////////////////////////////
    PCB,
    IO_BLOCK,
    F_OPEN,
    F_CLOSE,
    F_SEEK,
    SEGMENTATION_FAULT,
    F_READ,
    F_WRITE,
    F_TRUNCATE,
    WAIT,
    SIGNAL,
    CREATE_SEGMENT,
    DELETE_SEGMENT,
    YIELD,
    EXIT,

    //////HANDSHAKES
    HANDSHAKE_FS,
    HANDSHAKE_CPU,
    /////////////////////////////////MEMORIA///////////////////////////////////////////
    INICIALIZAR_PROCESO_MEMORIA,
    FINALIZAR_PROCESO_MEMORIA,
    ACCESO_PEDIDO_LECTURA,
    ACCESO_PEDIDO_ESCRITURA,
    CREACION_SEGMENTOS,
    ELIMINACION_SEGMENTOS,
    COMPACTACION_SEGMENTOS,
    LECTURA_REALIZADA,
    ESCRITURA_REALIZADA,
    FINALIZACION_PROCESO_TERMINADA,
    SIN_ESPACIO_DISPONIBLE,
    SE_NECESITA_COMPACTACION,
    CREACION_SEGMENTO_EXITOSO,
    ESTRUCTURAS_INICALIZADAS,
    /////////////////////////////////FILESYSTEM///////////////////////////////////////
    APERTURA_ARCHIVO,
    CREACION_ARCHIVO,
    TRUNCACION_ARCHIVO,
    LECTURA_ARCHIVO,
    ESCRITURA_ARCHIVO,
    APERTURA_ARCHIVO_EXITOSA,
    APERTURA_ARCHIVO_FALLIDA,
    CREACION_ARCHIVO_EXITOSA,
    TRUNCACION_ARCHIVO_EXITOSA,
    LECTURA_ARCHIVO_EXITOSA,
    ESCRITURA_ARCHIVO_EXITOSA,
    //////////////////////////////////////
    ERROR,
    ERROR_INDICE_TP,

} op_code;




typedef struct
{
    int size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
} t_paquete;

//GENERICAS
void enviarOrden(op_code orden, int socket, t_log *logger);
void recibirOrden(int socket);
void enviarValor_uint32(uint32_t valor, int socket, op_code orden, t_log *logger);
uint32_t recibirValor_uint32(int socket, t_log *logger);

void crear_buffer(t_paquete* paquete);
void* recibir_stream(int *size, uint32_t cliente_socket);

int recibir_operacion(int socket_cliente);

t_paquete* crear_paquete(op_code codigo, t_log* logger);
void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
t_proceso* recibir_paquete(int socket_cliente);
void eliminar_paquete(t_paquete* paquete, t_log* logger);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);


bool enviarListaUint32_t(t_list* listaInts, int socket_cliente, t_log* logger, op_code codigo);
bool agregarUint32_tsAPaquete(t_list* listaInts, t_paquete* paquete);
t_list* recibirListaUint32_t(int socket_cliente);


bool enviarDatos(void* datos, uint32_t tamanioDatos, op_code codigo, int socket_cliente, t_log* logger);
bool agregarDatosAPaquete(void* datos, uint32_t tamanioDatos, t_paquete* paquete);
void* recibirDatos(int socket_cliente, uint32_t tamanioDatos);



bool enviarEnteroYString(uint32_t entero,char* string, uint32_t tamanioString, int socket_cliente, t_log* logger, op_code codigo);
bool agregarEnteroYStringAPaquete(uint32_t entero, char* string, uint32_t tamanioString, t_paquete* paquete);
char* recibirEnteroYString(int socket_cliente,uint32_t* entero);

bool enviarString(char* string, int socket_cliente, op_code codigoOperacion, t_log* logger);
bool agregarStringAPaquete(char* string, t_paquete* paquete);
char* recibirString(int socket_cliente);

uint32_t enviar_int_array(uint32_t *array, int conexion, op_code codigo, t_log* logger);
void agregarIntArrayAPaquete(t_paquete* paquete, uint32_t *array);
uint32_t * recibir_int_array(int conexion);



void enviar_mensaje(char* mensaje, int socket_cliente, op_code codigoOperacion, t_log* logger);
char* recibir_mensaje(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);

bool enviarParamsParaLecturaEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid, op_code codigoOperacion, t_log* logger, int socket_cliente);
bool agregarParamsParaLecturaEscrituraArchivoAPaquete(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid, t_paquete* paquete);
void recibirParamsParaLecturaEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid, int socket_cliente);


bool enviarListaInstrucciones(t_list* listaInstrucciones, int socket_cliente, t_log* logger);
bool agregarInstruccionesAPaquete(t_list* listaInstrucciones, t_paquete* paquete);
t_instr * sumarTamaniosInstrucciones(t_instr* unaInstruccion,t_instr *otraInstruccion);
int calcularTamanioDeInstruccion(t_instr* unaInstruccion);
t_list* recibirListaInstrucciones(int socket_cliente);


bool enviarTablasSegmentos(t_list* tablasSegmentos, int socket_cliente, t_log* logger,op_code codigo);
bool agregarTablasAPaquete(t_list* tablasSegmentos, t_paquete* paquete);
t_list* recibirTablasSegmentosInstrucciones(int socket_cliente);



bool enviarListaIntsYDatos(t_list* listaInts,t_datos* datos, int socket_cliente, t_log* logger, op_code codigo);
bool agregarIntsYDatosAPaquete(t_list* listaInts, t_datos* datos, t_paquete* paquete);
t_list* recibirListaIntsYDatos(int cliente_socket,t_datos* datos);


void agregar_tablaSegmentos_a_paquete(t_paquete* paquete, t_tablaSegmentos* tablaSegmentos);
void agregar_instrucciones_a_paquete(t_paquete *paquete, t_list *instrucciones);
void agregar_segmentos_a_paquete(t_paquete *paquete, t_list *segmentos);
void agregar_registros_a_paquete(t_paquete *paquete, registros_cpu *registro);
void agregar_PCB_a_paquete(t_paquete *paquete, t_pcb* pcb);

//Necesarias para cpu-kernel
void enviar_paquete_pcb(t_pcb* pcbDelProceso, int conexion, op_code codigo, t_log* logger);
void enviar_paquete_pcb2(t_pcb* pcb_actual,int fd_kernel,op_code YIELD,t_log* info_logger);
t_pcb* recibir_pcb(int conexion);
t_pcb*  recibir_pcb_direccion(int conexion,uint32_t* parametroDireccion);

//////////////////////////////////////////////////////////


t_pcb * recibir_pcb2 ( int );
t_pcb * recibir_paquete_con_PCB(uint32_t*, char* ) ;
char* sacar_cadena_de_paquete( uint32_t*desplazamiento , char* buffer);
uint32_t  sacar_uint32_t_de_paquete( uint32_t*desplazamiento, char* buffer);
uint8_t  sacar_uint8_t_de_paquete( uint32_t*desplazamiento, char* buffer) ;
void agregar_registroPCB(char*,t_paquete* );


void enviar_archivoTruncacion(t_archivoTruncate* archivoTruncate, int conexion, op_code codigo, t_log* logger);
void agregar_archivoTruncacion_a_paquete(t_paquete* paquete, t_archivoTruncate* archivoTruncate);
t_archivoTruncate* recibir_archivoTruncacion(int conexion);

void enviar_archivoRW(t_archivoRW* archivoRW, int conexion, op_code codigo, t_log* logger);
void agregar_archivoRW_a_paquete(t_paquete* paquete, t_archivoRW* archivoRw);
t_archivoRW* recibir_archivoRW(int conexion);

#endif