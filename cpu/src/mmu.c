#include <mmu.h>

char* ip_cpu;
char* puerto_cpu;
int fd_memoria;
int fd_kernel;
int fd_cpu;

t_segmento* segmento;
int cliente_servidor;
int num_segmento;


char registroCPU_AX[4];
char registroCPU_BX[4];
char registroCPU_CX[4];
char registroCPU_DX[4];
char registroCPU_EAX[8];
char registroCPU_EBX[8];
char registroCPU_ECX[8];
char registroCPU_EDX[8];
char registroCPU_RAX[16];
char registroCPU_RBX[16];
char registroCPU_RCX[16];
char registroCPU_RDX[16];

int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes ) {
    log_info(info_logger,"valor de dir logica : %d", direccion_logica);
    log_info(info_logger,"tamaniomaxseg: %d", cfg_cpu->TAM_MAX_SEGMENTO);

    num_segmento = direccion_logica / cfg_cpu->TAM_MAX_SEGMENTO;

    log_info(info_logger,"numero de segmento: %d", num_segmento);

    int desplazamiento_segmento = direccion_logica % cfg_cpu->TAM_MAX_SEGMENTO;

    log_info(info_logger,"desplazamiento: %d", desplazamiento_segmento);

    if (error_segmentationFault(desplazamiento_segmento, cantidad_de_bytes)) {
        copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
/*
        t_paquete* paquete = crear_paquete(SEGMENTATION_FAULT, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        enviar_paquete(paquete, fd_kernel);
        eliminar_paquete(paquete, info_logger);
*/
        enviar_paquete_pcb2(pcb_actual, fd_kernel,SEGMENTATION_FAULT, info_logger);

        cicloInstruccionesDebeEjecutarse = false;

        log_info(info_logger, "PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamaño: <%d>", pcb_actual->id, num_segmento, desplazamiento_segmento, segmento->limite );

        return -1;
    }

    segmento = list_get(pcb_actual->tablaSegmentos->segmentos, num_segmento);
    log_info(info_logger,"segemento base: %d ", segmento->base);
    int direccion_fisica = segmento->base + desplazamiento_segmento;

    log_info(info_logger, "direccion fisica = base + desplazamiento: %d", direccion_fisica);

    return direccion_fisica;
}

bool error_segmentationFault(int desplazamiento_segmento, int cantidad_bytes) {

    //if (numero_segmento >= list_size(pcb_actual->tabla_segmentos))  return true;

    segmento = list_get(pcb_actual->tablaSegmentos->segmentos, num_segmento);

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
void copiar_registrosCPU_a_los_registroPCB(registros_cpu* registro) {
    memcpy(registro->registro_AX,registroCPU_AX,4);
    memcpy(registro->registro_BX,registroCPU_BX,4);
    memcpy(registro->registro_CX,registroCPU_CX,4);
    memcpy(registro->registro_DX,registroCPU_DX,4);
    memcpy(registro->registro_EAX,registroCPU_EAX,8);
    memcpy(registro->registro_EBX,registroCPU_EBX,8);
    memcpy(registro->registro_ECX,registroCPU_ECX,8);
    memcpy(registro->registro_EDX,registroCPU_EDX,8);
    memcpy(registro->registro_RAX,registroCPU_RAX,16);
    memcpy(registro->registro_RBX,registroCPU_RBX,16);
    memcpy(registro->registro_RCX,registroCPU_RCX,16);
    memcpy(registro->registro_RDX,registroCPU_RDX,16);
}





