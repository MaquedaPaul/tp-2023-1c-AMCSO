//
// Created by utnso on 4/7/23.
//

#include <algoritmos.h>



t_segmento* firstFit(uint32_t tamanioSegmento){
    t_segmento* huecoLibre;
    bool tieneMenorBase(t_segmento* unSegmento, t_segmento* otroSegmento){
        return unSegmento->base < otroSegmento->base;
    }
    list_sort(huecosLibres, tieneMenorBase);
    bool tieneElMinimoIndispensable(t_segmento* segmento){
        return segmento->limite >= tamanioSegmento;

    }
    huecoLibre = list_find(huecosLibres, tieneElMinimoIndispensable);

    return huecoLibre;
}
t_segmento* bestFit(uint32_t tamanioSegmento){
    t_segmento* huecoLibre;
    bool ordenarLimiteMenorAMayor(t_segmento* unSegmento, t_segmento* otroSegmento){
        return unSegmento->limite < otroSegmento->limite;
    }
    list_sort(huecosLibres, ordenarLimiteMenorAMayor);
    bool tieneElMinimoIndispensable(t_segmento* segmento){
        return segmento->limite >= tamanioSegmento;

    }
    huecoLibre = list_find(huecosLibres, tieneElMinimoIndispensable);
    return huecoLibre;
}
t_segmento* worstFit(uint32_t tamanioSegmento){
    t_segmento* huecoLibre;
    bool ordenarLimiteMayorAMenor(t_segmento* unSegmento, t_segmento* otroSegmento){
        return unSegmento->limite > otroSegmento->limite;
    }
    list_sort(huecosLibres, ordenarLimiteMayorAMenor);
    bool tieneElMinimoIndispensable(t_segmento* segmento){
        return segmento->limite >= tamanioSegmento;

    }
    huecoLibre = list_find(huecosLibres, tieneElMinimoIndispensable);
    return huecoLibre;
}


t_segmento* buscarHuecoLibre(uint32_t tamanioSegmento){
    char* algoritmoAsignacion = cfg_memory->ALGORITMO_ASIGNACION;
    t_segmento* huecoLibre;
    if(strcmp(algoritmoAsignacion,"FIRST") == 0){
        huecoLibre= firstFit(tamanioSegmento);
    }
    if(strcmp(algoritmoAsignacion,"BEST") == 0){
        huecoLibre= bestFit(tamanioSegmento);
    }
    if(strcmp(algoritmoAsignacion,"WORST") == 0){
        huecoLibre= worstFit(tamanioSegmento);
    }
    return huecoLibre;
}