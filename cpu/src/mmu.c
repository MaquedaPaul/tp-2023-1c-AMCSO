#include <mmu.h>

uint32_t tam_max_segmento;
t_pcb *pcb_actual;
t_segmento* segmento;
int cliente_servidor;

char registro_AX[4];
char registro_BX[4];
char registro_CX[4];
char registro_DX[4];
char registro_EAX[8];
char registro_EBX[8];
char registro_ECX[8];
char registro_EDX[8];
char registro_RAX[16];
char registro_RBX[16];
char registro_RCX[16];
char registro_RDX[16];


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

    int num_segmento = floor(direccion_logica / tam_max_segmento);
    int desplazamiento_segmento = direccion_logica % tam_max_segmento;

    if (error_segmentationFault(num_segmento, desplazamiento_segmento, cantidad_de_bytes)) {

        copiar_registros(pcb_actual->registrosCpu);
        t_paquete* paquete = crear_paquete(SEGMENTATION_FAULT, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        enviar_paquete(paquete, cliente_servidor);
        eliminar_paquete(paquete, info_logger);
        return -1;
    }

    int direccion_fisica = segmento->base + desplazamiento_segmento;

    return direccion_fisica;
}

bool error_segmentationFault(int numero_segmento, int desplazamiento_segmento, int cantidad_bytes) {

    //if (numero_segmento >= list_size(pcb_actual->tabla_segmentos))  return true;

    segmento = list_get(pcb_actual->tablaSegmentos, numero_segmento);

    return ((desplazamiento_segmento + cantidad_bytes) > segmento->limite);
}


void copiar_registros(registros_cpu* registros_PCB) {
    strcpy( registros_PCB->registroAX , registro_AX );
    strcpy( registros_PCB->registroBX , registro_BX );
    strcpy( registros_PCB->registroCX , registro_CX );
    strcpy( registros_PCB->registroDX , registro_DX );
    strcpy( registros_PCB->registroEAX , registro_EAX );
    strcpy( registros_PCB->registroEBX , registro_EBX );
    strcpy( registros_PCB->registroECX , registro_ECX );
    strcpy( registros_PCB->registroEDX , registro_EDX );
    strcpy( registros_PCB->registroRAX , registro_RAX );
    strcpy( registros_PCB->registroRBX , registro_RBX );
    strcpy( registros_PCB->registroRCX , registro_RCX );
    strcpy( registros_PCB->registroRDX , registro_RDX );

}


int buscar_registro(char* registro)  {

    int bytes = -1;

    if ((strcmp(registro, "AX") == 0)||(strcmp(registro, "BX") == 0)||(strcmp(registro, "CX") == 0)||(strcmp(registro, "DX") == 0))
        bytes = 4;

    if ((strcmp(registro, "EAX") == 0)||(strcmp(registro, "EBX") == 0)||(strcmp(registro, "ECX") == 0)||(strcmp(registro, "EDX") == 0))
        bytes = 8;

    if ((strcmp(registro, "RAX") == 0)||(strcmp(registro, "RBX") == 0)||(strcmp(registro, "RCX") == 0)||(strcmp(registro, "RDX") == 0))
        bytes = 16;

    return bytes;
}
