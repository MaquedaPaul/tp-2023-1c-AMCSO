//
// Created by utnso on 4/17/23.
//

#include <gestion_memoria.h>
void* espacio_contiguo;
t_list* tablasSegmentos;
t_list* huecosUsados;
t_list* huecosLibres;
uint32_t espacioDisponible=0;
t_segmento* segmento0;
int cantidadMaximaSegmentos = 0;

void escribirEnPosicion(uint32_t direccion, uint32_t valor){
    memcpy(espacio_contiguo + direccion, &valor, sizeof(uint32_t));
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
    t_tablaSegmentos* tablaEncontrada = list_find(tablasSegmentos,coincidenPids);
    return tablaEncontrada;
}



uint32_t realizarCreacionSegmento(uint32_t pid, t_segmento* huecoLibre, uint32_t tamanio){
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
        segmentoParaAgregar= dividirEnDosYObtenerUtilizado(huecoLibre,tamanio);
    }

    list_add(tablaEncontrada->segmentos, segmentoParaAgregar);
    return 0;
}

bool removerDeHuecosUsados(t_segmento* huecoUsado){
    bool coincidenBases(t_segmento* segmento){
        return segmento->base == huecoUsado->base;
    }
    list_remove_by_condition(huecosUsados,coincidenBases);
    return true;
}

bool removerDeHuecosLibres(t_segmento* huecoLibre){
    bool coincidenBases(t_segmento* segmento){
        return segmento->base == huecoLibre->base;
    }
    list_remove_by_condition(huecosLibres,coincidenBases);
    return true;
}

bool agregarAHuecosUsados(t_segmento* huecoLibre){
    list_add(huecosUsados, huecoLibre);
    return true;
}
bool agregarAHuecosLibres(t_segmento* huecoLibre){
    list_add(huecosLibres, huecoLibre);
    return true;
}

t_segmento* dividirEnDosYObtenerUtilizado(t_segmento* huecoLibre, uint32_t tamanio){
    t_segmento* segmentoSobranteLibre = malloc(sizeof(t_segmento));
    t_segmento* segmentoUtilizado = malloc(sizeof(t_segmento));
    //El segmento que utilizamos tiene que tener su base igual a la del hueco libre
    //pero menor que su limite, por eso usamos el tamanio pasado por parámetro
    segmentoUtilizado->limite=tamanio;
    segmentoUtilizado->base=huecoLibre->base;
    //El segmento libre tiene que tener como base la misma que el hueco libre, pero sumando el tamanio
    //El limite es limite-tamanio
    uint32_t baseSobrante = huecoLibre->base+tamanio;
    uint32_t limiteSobrante = huecoLibre->limite - tamanio;
    segmentoSobranteLibre->base = baseSobrante;
    segmentoSobranteLibre->limite = limiteSobrante;
    list_add(huecosUsados, segmentoUtilizado);
    list_add(huecosLibres, segmentoSobranteLibre);

    return segmentoUtilizado;
}



t_tablaSegmentos* crearTablaSegmentos(uint32_t pid){
    t_tablaSegmentos* nuevaTabla = malloc(sizeof (t_tablaSegmentos));
    nuevaTabla->pid = pid;
    list_add(nuevaTabla->segmentos,segmento0);
    return nuevaTabla;
}

t_tablaSegmentos* buscarTablaConPid(uint32_t pid){
    bool coincidenPids(t_tablaSegmentos* unaTabla){
        return unaTabla->pid == pid;
    }
    return list_find(tablasSegmentos, coincidenPids);
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
    return list_find(huecosLibres,coincideDireccion);

}
t_segmento* sinConocerLaBaseBuscarSegmentoLibreAnteriorA(t_segmento* segmento){
    bool laSumaDeLaBaseYElLimiteEquivaleALaBaseDelSegmentoIngresado(t_segmento* segmento){
        return (segmento->base + segmento->limite+1)== segmento->base;
    }
    return list_find(huecosLibres,laSumaDeLaBaseYElLimiteEquivaleALaBaseDelSegmentoIngresado);
}
void eliminarDatosSegmento(t_segmento* segmento){

}
void consolidarSegmentos(t_segmento* unSegmento, t_segmento* otroSegmento ){
    t_segmento* nuevoSegmentoLibre = malloc(sizeof (t_segmento));
    if(unSegmento->base < otroSegmento->base){
        nuevoSegmentoLibre->base = unSegmento->base;
    }else{
        nuevoSegmentoLibre->base = otroSegmento->base;

    }
    nuevoSegmentoLibre->limite = unSegmento->limite + otroSegmento->limite;
    removerDeHuecosLibres(unSegmento);
    removerDeHuecosLibres(otroSegmento);
    agregarAHuecosLibres(nuevoSegmentoLibre);
    //TODO Frees de los segmentos
}

void realizarEliminacionSegmento(t_segmento* segmento, uint32_t pid){
    eliminarDatosSegmento(segmento);
    t_tablaSegmentos* tablaEncontrada =buscarTablaConPid(pid);
    bool coincidenDirecciones(t_segmento* unSegmento){
        return unSegmento->base == segmento->base;
    }
    list_remove_by_condition(tablaEncontrada->segmentos, coincidenDirecciones); //TODO LIBERAR Y CONTROLAR

    t_segmento * segmentoLibreSuperior = buscarSegmentoLibreEnBaseADireccion(segmento->base+segmento->limite+1);
    if(segmentoLibreSuperior != NULL){
        consolidarSegmentos(segmento, segmentoLibreSuperior);
    }
    t_segmento * segmentoLibreInferior =  sinConocerLaBaseBuscarSegmentoLibreAnteriorA(segmento);
    if(segmentoLibreInferior != NULL){
        consolidarSegmentos(segmento, segmentoLibreInferior);
    }

}

void realizarEliminacionSegmentoSinPid(t_segmento* segmento){
    eliminarDatosSegmento(segmento);
    t_segmento * segmentoLibreSuperior = buscarSegmentoLibreEnBaseADireccion(segmento->base+segmento->limite+1);
    if(segmentoLibreSuperior != NULL){
        consolidarSegmentos(segmento, segmentoLibreSuperior);
    }
    t_segmento * segmentoLibreInferior =  sinConocerLaBaseBuscarSegmentoLibreAnteriorA(segmento);
    if(segmentoLibreInferior != NULL){
        consolidarSegmentos(segmento, segmentoLibreInferior);
    }

}

t_segmento* buscarPrimerHuecoLibre(){
    bool esLaMenorBase(t_segmento* segmento,t_segmento* otroSegmento){
        return segmento-> base < otroSegmento->base;
    }
    list_sort(huecosLibres, esLaMenorBase);
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

    t_segmento * segmentoLibreSuperior = buscarSegmentoLibreEnBaseADireccion(segmentoLibre->base+segmentoLibre->limite+1);
    if(segmentoLibreSuperior != NULL){
        consolidarSegmentos(segmentoLibre, segmentoLibreSuperior);
    }

    //NO HARIA FALTA
    t_segmento * segmentoLibreInferior =  sinConocerLaBaseBuscarSegmentoLibreAnteriorA(segmentoLibre);
    if(segmentoLibreInferior != NULL){
        consolidarSegmentos(segmentoLibre, segmentoLibreInferior);
    }
}


void compactarSegmentos(){
    t_segmento* primerHuecoLibre = buscarPrimerHuecoLibre();
    t_segmento* siguienteSegmentoUsado = buscarSegmentoEnBaseADireccion(primerHuecoLibre->base +primerHuecoLibre->limite+1);
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
        t_segmento* siguienteSegmento = buscarSegmentoLibreEnBaseADireccion(unSegmento->base +unSegmento->limite+1);
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

    return 0;
}


void informarTablasActualizadas(int cliente_socket)//Esto iria en protocolo.c
{

}
/*





 */




