#include <gestion_cpu.h>

int fd_cpu;
char* ip_cpu;
char* puerto_cpu;
int fd_memoria;


//Instruccion a ejecutar actual

bool cicloInstruccionesDebeEjecutarse = true;
instr_t* instruccion;
char* nombre_instruccion_actual;


void ejecutar_SET(char* registro, char* valor){

    cambiar_valor_registro(registro,valor);
    pcb_actual->programCounter++;

}

void ejecutar_MOV_IN(char* registro, int direccion_logica) {

    int cantidad_bytes = buscar_registro(registro);

    //if( buscar_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica,cantidad_bytes);
    if (!(direccion_fisica < 0)) {

           char* valor = leer_valor_de_memoria(direccion_fisica);

           cambiar_valor_registro(registro,valor);

           pcb_actual->programCounter++;
    }

}


void ejecutar_MOV_OUT(int direccion_logica,char* registro ) {
    int cantidad_bytes = buscar_registro(registro);

    //if( buscar_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {
        char valor[17];
        obtener_valor_registro(registro, valor);
        pcb_actual->programCounter++;
    }


}


void ejecutar_IO(int tiempo) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(IO_BLOCK,info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    agregar_a_paquete(paquete, &tiempo, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete,info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}





void ejecutar_F_OPEN(char* nombre_archivo) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_OPEN,info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_F_CLOSE(char* nombre_archivo) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_CLOSE,info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}

void ejecutar_F_SEEK(char* nombre_archivo, int posicion) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_SEEK, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &posicion, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_F_READ(char* nombre_archivo, int direccion_logica, int cantidad_bytes) {
    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {

        copiar_registros(pcb_actual->registrosCpu);
        pcb_actual->programCounter++;
        t_paquete* paquete = crear_paquete(F_READ, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        int largo_nombre = strlen(nombre_archivo) + 1;
        agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
        agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
        agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
        agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
        enviar_paquete(paquete, cliente_servidor);
        eliminar_paquete(paquete, info_logger);
    }

    cicloInstruccionesDebeEjecutarse = false;

}



void ejecutar_F_WRITE(char* nombre_archivo, int direccion_logica, int cantidad_bytes) {
    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {

        copiar_registros(pcb_actual->registrosCpu);
        pcb_actual->programCounter++;
        t_paquete* paquete = crear_paquete(F_WRITE, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        int largo_nombre = strlen(nombre_archivo) + 1;
        agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
        agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
        agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
        agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
        enviar_paquete(paquete, cliente_servidor);
        eliminar_paquete(paquete, info_logger);
    }

    cicloInstruccionesDebeEjecutarse = false;

}


void ejecutar_F_TRUNCATE(char* nombre_archivo, int tamanio) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_TRUNCATE, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &tamanio, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}



void ejecutar_WAIT(char* nombre_recurso) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(WAIT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_SIGNAL(char* nombre_recurso) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(SIGNAL, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_CREATE_SEGMENT(int id_del_segmento, int tamanio) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(CREATE_SEGMENT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    agregar_a_paquete(paquete, &id_del_segmento, sizeof(int));
    agregar_a_paquete(paquete, &tamanio, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}

void ejecutar_DELETE_SEGMENT(int id_del_segmento) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(DELETE_SEGMENT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    agregar_a_paquete(paquete, &id_del_segmento, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}



void ejecutar_YIELD() {
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(YIELD, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_EXIT() {
    copiar_registros(pcb_actual->registrosCpu);
    t_paquete* paquete = crear_paquete(EXIT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void cambiar_valor_registro(char* registro,char* valor) {

    if (strcmp(registro, "AX") == 0)
        memcpy( registroCPU_AX, valor,4);

    if (strcmp(registro, "BX") == 0)
        memcpy( registroCPU_BX, valor,4);

    if (strcmp(registro, "CX") == 0)
        memcpy( registroCPU_CX, valor,4);

    if (strcmp(registro, "DX") == 0)
        memcpy( registroCPU_DX, valor,4);

    if (strcmp(registro, "EAX") == 0)
        memcpy( registroCPU_EAX, valor,8);

    if (strcmp(registro, "EBX") == 0)
        memcpy( registroCPU_EBX, valor,8);

    if (strcmp(registro, "ECX") == 0)
        memcpy( registroCPU_ECX, valor,8);

    if (strcmp(registro, "EDX") == 0)
        memcpy( registroCPU_EDX, valor,8);

    if (strcmp(registro, "RAX") == 0)
        memcpy( registroCPU_RAX, valor,16);

    if (strcmp(registro, "RBX") == 0)
        memcpy( registroCPU_RBX, valor,16);

    if (strcmp(registro, "RCX") == 0)
        memcpy( registroCPU_RCX, valor,16);

    if (strcmp(registro, "RDX") == 0)
        memcpy( registroCPU_RDX, valor,16);

}


int buscar_registro(char* registro)  { //pondria un otro nombre a la funcion  //   int tamaño_a_escribir(char* tipo_registro)


    int bytes;

    if ((strcmp(registro, "AX") == 0)||(strcmp(registro, "BX") == 0)||(strcmp(registro, "CX") == 0)||(strcmp(registro, "DX") == 0))
        bytes = 4;

    if ((strcmp(registro, "EAX") == 0)||(strcmp(registro, "EBX") == 0)||(strcmp(registro, "ECX") == 0)||(strcmp(registro, "EDX") == 0))
        bytes = 8;

    if ((strcmp(registro, "RAX") == 0)||(strcmp(registro, "RBX") == 0)||(strcmp(registro, "RCX") == 0)||(strcmp(registro, "RDX") == 0))
        bytes = 16;

    return bytes;
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

void obtener_valor_registro(char* registro,char valor[]) {


    if (strcmp(registro, "AX") == 0)
        memcpy( valor, registroCPU_AX,4);

    if (strcmp(registro, "BX") == 0)
        memcpy( valor,registroCPU_BX,4);

    if (strcmp(registro, "CX") == 0)
        memcpy( valor, registroCPU_CX,4);

    if (strcmp(registro, "DX") == 0)
        memcpy( valor, registroCPU_DX,4);

    if (strcmp(registro, "EAX") == 0)
        memcpy( valor, registroCPU_EAX,8);

    if (strcmp(registro, "EBX") == 0)
        memcpy( valor, registroCPU_EBX,8);

    if (strcmp(registro, "ECX") == 0)
        memcpy( valor, registroCPU_ECX,8);

    if (strcmp(registro, "EDX") == 0)
        memcpy( valor, registroCPU_EDX,8);

    if (strcmp(registro, "RAX") == 0)
        memcpy( valor, registroCPU_RAX,16);

    if (strcmp(registro, "RBX") == 0)
        memcpy( valor, registroCPU_RBX,16);

    if (strcmp(registro, "RCX") == 0)
        memcpy( valor, registroCPU_RCX,16);

    if (strcmp(registro, "RDX") == 0)
        memcpy( valor, registroCPU_RDX,16);
}


char* leer_valor_de_memoria(int direccion_fisica) {
    char* valor;
    t_paquete* paquete = crear_paquete(ACCESO_PEDIDO_LECTURA, info_logger);
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    enviar_paquete(paquete, fd_memoria);


    log_info(info_logger, "PID: <%d> - Acción: <LEER> - Segmento:< %d > - Dirección Fisica: <%d> - Valor: <%s>", pcb_actual->id, num_segmento, direccion_fisica, valor);

    eliminar_paquete(paquete, info_logger);

    // recv(,  , MSG_WAITALL);



    return valor;
}
