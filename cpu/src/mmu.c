#include <mmu.h>



int obtener_direccion_logica(char* direccion_logica, char* nombre_instruccion){
    int dl;

    if(strcmp(nombre_instruccion, "MOV_IN") == 0){
        dl = atoi(direccion_logica);
    }else if(strcmp(nombre_instruccion, "MOV_OUT") == 0){
        dl = atoi(direccion_logica);
    }
    return dl;
}

int obtener_direccion_fisica(int direccion_logica) {
    int df;
    int numero_segmento = floor(direccion_logica / cfg_cpu -> TAM_MAX_SEGMENTO);
    int desplazamiento_segmento = direccion_logica %  cfg_cpu -> TAM_MAX_SEGMENTO;

    return df;
}

int es_segmetation_fault(int direccion_fisica, int direccion_logica, int tamaño_a_leer_o_escribir){
    int num_segmento = direccion_logica % cfg_cpu->TAM_MAX_SEGMENTO;
    int desplazamiento_segmento = direccion_logica % cfg_cpu->TAM_MAX_SEGMENTO;

    if(desplazamiento_segmento + tamaño_a_leer_o_escribir > cfg_cpu-> TAM_MAX_SEGMENTO) {
        return 1;
    }else {
            return 0;
        }
}