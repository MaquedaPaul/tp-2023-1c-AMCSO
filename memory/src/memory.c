//
// Created by utnso on 4/17/23.
//

#include "memory.h"
void* espacio_contiguo;


void inicializarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket,info_logger);
    int tablaSegmentosInicial = inicializarProcesoConPid(pid);
    //enviarTablaSegmentos(cliente_socket);
}

int inicializarProcesoConPid(uint32_t pid){

    return 0;
}

void finalizarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket, info_logger);
    finalizarProcesoConPid(pid);
    enviarOrden(FINALIZACION_PROCESO_TERMINADA, cliente_socket, info_logger);
}

int finalizarProcesoConPid(uint32_t pid){
    return 0;
}

void realizarPedidoLectura(int cliente_socket){
    uint32_t posicion = recibirValor_uint32(cliente_socket, info_logger);
    uint32_t valor = buscarValorEnPosicion(posicion);
    enviarValor_uint32(valor,cliente_socket,VALOR_SOLICITADO,info_logger);
}

int buscarValorEnPosicion(uint32_t posicion){
    return 0;
}


void realizarPedidoEscritura(int cliente_socket){

}
void crearSegmento(int cliente_socket){

}
void eliminarSegmento(int cliente_socket){

}
void compactarSegmentos(int cliente_socket){

}