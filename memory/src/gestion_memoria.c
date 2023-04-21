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

void escribirEnPosicion(uint32_t direccion){

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



bool removerDeHuecosLibres(t_segmento* huecoLibre){

    bool coincidenLimites(t_segmento* segmento){
        return segmento->limite == huecoLibre->limite;
    }
    list_remove_by_condition(huecosLibres,coincidenLimites);
    return true;
}

bool agregarAHuecosUsados(t_segmento* huecoLibre){
    list_add(huecosUsados, huecoLibre);
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






void realizarEliminacionSegmento(uint32_t direccion){

}


uint32_t realizarCompactacion(){

}


void informarTablasActualizadas(uint32_t tablasActualizadas,int cliente_socket)//Esto iria en protocolo.c
{

}
/*





 */




