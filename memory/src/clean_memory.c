//
// Created by utnso on 4/26/23.
//

#include <clean_memory.h>




void limpiarYEliminarListaAuxiliarPeroSinEliminarContenido(t_list* unaLista){
    list_clean(unaLista);
    list_destroy(unaLista);
}

void limpiarHueco(t_segmento* unSegmento){
    free(unSegmento);
}