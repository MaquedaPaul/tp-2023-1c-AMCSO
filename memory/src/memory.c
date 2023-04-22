//
// Created by utnso on 4/17/23.
//

#include "memory.h"


void inicializarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket,info_logger);
    t_tablaSegmentos * nuevaTabla = crearTablaSegmentos(pid);
    list_add(tablasSegmentos, nuevaTabla);

    //enviarTablaSegmentos(cliente_socket); //TODO
}


void finalizarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket, info_logger);
    finalizarProcesoConPid(pid);
    enviarOrden(FINALIZACION_PROCESO_TERMINADA, cliente_socket, info_logger);
}

int finalizarProcesoConPid(uint32_t unPid){
    t_tablaSegmentos* tablaEncontrada = buscarTablaConPid(unPid);
    bool coincideBase(t_segmento* unSegmento){
        return unSegmento->base == segmento0->base;
    }
    list_remove_by_condition(tablaEncontrada->segmentos, coincideBase);
    list_iterate(tablaEncontrada->segmentos,realizarEliminacionSegmentoSinPid);
    list_clean(tablaEncontrada->segmentos); //TODO FALTARIA EL DESTROY
    bool coincidePid(t_tablaSegmentos* tablaEncontrada){
        return tablaEncontrada->pid == unPid;
    }
    list_remove_by_condition(tablasSegmentos,coincidePid);
    return 0;
}

void realizarPedidoLectura(int cliente_socket){
    uint32_t posicion = recibirValor_uint32(cliente_socket, info_logger);
    uint32_t valor = buscarValorEnPosicion(posicion);
    enviarValor_uint32(valor,cliente_socket,VALOR_SOLICITADO,info_logger);
}

int buscarValorEnPosicion(uint32_t posicion){
    uint32_t valor;
    memcpy(&valor, espacio_contiguo + posicion, sizeof (uint32_t));
    return valor;
}


void realizarPedidoEscritura(int cliente_socket){
    uint32_t direccion = recibirValor_uint32(cliente_socket, info_logger);
    uint32_t valor;
    escribirEnPosicion(direccion, valor);
    enviarOrden(ESCRITURA_REALIZADA, cliente_socket, info_logger);
}

void crearSegmento(int cliente_socket){
    t_list* listaInts;// = recibirListaInts(cliente_socket); //TODO HACER FUNCION
    uint32_t pid = list_get(listaInts, 0);
    uint32_t tamanioSegmento = list_get(listaInts, 1);
    //uint32_t pid= recibirValor_uint32(cliente_socket,info_logger);
    if(!hayDisponibilidadDeEspacio(tamanioSegmento)){
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
    uint32_t pid;
    t_segmento* segmentoAEliminar = buscarSegmentoEnBaseADireccion(direccion);
    realizarEliminacionSegmento(segmentoAEliminar,pid);
    //Deberia informar a kernel de la eliminacion? no dice nada en el tp
}

void compactacionSegmentos(int cliente_socket){
    recibirOrden(cliente_socket);
    realizarCompactacion();
    informarTablasActualizadas(cliente_socket);
}