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
#include <utils/estructuras.h>

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
    HANDSHAKE_CPU,
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
    PROCESO_INICIADO,
    PROCESO_BLOQUEADO,
    PAGINAS_INICIALIZADAS,
    PAGINAS_LIBERADAS,
    PEDIDO_LECTURA,
    PEDIDO_ESCRITURA,
    LECTURA_REALIZADA,
    ESCRITURA_REALIZADA,
    SOLICITUD_MARCO,
    ACCESO_RESULTO_PAGE_FAULT,
    ENVIO_MARCO_CORRESPONDIENTE,
    PAGE_FAULT,
    PROCESO_CARGADO_EN_MP,
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

t_paquete* crear_paquete(op_code codigo);
void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
t_proceso* recibir_paquete(int socket_cliente);
void eliminar_paquete(t_paquete* paquete, t_log* logger);

//Necesarias para cpu-kernel
void enviar_paquete_pcb(pcb* pcbDelProceso, int conexion, op_code codigo, t_log* logger);
void agregarPcbAPaquete(t_paquete* paquete, pcb* pcb);
pcb* recibir_pcb(int conexion);

void enviar_paquete_pcbPf(pcb_page_fault* pcbPfDelProceso, int conexion, op_code codigo, t_log* logger);
void agregarPcbPfAPaquete(t_paquete* paquete, pcb_page_fault* pcbDelProceso);
pcb_page_fault* recibir_pcbPf(int conexion);

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