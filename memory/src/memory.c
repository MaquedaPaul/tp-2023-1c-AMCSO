//
// Created by utnso on 4/17/23.
//

#include "memory.h"


void inicializarProceso(int cliente_socket){
    uint32_t pid = recibirValor_uint32(cliente_socket,info_logger);
//    t_tablaSegmentos * nuevaTabla = crearTablaSegmentos(pid);
 //   pthread_mutex_lock(&mutex_tablasSegmentos);
   // list_add(tablasSegmentos, nuevaTabla);
    //pthread_mutex_unlock(&mutex_tablasSegmentos);
    t_list* listaConUnElemento = list_create();
    //list_add(listaConUnElemento, nuevaTabla);

    enviarTablasSegmentos(listaConUnElemento, cliente_socket, info_logger,ESTRUCTURAS_INICALIZADAS);
    //limpiarYEliminarListaAuxiliarPeroSinEliminarContenido(listaConUnElemento);
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
    list_clean(tablaEncontrada->segmentos);
    list_destroy(tablaEncontrada->segmentos);

    bool coincidePid(t_tablaSegmentos* tablaEncontrada){
        return tablaEncontrada->pid == unPid;
    }

    list_remove_by_condition(tablasSegmentos,coincidePid);
    free(tablaEncontrada);
    eliminacionProceso(unPid);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    pthread_mutex_unlock(&mutex_espacioDisponible);
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&mutex_tablasSegmentos);

    return 0;
}


void realizarPedidoLectura(int cliente_socket){
    t_list* listaInts = recibirListaUint32_t(cliente_socket);
    uint32_t* posicion = list_get(listaInts,0);
    bool esCpu= cliente_socket == ipCpu;
    uint32_t* tamanio = list_get(listaInts,1);
    uint32_t* pid = list_get(listaInts,2);
    pthread_mutex_lock(&mutex_espacioContiguo);
    accesoEspacioUsuarioLecturaRetardoPrevio(*posicion, *tamanio, *pid);
    simularRetardoSinMensaje(cfg_memory->RETARDO_MEMORIA);
    accesoEspacioUsuarioLecturaRetardoConcedido();
    void* datos = buscarDatosEnPosicion(*pid, *posicion, *tamanio, esCpu);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarDatos(datos,*tamanio, LECTURA_REALIZADA,cliente_socket , info_logger);
}




void realizarPedidoEscritura(int cliente_socket){
    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(cliente_socket, unosDatos);
    uint32_t* posicion = list_get(listaInts,0);
    bool esCpu= cliente_socket == ipCpu;
    uint32_t* pid = list_get(listaInts,2);
    pthread_mutex_lock(&mutex_espacioContiguo);
    accesoEspacioUsuarioEscrituraRetardoPrevio(*posicion, *pid);
    simularRetardoSinMensaje(cfg_memory->RETARDO_MEMORIA);
    accesoEspacioUsuarioEscrituraRetardoConcedido();
    escribirEnPosicion(*posicion,unosDatos->datos, unosDatos->tamanio, *pid,esCpu);

    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarOrden(ESCRITURA_REALIZADA, cliente_socket, info_logger);
}

void crearSegmento(int cliente_socket) {
    t_list *listaInts = recibirListaUint32_t(cliente_socket);
    uint32_t* pid = list_get(listaInts, 0);
    uint32_t* tamanioSegmento = list_get(listaInts, 1);
    pthread_mutex_lock(&mutex_espacioDisponible);
    if (!hayDisponibilidadDeEspacio(*tamanioSegmento)) {
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
    t_segmento* huecoLibre = buscarHuecoLibre(*tamanioSegmento);
    if(huecoLibre == NULL){
        log_error(error_logger,"Ocurrio algo que no debia pasar, ayuda :(");
    }

    uint32_t direccion = realizarCreacionSegmento(*pid, huecoLibre, *tamanioSegmento);
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&mutex_idSegmento);
    pthread_mutex_unlock(&mutex_espacioDisponible);
    enviarValor_uint32(direccion,cliente_socket,CREACION_SEGMENTO_EXITOSO,info_logger);

}
void eliminarSegmento(int cliente_socket){
    t_list *listaInts = recibirListaUint32_t(cliente_socket);
    uint32_t* pid = list_get(listaInts, 0);
    uint32_t* idSegmento = list_get(listaInts, 1);

    pthread_mutex_lock(&mutex_huecosDisponibles);
    pthread_mutex_lock(&mutex_huecosUsados);
    pthread_mutex_lock(&mutex_espacioDisponible);
    pthread_mutex_lock(&tablasSegmentos);
    pthread_mutex_lock(&espacio_contiguo);

    t_segmento* segmentoAEliminar = buscarSegmentoSegunId(*idSegmento);
    realizarEliminacionSegmento(segmentoAEliminar,*pid);
    t_tablaSegmentos* tablaAEnviar = buscarTablaConPid(*pid);
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&mutex_espacioDisponible);
    pthread_mutex_unlock(&tablasSegmentos);
    pthread_mutex_unlock(&espacio_contiguo);
    enviarTablasSegmentos(tablaAEnviar,cliente_socket, info_logger,SEGMENTO_ELIMINADO);
    //Deberia informar a kernel de la eliminacion? no dice nada en el tp //TODO
}

void compactacionSegmentos(int cliente_socket){
    recibirOrden(cliente_socket);
    inicioCompactacion();
    compactacionRetardoPrevio(cfg_memory->RETARDO_MEMORIA);
    simularRetardoSinMensaje(cfg_memory->RETARDO_MEMORIA);
    pthread_mutex_lock(&mutex_huecosDisponibles);
    pthread_mutex_lock(&mutex_huecosUsados);
    pthread_mutex_lock(&tablasSegmentos);
    pthread_mutex_lock(&espacio_contiguo);
    realizarCompactacion();
    pthread_mutex_unlock(&mutex_huecosDisponibles);
    pthread_mutex_unlock(&mutex_huecosUsados);
    pthread_mutex_unlock(&tablasSegmentos);
    pthread_mutex_unlock(&espacio_contiguo);
    compactacionRetardoTerminada();
    informarTablasActualizadas(cliente_socket);

}

