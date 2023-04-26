//
// Created by utnso on 4/17/23.
//

#include "memory.h"


void inicializarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket,info_logger);
    t_tablaSegmentos * nuevaTabla = crearTablaSegmentos(pid);
    pthread_mutex_lock(&mutex_tablasSegmentos);
    list_add(tablasSegmentos, nuevaTabla);
    pthread_mutex_unlock(&mutex_tablasSegmentos);
    //enviarTablaSegmentos(cliente_socket); //TODO
}


void finalizarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket, info_logger);
    finalizarProcesoConPid(pid);
    enviarOrden(FINALIZACION_PROCESO_TERMINADA, cliente_socket, info_logger);
}

int finalizarProcesoConPid(uint32_t unPid){
    pthread_mutex_lock(&mutex_tablasSegmentos);
    pthread_mutex_lock(&mutex_espacioContiguo);
    pthread_mutex_lock(&mutex_espacioDisponible);
    pthread_mutex_lock(&mutex_huecosDisponibles);
    pthread_mutex_lock(&mutex_huecosUsados);


    t_tablaSegmentos* tablaEncontrada = buscarTablaConPid(unPid);
    bool coincideId(t_segmento* unSegmento){
        return unSegmento->id == segmento0->id;
    }

    list_remove_by_condition(tablaEncontrada->segmentos, coincideId);

    list_iterate(tablaEncontrada->segmentos,realizarEliminacionSegmentoSinPid);
    list_clean(tablaEncontrada->segmentos); //TODO FALTARIA EL DESTROY
    bool coincidePid(t_tablaSegmentos* tablaEncontrada){
        return tablaEncontrada->pid == unPid;
    }

    list_remove_by_condition(tablasSegmentos,coincidePid);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    pthread_mutex_unlock(&mutex_espacioDisponible);
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&mutex_tablasSegmentos);
    eliminacionProceso(unPid);
    return 0;
}


void realizarPedidoLectura(int cliente_socket){
    uint32_t posicion = recibirValor_uint32(cliente_socket, info_logger);
    bool esCpu;
    uint32_t tamanio;
    uint32_t pid;
    //TODO Definir si el valor es uint32_t o void*
    pthread_mutex_lock(&mutex_espacioContiguo);
    void* datos = buscarDatosEnPosicion(pid, posicion, tamanio, esCpu);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    //enviarDatos(datos, cliente_socket, VALOR_SOLICITADO, info_logger);
    //enviarValor_uint32(valor,cliente_socket,VALOR_SOLICITADO,info_logger);
}




void realizarPedidoEscritura(int cliente_socket){
    uint32_t direccion = recibirValor_uint32(cliente_socket, info_logger);
    bool esCpu;
    uint32_t pid;
    uint32_t tamanio;
    void* datos;
    pthread_mutex_lock(&mutex_espacioContiguo);
    escribirEnPosicion(direccion,datos, tamanio, pid,esCpu);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarOrden(ESCRITURA_REALIZADA, cliente_socket, info_logger);
}

void crearSegmento(int cliente_socket) {
    t_list *listaInts;// = recibirListaInts(cliente_socket); //TODO HACER FUNCION
    uint32_t pid = list_get(listaInts, 0);
    uint32_t tamanioSegmento = list_get(listaInts, 1);
    //uint32_t pid= recibirValor_uint32(cliente_socket,info_logger);
    pthread_mutex_lock(&mutex_espacioDisponible);
    if (!hayDisponibilidadDeEspacio(tamanioSegmento)) {
        enviarOrden(SIN_ESPACIO_DISPONIBLE, cliente_socket, info_logger);
        pthread_mutex_unlock(&mutex_espacioDisponible);
        return;
    }
    pthread_mutex_unlock(&mutex_espacioDisponible);
    pthread_mutex_lock(&mutex_huecosDisponibles);

    if(elEspacioSeEncuentraEnDiferentesHuecos()){
        enviarOrden(SE_NECESITA_COMPACTACION,cliente_socket,info_logger);
        pthread_mutex_unlock(&mutex_huecosDisponibles);
        return;
    }
    pthread_mutex_lock(&mutex_huecosUsados);
    pthread_mutex_lock(&mutex_idSegmento);
    pthread_mutex_lock(&mutex_espacioDisponible);
    t_segmento* huecoLibre = buscarHuecoLibre(tamanioSegmento);

    uint32_t direccion = realizarCreacionSegmento(pid, huecoLibre, tamanioSegmento);
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&mutex_idSegmento);
    pthread_mutex_unlock(&mutex_espacioDisponible);
    enviarValor_uint32(direccion,cliente_socket,CREACION_SEGMENTO_EXITOSO,info_logger);

}
void eliminarSegmento(int cliente_socket){
    uint32_t direccion= recibirValor_uint32(cliente_socket,info_logger); //direccion de segmento?
    uint32_t pid;
    pthread_mutex_lock(&mutex_huecosDisponibles);
    pthread_mutex_lock(&mutex_huecosUsados);
    pthread_mutex_lock(&mutex_espacioDisponible);
    pthread_mutex_lock(&tablasSegmentos);
    pthread_mutex_lock(&espacio_contiguo);

    t_segmento* segmentoAEliminar = buscarSegmentoEnBaseADireccion(direccion);

    realizarEliminacionSegmento(segmentoAEliminar,pid);
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&mutex_espacioDisponible);
    pthread_mutex_unlock(&tablasSegmentos);
    pthread_mutex_unlock(&espacio_contiguo);
    //Deberia informar a kernel de la eliminacion? no dice nada en el tp
}

void compactacionSegmentos(int cliente_socket){
    recibirOrden(cliente_socket);
    inicioCompactacion();
    pthread_mutex_lock(&mutex_huecosDisponibles);
    pthread_mutex_lock(&mutex_huecosUsados);
    pthread_mutex_lock(&tablasSegmentos);
    pthread_mutex_lock(&espacio_contiguo);
    realizarCompactacion();
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&tablasSegmentos);
    pthread_mutex_unlock(&espacio_contiguo);
    informarTablasActualizadas(cliente_socket);

}