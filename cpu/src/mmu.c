#include <mmu.h>

int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes ) {
    num_segmento = floor(direccion_logica / cfg_cpu->TAM_MAX_SEGMENTO);            // DUDA       no me anda el floor con una variable int
    int desplazamiento_segmento = direccion_logica % cfg_cpu->TAM_MAX_SEGMENTO;

    if (error_segmentationFault(desplazamiento_segmento, cantidad_de_bytes)) {
        copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
/*
        t_paquete* paquete = crear_paquete(SEGMENTATION_FAULT, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        enviar_paquete(paquete, fd_kernel);
        eliminar_paquete(paquete, info_logger);
*/
        enviar_paquete_pcb(pcb_actual, fd_kernel,SEGMENTATION_FAULT, info_logger);

        cicloInstruccionesDebeEjecutarse = false;


        log_info(info_logger, "PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamaño: <%d>", pcb_actual->id, num_segmento, desplazamiento_segmento, segmento->limite );

        return -1;
    }

    int direccion_fisica = segmento->base + desplazamiento_segmento;

    return direccion_fisica;
}

bool error_segmentationFault(int desplazamiento_segmento, int cantidad_bytes) {

    //if (numero_segmento >= list_size(pcb_actual->tabla_segmentos))  return true;

    segmento = list_get(pcb_actual->tablaSegmentos, num_segmento);

/* 
int i;
for (i = 0; i < list_size(pcb_actual->tablaSegmentos); i++) {

        t_segmento*  segmento_aux = list_get(pcb_actual->tablaSegmentos, i);

             if(segmento_aux->id == num_segmento){

                      segmento = segmento_aux;

                      break;             
                }

}

 */


    return ((desplazamiento_segmento + cantidad_bytes) > segmento->limite);
}






