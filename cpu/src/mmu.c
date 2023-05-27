#include <mmu.h>

t_pcb *pcb_actual;
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

    num_segmento = floor(direccion_logica / cfg_cpu->TAM_MAX_SEGMENTO);
    int desplazamiento_segmento = direccion_logica % cfg_cpu->TAM_MAX_SEGMENTO;

    if (error_segmentationFault(desplazamiento_segmento, cantidad_de_bytes)) {

        copiar_registros(pcb_actual->registrosCpu);
        t_paquete* paquete = crear_paquete(SEGMENTATION_FAULT, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        enviar_paquete(paquete, cliente_servidor);

       // log_info(info_logger, "PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamaño: <%d>\n", pcb_actual->id, num_segmento, desplazamiento_segmento, segmento-> tamanio_segmento );

        eliminar_paquete(paquete, info_logger);

        cicloInstruccionesDebeEjecutarse = false;

        return -1;
    }

    int direccion_fisica = segmento->base + desplazamiento_segmento;

    return direccion_fisica;
}

bool error_segmentationFault(int desplazamiento_segmento, int cantidad_bytes) {

    //if (numero_segmento >= list_size(pcb_actual->tabla_segmentos))  return true;

    segmento = list_get(pcb_actual->tablaSegmentos, num_segmento);

    return ((desplazamiento_segmento + cantidad_bytes) > segmento->limite);
}






