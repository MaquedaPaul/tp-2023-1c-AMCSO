//
// Created by utnso on 4/17/23.
//

#include <gestion_memoria.h>
void* espacio_contiguo;
t_list* tablasSegmentos;
t_list* huecosUsados;
t_list* huecosLibres;


void escribirEnPosicion(uint32_t direccion){

}

bool hayDisponibilidadDeEspacio(){
    return true;
}
bool elEspacioSeEncuentraEnDiferentesHuecos(){
    return true;
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
        segmentoParaAgregar= dividirEnDos(huecoLibre,tamanio);
    }



    list_add(tablaEncontrada->segmentos, segmentoParaAgregar);
    return 0;
}



bool removerDeHuecosLibres(t_segmento* huecoLibre){
    return true;
}
bool agregarAHuecosUsados(t_segmento* huecoLibre){
    return true;
}
t_segmento* dividirEnDos(t_segmento* huecoLibre,uint32_t tamanio){
    t_segmento* segmento;
    return segmento;
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




