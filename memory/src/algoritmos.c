//
// Created by utnso on 4/7/23.
//

#include <algoritmos.h>



t_segmento* firstFit(){
    t_segmento* huecoLibre;
    return huecoLibre;
}
t_segmento* bestFit(){
    t_segmento* huecoLibre;
    return huecoLibre;
}
t_segmento* worstFit(){
    t_segmento* huecoLibre;
    return huecoLibre;
}


t_segmento* buscarHuecoLibre(uint32_t tamanioSegmento){
    char* algoritmoAsignacion = cfg_memory->ALGORITMO_ASIGNACION;
    t_segmento* huecoLibre;
    if(strcmp(algoritmoAsignacion,"FIRST") == 0){
        huecoLibre= firstFit();
    }
    if(strcmp(algoritmoAsignacion,"BEST") == 0){
        huecoLibre= bestFit();
    }
    if(strcmp(algoritmoAsignacion,"WORST") == 0){
        huecoLibre= worstFit();
    }
    return huecoLibre;
}