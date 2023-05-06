#include <mmu.h>

uint32_t tam_max_segmento;
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

int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes ) {

    num_segmento = floor(direccion_logica / cfg_cpu->TAM_MAX_SEGMENTO);
    int desplazamiento_segmento = direccion_logica % cfg_cpu->TAM_MAX_SEGMENTO;

    if (error_segmentationFault(desplazamiento_segmento, cantidad_de_bytes)) {

        copiar_registros(pcb_actual->registrosCpu);
        t_paquete* paquete = crear_paquete(SEGMENTATION_FAULT, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        enviar_paquete(paquete, cliente_servidor);
         //    log_info(logger, "PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamaño: <%d>\n", pcb_actual->id, num_segmento,    ,segmento->tamanio_segmento );  

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




void copiar_registros(registros_cpu* registro) {

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


int buscar_registro(char* registro)  {

    int bytes;

    if ((strcmp(registro, "AX") == 0)||(strcmp(registro, "BX") == 0)||(strcmp(registro, "CX") == 0)||(strcmp(registro, "DX") == 0))
        bytes = 4;

    if ((strcmp(registro, "EAX") == 0)||(strcmp(registro, "EBX") == 0)||(strcmp(registro, "ECX") == 0)||(strcmp(registro, "EDX") == 0))
        bytes = 8;

    if ((strcmp(registro, "RAX") == 0)||(strcmp(registro, "RBX") == 0)||(strcmp(registro, "RCX") == 0)||(strcmp(registro, "RDX") == 0))
        bytes = 16;

    return bytes;
}
