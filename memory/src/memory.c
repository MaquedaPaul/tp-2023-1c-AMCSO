//
// Created by utnso on 4/17/23.
//

#include "memory.h"


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
    uint32_t direccion = recibirValor_uint32(cliente_socket, info_logger);
    escribirEnPosicion(direccion);
    enviarOrden(ESCRITURA_REALIZADA, cliente_socket, info_logger);
}

void crearSegmento(int cliente_socket){
    t_list* listaInts;// = recibirListaInts(cliente_socket); //TODO HACER FUNCION
    uint32_t pid = list_get(listaInts, 0);
    uint32_t tamanioSegmento = list_get(listaInts, 1);
    //uint32_t pid= recibirValor_uint32(cliente_socket,info_logger);
    if(!hayDisponibilidadDeEspacio()){
        enviarOrden(SIN_ESPACIO_DISPONIBLE,cliente_socket,info_logger);
        return;
    }
    if(elEspacioSeEncuentraEnDiferentesHuecos()){
        enviarOrden(SE_NECESITA_COMPACTACION,cliente_socket,info_logger);
        return;
    }
    t_segmento* huecoLibre = buscarHuecoLibre(tamanioSegmento);
    uint32_t direccion = realizarCreacionSegmento(pid, huecoLibre, tamanioSegmento);
    enviarValor_uint32(direccion,cliente_socket,CREACION_SEGMENTO_EXITOSO,info_logger);

}
void eliminarSegmento(int cliente_socket){
    uint32_t direccion= recibirValor_uint32(cliente_socket,info_logger); //direccion de segmento?
    realizarEliminacionSegmento(direccion);
    //Deberia informar a kernel de la eliminacion? no dice nada en el tp
}

void compactarSegmentos(int cliente_socket){
    recibirOrden(cliente_socket);
    int tablasActualizadas = realizarCompactacion();
    informarTablasActualizadas(tablasActualizadas, cliente_socket);
}