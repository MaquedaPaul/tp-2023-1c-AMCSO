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

typedef enum
{
    //-------------MENSAJES ENTRE CONSOLA-KERNEL------------------------------------
	GESTIONAR_CONSOLA_NUEVA = 0,
    SOLICITAR_VALOR,
    IMPRIMIR_VALOR,
    //------------------------------------------------------------------------------
    //-------------MENSAJES SERIALIZACION GLOBALES---------------------------------
    PAQUETE = 3,
    MENSAJE,
    PCB = 30,
    //-----------------------------------------------------------------------------
    //-----------MENSAJES CPU-KERNEL---------------------------------------------
    PROCESO_TERMINADO,
    BLOCKED_IO,
    PROCESO_DESALOJADO,
    PROCESO_DESALOJADO_CON_PAGEFAULT,
    BLOCKED_PF,
    BLOCKED_TECLADO,
    BLOCKED_PANTALLA,
    CPU_VACIA,
    ERROR_SIGSEGV,
    INTERRUPCION,
    //---------------------------------------------------------------------------------
    //----------------MENSAJES DE KERNEL-CPU--------------------------------------------
    DESALOJAR_PROCESO,
    //----------------------------------------------------------------------------------
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

bool enviarListaUint32_t(t_list* listaInts, int socket_cliente, t_log* logger, op_code codigo);
bool agregarUint32_tsAPaquete(t_list* listaInts, t_paquete* paquete);
t_list* recibirListaUint32_t(int socket_cliente);


bool enviarDatos(void* datos, uint32_t tamanioDatos, op_code codigo, int socket_cliente, t_log* logger);
bool agregarDatosAPaquete(void* datos, uint32_t tamanioDatos, t_paquete* paquete);
void* recibirDatos(int socket_cliente, uint32_t tamanioDatos);


bool enviarString(char* string, int cliente_socket, op_code codigoOperacion);

bool agregarStringAPaquete();
char* recibirString(int cliente_socket);

void recibirParamsParaLecturaArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid);
void recibirParamsParaEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t pid,  uint32_t direccionFisica);



//Necesarias para cpu-kernel
/*
void enviar_paquete_pcb(pcb* pcbDelProceso, int conexion, op_code codigo, t_log* logger);
 */
/*
void agregarPcbAPaquete(t_paquete* paquete, pcb* pcb);
pcb* recibir_pcb(int conexion);

void enviar_paquete_pcbPf(pcb_page_fault* pcbPfDelProceso, int conexion, op_code codigo, t_log* logger);
void agregarPcbPfAPaquete(t_paquete* paquete, pcb_page_fault* pcbDelProceso);
pcb_page_fault* recibir_pcbPf(int conexion);
*/
//int array
uint32_t enviar_int_array(uint32_t *array, int conexion, op_code codigo, t_log* logger);
void agregarIntArrayAPaquete(t_paquete* paquete, uint32_t *array);
uint32_t * recibir_int_array(int conexion);

void enviar_pantalla_teclado(char* registro,int conexion, op_code codigo, t_log* logger);
void agregarRegistroAPaquete(t_paquete* paquete, char* registro);
char* recibir_pantalla_teclado(int conexion);

void enviar_mensaje(char* mensaje, int socket_cliente, op_code codigoOperacion, t_log* logger);
char* recibir_mensaje(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);




bool enviarListaInstrucciones(t_list* listaInstrucciones, int socket_cliente, t_log* logger);
bool agregarInstruccionesAPaquete(t_list* listaInstrucciones, t_paquete* paquete);
instr_t* sumarTamaniosInstrucciones(instr_t *unaInstruccion,instr_t *otraInstruccion);
int calcularTamanioDeInstruccion(instr_t *unaInstruccion);
t_list* recibirListaInstrucciones(int socket_cliente);


bool enviarTablasSegmentos(t_list* tablasSegmentos, int socket_cliente, t_log* logger);
bool agregarTablasAPaquete(t_list* tablasSegmentos, t_paquete* paquete);
t_list* recibirTablasSegmentosInstrucciones(int socket_cliente);



bool enviarListaIntsYDatos(t_list* listaInts,t_datos* datos, int socket_cliente, t_log* logger, op_code codigo);
bool agregarIntsYDatosAPaquete(t_list* listaInts, t_datos* datos, t_paquete* paquete);
t_list* recibirListaIntsYDatos(int cliente_socket,t_datos* datos);






/*

void *recibir_stream(int *size, int socket_cliente);
void recibir_mensaje(t_log *logger, int socket_cliente);
t_list *recibir_paquete(int socket_cliente);


bool send_aprobar_operativos(int fd, uint8_t nota1, uint8_t nota2);
bool recv_aprobar_operativos(int fd, uint8_t *nota1, uint8_t *nota2);

bool send_mirar_netflix(int fd, char *peli, uint8_t cant_pochoclos);
bool recv_mirar_netflix(int fd, char **peli, uint8_t *cant_pochoclos);

bool send_debug(int fd);
*/
#endif