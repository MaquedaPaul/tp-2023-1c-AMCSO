//
// Created by utnso on 4/17/23.
//

#include <gestion_memoria.h>



bool memoriaInicializada = false;
bool listaDisponiblesCreada = false;
bool listaUsadosCreada = false;
bool tablaSegmentosCreada = false;
bool segmento0Creado = false;
bool semaforosCreados = false;

void* espacio_contiguo;
t_list* tablasSegmentos;
t_list* huecosUsados;
t_list* huecosLibres;
uint32_t espacioDisponible=0;
t_segmento* segmento0;
uint32_t idDisponible = 0;
int cantidadMaximaSegmentos = 0;
uint32_t ipCpu;
uint32_t ipFs;

pthread_mutex_t mutex_espacioContiguo;
pthread_mutex_t mutex_tablasSegmentos;
pthread_mutex_t mutex_huecosUsados;
pthread_mutex_t mutex_huecosDisponibles;
pthread_mutex_t mutex_idSegmento;
pthread_mutex_t mutex_espacioDisponible;




void escribirEnPosicion(uint32_t direccion, void* datos, uint32_t tamanio, uint32_t pid, bool esCpu){
    if(esCpu){
        accesoEspacioUsuarioEscrituraCPU(pid, direccion,tamanio);
    }else{
        accesoEspacioUsuarioEscrituraFS(pid, direccion,tamanio);
    }

    memcpy(espacio_contiguo + direccion, datos, tamanio);

}

void* buscarDatosEnPosicion(uint32_t pid, uint32_t posicion, uint32_t tamanio, bool esCpu){
    void* datos= malloc(tamanio);
    if(esCpu){
        accesoEspacioUsuarioLecturaCPU(pid, posicion, tamanio);
    }else{
        accesoEspacioUsuarioLecturaFS(pid, posicion, tamanio);
    }

    memcpy(datos, espacio_contiguo + posicion, tamanio);

    return datos;
}

bool hayDisponibilidadDeEspacio(uint32_t tamanioSegmento){

    return espacioDisponible > tamanioSegmento;
}
bool elEspacioSeEncuentraEnDiferentesHuecos(uint32_t tamanioSegmento){
    //existeAlMenosUnHuecoDeIgualMayorTamanio()
    bool tieneEspacioSuficiente(t_segmento* unSegmento){
        return unSegmento->limite >= tamanioSegmento;
    }

    bool resultado = list_any_satisfy(huecosLibres, tieneEspacioSuficiente);

    return !resultado;
}

t_tablaSegmentos* buscarEnTablasSegmentos(uint32_t unPid){
    bool coincidenPids(t_tablaSegmentos* unaTabla){
        return unaTabla->pid == unPid;
    }
    pthread_mutex_lock(&mutex_tablasSegmentos);
    t_tablaSegmentos* tablaEncontrada = list_find(tablasSegmentos,coincidenPids);
    pthread_mutex_unlock(&mutex_tablasSegmentos);
    return tablaEncontrada;
}



uint32_t realizarCreacionSegmento(uint32_t pid, t_segmento* huecoLibre, uint32_t tamanio, uint32_t idSegmento){
    t_segmento* segmentoParaAgregar;
    t_tablaSegmentos* tablaEncontrada = buscarEnTablasSegmentos(pid);


    if(huecoLibre->limite == tamanio){
        removerDeHuecosLibres(huecoLibre);
        agregarAHuecosUsados(huecoLibre);
        segmentoParaAgregar = huecoLibre;
    }
    if(huecoLibre->limite < tamanio){
        log_error(error_logger,"Ocurrio algo que no debia pasar, ayuda");

        return -1;
    }
    if(huecoLibre->limite > tamanio){
        segmentoParaAgregar= dividirEnDosYObtenerUtilizado(huecoLibre,tamanio, idSegmento);
    }

    espacioDisponible-=tamanio;
    list_add(tablaEncontrada->segmentos, segmentoParaAgregar);
    creacionSegmento(pid,idSegmento,segmentoParaAgregar->base, segmentoParaAgregar->limite);

    return 0;
}

bool removerDeHuecosUsados(t_segmento* huecoUsado){
    bool coincidenBases(t_segmento* segmento){
        return segmento->base == huecoUsado->base;
    }
    t_segmento* huecoEliminado = list_remove_by_condition(huecosUsados,coincidenBases);
    return huecoEliminado != NULL;
}

bool removerDeHuecosLibres(t_segmento* huecoLibre){
    bool coincidenBases(t_segmento* segmento){
        return segmento->base == huecoLibre->base;
    }
    t_segmento* huecoEliminado = list_remove_by_condition(huecosLibres,coincidenBases);

    return huecoEliminado != NULL;
}

bool agregarAHuecosUsados(t_segmento* huecoLibre){
    list_add(huecosUsados, huecoLibre);
    return true;
}
bool agregarAHuecosLibres(t_segmento* huecoLibre){
    list_add(huecosLibres, huecoLibre);
    return true;
}

t_segmento* dividirEnDosYObtenerUtilizado(t_segmento* huecoLibre, uint32_t tamanio, uint32_t id){
    t_segmento* segmentoSobranteLibre = malloc(sizeof(t_segmento));
    t_segmento* segmentoUtilizado = malloc(sizeof(t_segmento));
    //El segmento que utilizamos tiene que tener su base igual a la del hueco libre
    //pero menor que su limite, por eso usamos el tamanio pasado por parámetro
    segmentoUtilizado->limite=tamanio;
    segmentoUtilizado->base=huecoLibre->base;
    segmentoUtilizado->id= id;

    //El segmento libre tiene que tener como base la misma que el hueco libre, pero sumando el tamanio
    //El limite es limite-tamanio
    uint32_t baseSobrante = huecoLibre->base+tamanio;
    uint32_t limiteSobrante = huecoLibre->limite - tamanio;
    segmentoSobranteLibre->base = baseSobrante;
    segmentoSobranteLibre->limite = limiteSobrante;
    segmentoSobranteLibre->id= -1;

    list_add(huecosUsados, segmentoUtilizado);
    list_add(huecosLibres, segmentoSobranteLibre);
    //Elimino el hueco libre utilizado puesto que ahora se dividió en dos y el sobrante es el nuevo libre
    removerDeHuecosLibres(huecoLibre);
    free(huecoLibre);

    return segmentoUtilizado;
}



t_tablaSegmentos* crearTablaSegmentos(uint32_t pid){
    t_tablaSegmentos* nuevaTabla = malloc(sizeof (t_tablaSegmentos));
    nuevaTabla->pid = pid;
    nuevaTabla->segmentos = list_create();
    list_add(nuevaTabla->segmentos,segmento0);
    creacionProceso(pid);
    return nuevaTabla;
}

t_tablaSegmentos* buscarTablaConPid(uint32_t pid){
    bool coincidenPids(t_tablaSegmentos* unaTabla){
        return unaTabla->pid == pid;
    }

    t_tablaSegmentos* tablaEncontrada =list_find(tablasSegmentos, coincidenPids);

    return tablaEncontrada;
}

t_segmento* buscarSegmentoSegunId(uint32_t unId){
    bool coincideId(t_segmento* segmento){
        return segmento->id == unId;
    }
    t_segmento *usado = list_find(huecosUsados,coincideId);
    t_segmento *libre = list_find(huecosLibres,coincideId);
    if(usado != NULL){
        return usado;
    }
    else if(libre != NULL){
        return libre;
    }
    else {
        log_warning(warning_logger, "Segmento con ID: [%d] no encontrado",unId);
        return NULL;
    }

}

t_segmento* buscarSegmentoEnBaseADireccion(uint32_t direccion){
    bool coincideDireccion(t_segmento* segmento){
        return segmento->base == direccion;
    }
    t_segmento *usado = list_find(huecosUsados,coincideDireccion);
    t_segmento *libre = list_find(huecosLibres,coincideDireccion);
    if(usado != NULL){
        return usado;
    }
    else if(libre != NULL){
        return libre;
    }
    else {
        log_warning(warning_logger, "Segmento con direccion: [%d] no encontrado",direccion);
    return NULL;
    }

}
t_segmento* buscarSegmentoLibreEnBaseADireccion(uint32_t direccion){
    bool coincideDireccion(t_segmento* segmento){
        return segmento->base == direccion;
    }

    t_segmento* segmentoEncontrado =list_find(huecosLibres,coincideDireccion);
    return segmentoEncontrado;

}
t_segmento* sinConocerLaBaseBuscarSegmentoLibreAnteriorA(t_segmento* segmento){
    bool laSumaDeLaBaseYElLimiteEquivaleALaBaseDelSegmentoIngresado(t_segmento* segmento){
        return (segmento->base + segmento->limite+1)== segmento->base;
    }
    return list_find(huecosLibres,laSumaDeLaBaseYElLimiteEquivaleALaBaseDelSegmentoIngresado);
}
bool limpiarSeccionDeMemoria(uint32_t direccion, uint32_t tamanio){

    memset(espacio_contiguo+direccion,0,tamanio);
    espacioDisponible+= tamanio;

    return true;
}


void eliminarDatosSegmento(t_segmento* segmento){
    limpiarSeccionDeMemoria(segmento->base, segmento->limite);
}
void consolidarSegmentos(t_segmento* unSegmento, t_segmento* otroSegmento ){
    t_segmento* nuevoSegmentoLibre = malloc(sizeof (t_segmento));
    if(unSegmento->base < otroSegmento->base){
        nuevoSegmentoLibre->base = unSegmento->base;
    }else{
        nuevoSegmentoLibre->base = otroSegmento->base;

    }

    nuevoSegmentoLibre->limite = unSegmento->limite + otroSegmento->limite;
    if(!removerDeHuecosLibres(unSegmento)){
        log_warning(warning_logger,"Se intento remover un segmento que no era libre");
    }
    if(!removerDeHuecosLibres(otroSegmento)){
        log_warning(warning_logger,"Se intento remover un segmento que no era libre");
    }



    agregarAHuecosLibres(nuevoSegmentoLibre);
}

void realizarEliminacionSegmento(t_segmento* segmento, uint32_t pid){
    eliminacionSegmento(pid, segmento->id,segmento->base,segmento->limite);
    eliminarDatosSegmento(segmento);
    t_tablaSegmentos* tablaEncontrada =buscarTablaConPid(pid);

    bool coincidenDirecciones(t_segmento* unSegmento){
        return unSegmento->base == segmento->base;
    }

    list_remove_by_condition(tablaEncontrada->segmentos, coincidenDirecciones); //TODO LIBERAR Y CONTROLAR

    t_segmento * segmentoLibreSuperior = buscarSegmentoLibreEnBaseADireccion(segmento->base+segmento->limite);
    if(segmentoLibreSuperior != NULL){
        consolidarSegmentos(segmento, segmentoLibreSuperior);
        limpiarHueco(segmentoLibreSuperior);
    }
    t_segmento * segmentoLibreInferior =  sinConocerLaBaseBuscarSegmentoLibreAnteriorA(segmento);
    if(segmentoLibreInferior != NULL){
        consolidarSegmentos(segmento, segmentoLibreInferior);
        limpiarHueco(segmentoLibreInferior);
    }
    limpiarHueco(segmento);

}

void realizarEliminacionSegmentoSinPid(t_segmento* segmento){

    eliminarDatosSegmento(segmento);
    t_segmento * segmentoLibreSuperior = buscarSegmentoLibreEnBaseADireccion(segmento->base+segmento->limite);
    if(segmentoLibreSuperior != NULL){
        consolidarSegmentos(segmento, segmentoLibreSuperior);
        limpiarHueco(segmentoLibreSuperior);
    }
    t_segmento * segmentoLibreInferior =  sinConocerLaBaseBuscarSegmentoLibreAnteriorA(segmento);
    if(segmentoLibreInferior != NULL){
        consolidarSegmentos(segmento, segmentoLibreInferior);
        limpiarHueco(segmentoLibreInferior);
    }
    limpiarHueco(segmento);

}

t_segmento* buscarPrimerHuecoLibre(){
    bool esLaMenorBase(t_segmento* segmento,t_segmento* otroSegmento){
        return segmento-> base < otroSegmento->base;
    }
    pthread_mutex_lock(&mutex_huecosDisponibles);
    list_sort(huecosLibres, esLaMenorBase);
    pthread_mutex_unlock(&mutex_huecosDisponibles);

    t_segmento* segmentoMenor = list_get(huecosLibres,0);
    return segmentoMenor;
}


bool intercambiarDatosSegmentosEnMp(t_segmento* unSegmento, t_segmento* otroSegmento){
    void* datosPrimer = malloc(sizeof(unSegmento->limite));
    void* datosSegundo = malloc(sizeof(otroSegmento->limite));
    memcpy(datosPrimer, espacio_contiguo+unSegmento->base, unSegmento->limite);
    memcpy(datosSegundo, espacio_contiguo+otroSegmento->base, otroSegmento->limite);
    memcpy(espacio_contiguo+unSegmento->base,datosSegundo,otroSegmento->limite);
    memcpy(espacio_contiguo+otroSegmento->base,datosPrimer,unSegmento->limite);

    return true;
}
bool consolidarSiExistenAledaniosA(t_segmento* segmentoLibre){

    t_segmento * segmentoLibreSuperior = buscarSegmentoLibreEnBaseADireccion(segmentoLibre->base+segmentoLibre->limite);
    bool seConsolido= false;
    if(segmentoLibreSuperior != NULL){
        consolidarSegmentos(segmentoLibre, segmentoLibreSuperior);
        limpiarHueco(segmentoLibreSuperior);
        seConsolido=true;
    }

    //NO HARIA FALTA
    t_segmento * segmentoLibreInferior =  sinConocerLaBaseBuscarSegmentoLibreAnteriorA(segmentoLibre);
    if(segmentoLibreInferior != NULL){
        consolidarSegmentos(segmentoLibre, segmentoLibreInferior);
        limpiarHueco(segmentoLibreSuperior);
        seConsolido=true;
    }
    if(seConsolido){
        limpiarHueco(segmentoLibre);
    }
}


void compactarSegmentos(){
    t_segmento* primerHuecoLibre = buscarPrimerHuecoLibre();
    t_segmento* siguienteSegmentoUsado = buscarSegmentoEnBaseADireccion(primerHuecoLibre->base +primerHuecoLibre->limite);
    uint32_t siguienteBase = primerHuecoLibre->base;
    uint32_t siguienteLimite = primerHuecoLibre->limite;
    intercambiarDatosSegmentosEnMp(primerHuecoLibre, siguienteSegmentoUsado);
    removerDeHuecosUsados(siguienteSegmentoUsado);
    removerDeHuecosLibres(primerHuecoLibre);
    primerHuecoLibre->base = siguienteSegmentoUsado->base;
    primerHuecoLibre->limite = siguienteSegmentoUsado->limite;
    siguienteSegmentoUsado->base = siguienteBase;
    siguienteSegmentoUsado->limite = siguienteLimite;
    agregarAHuecosUsados(siguienteSegmentoUsado);
    consolidarSiExistenAledaniosA(primerHuecoLibre);

}

bool hayHuecosIntercalados(){
    bool ordenarSegunBase(t_segmento* unSegmento, t_segmento* otroSegmento){
        return unSegmento->base < otroSegmento->base;
    }
    list_sort(huecosLibres, ordenarSegunBase);

    bool elLimiteNoCoincideConLaBaseDelSiguiente(t_segmento* unSegmento){
        t_segmento* siguienteSegmento = buscarSegmentoLibreEnBaseADireccion(unSegmento->base +unSegmento->limite);
        if(siguienteSegmento == NULL){
            return false;
        }
        return true;
    }

    list_any_satisfy(huecosLibres, elLimiteNoCoincideConLaBaseDelSiguiente);

    return true;
}



uint32_t realizarCompactacion(){
    while(list_size(huecosLibres) >= 2 || hayHuecosIntercalados()){
        compactarSegmentos();
    }
    void detallarTablas(t_tablaSegmentos* unaTabla){
        uint32_t pid = unaTabla->pid;
        void detallarSegmento(t_segmento* unSegmento){
            uint32_t idSegmento = unSegmento->id;
            uint32_t direccionBase = unSegmento->base;
            uint32_t tamanio= unSegmento->limite;
            resultadoCompactacion(pid,idSegmento,direccionBase,tamanio);
        }
        list_iterate(unaTabla->segmentos,detallarSegmento);
    }
    list_iterate(tablasSegmentos,detallarTablas);
    return 0;
}


void informarTablasActualizadas(int cliente_socket)
{
    enviarTablasSegmentos(tablasSegmentos,cliente_socket, info_logger,COMPACTACION_FINALIZADA);
}
/*





 */




