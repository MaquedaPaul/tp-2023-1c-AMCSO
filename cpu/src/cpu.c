#include <cpu.h>


void iniciar_registros (registros_cpu* registro) {

memcpy(registroCPU_AX,registro->registro_AX,4);
memcpy(registroCPU_BX,registro->registro_BX,4);
memcpy(registroCPU_CX,registro->registro_CX,4);
memcpy(registroCPU_DX,registro->registro_DX,4);
memcpy(registroCPU_EAX,registro->registro_EAX,8);
memcpy(registroCPU_EBX,registro->registro_EBX,8);
memcpy(registroCPU_ECX,registro->registro_ECX,8);
memcpy(registroCPU_EDX,registro->registro_EDX,8);
memcpy(registroCPU_RAX,registro->registro_RAX,16);
memcpy(registroCPU_RBX,registro->registro_RBX,16);
memcpy(registroCPU_RCX,registro->registro_RCX,16);
memcpy(registroCPU_RDX,registro->registro_RDX,16);

}


void ciclo_instrucciones(){

// instr_t* instruccion;

    while(cicloInstruccionesDebeEjecutarse){      // && (pcb->programCounter < list_size(pcb->instr))

        instruccion = fetch();

        nombre_instruccion_actual = decode(); // decode(instruccion);

        execute();
    }  // execute(instruccion);

    cicloInstruccionesDebeEjecutarse = true;
}


instr_t* fetch(){
    return list_get(pcb_actual->instr, pcb_actual->programCounter);
}


char* decode(){

    if(strcmp(instruccion->id, "SET") == 0){

        usleep(cfg_cpu->RETARDO_INSTRUCCION * 1000); }

    return instruccion->id;
}



void execute() {
    if (strcmp(nombre_instruccion_actual , "SET") == 0) {
        char* registro = instruccion->param1;    //  list_get(instruccion->parametros, 0); si quiero hacer una lista de parametros
        char* valor = instruccion->param2;
        log_info(info_logger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%s>", pcb_actual->id, registro, valor);

        ejecutar_SET(registro, valor);
    }

    else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) {
        char* registro = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_IN> - <%s> - <%d>", pcb_actual->id, registro, direccion_logica);

        ejecutar_MOV_IN(registro, direccion_logica);
    }

    else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {

        int direccion_logica = atoi(instruccion->param1);
        char* registro = instruccion->param2;
        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_OUT> - <%d> - <%s>", pcb_actual->id, direccion_logica, registro );

        ejecutar_MOV_OUT(direccion_logica,registro);
    }

    else if (strcmp(nombre_instruccion_actual, "I/O") == 0) {

        int tiempo = atoi(instruccion->param1);
        log_info(info_logger, "PID: <%d> - Ejecutando <I/O> - <%d>", pcb_actual->id, tiempo);
        ejecutar_IO(tiempo);
    }

    else if (strcmp(nombre_instruccion_actual, "F_OPEN") == 0) {
        char* nombre_archivo = instruccion->param1;
        log_info(info_logger, "PID: <%d> - Ejecutando <F_OPEN> - <%s>", pcb_actual->id, nombre_archivo);

        ejecutar_F_OPEN(nombre_archivo);
    }
    else if (strcmp(nombre_instruccion_actual, "F_CLOSE") == 0) {
        char* nombre_archivo = instruccion->param1;
        log_info(info_logger, "PID: <%d> - Ejecutando <F_CLOSE> - <%s>", pcb_actual->id, nombre_archivo);

        ejecutar_F_CLOSE(nombre_archivo);
    }
    else if (strcmp(nombre_instruccion_actual, "F_SEEK") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_SEEK> - <%s> - <%d>", pcb_actual->id, nombre_archivo, direccion_logica );

        ejecutar_F_SEEK(nombre_archivo, direccion_logica);
    }
    else if (strcmp(nombre_instruccion_actual, "F_READ") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        int cantidad_de_bytes = atoi(instruccion->param3);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_READ> - <%s> - <%d> - <%d>", pcb_actual->id, nombre_archivo, direccion_logica, cantidad_de_bytes );

        ejecutar_F_READ(nombre_archivo, direccion_logica, cantidad_de_bytes);
    }
    else if (strcmp(nombre_instruccion_actual, "F_WRITE") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        int cantidad_de_bytes = atoi(instruccion->param3);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_WRITE> - <%s> - <%d> - <%d>", pcb_actual->id, nombre_archivo, direccion_logica, cantidad_de_bytes );

        ejecutar_F_WRITE(nombre_archivo, direccion_logica, cantidad_de_bytes);
    }
    else if (strcmp(nombre_instruccion_actual, "F_TRUNCATE") == 0) {
        char* nombre_archivo = instruccion->param1;
        int tamanio = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_TRUNCATE> - <%s> - <%d>", pcb_actual->id, nombre_archivo, tamanio );

        ejecutar_F_TRUNCATE(nombre_archivo, tamanio);
    }
    else if (strcmp(nombre_instruccion_actual, "WAIT") == 0) {
        char* recurso = instruccion->param1;
        log_info(info_logger, "PID: <%d> - Ejecutando <WAIT> - <%s>", pcb_actual->id, recurso);

        ejecutar_WAIT(recurso);
    }
    else if (strcmp(nombre_instruccion_actual, "SIGNAL") == 0) {
        char* recurso = instruccion->param1;
        log_info(info_logger, "PID: <%d> - Ejecutando <SIGNAL> - <%s>", pcb_actual->id, recurso);

        ejecutar_SIGNAL(recurso);
    }
    else if (strcmp(nombre_instruccion_actual, "CREATE_SEGMENT") == 0) {
        int id_del_segmento = atoi(instruccion->param1);
        int tamanio = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <CREATE_SEGMENT> - <%d> - <%d>", pcb_actual->id, id_del_segmento, tamanio );

        ejecutar_CREATE_SEGMENT(id_del_segmento, tamanio);
    }
    else if (strcmp(nombre_instruccion_actual, "DELETE_SEGMENT") == 0) {
        int id_del_segmento = atoi(instruccion->param1);
        log_info(info_logger, "PID: <%d> - Ejecutando: <DELETE_SEGMENT> - <%d>", pcb_actual->id, id_del_segmento );

        ejecutar_DELETE_SEGMENT(id_del_segmento);
    }
    else if (strcmp(nombre_instruccion_actual, "YIELD") == 0) {
        log_info(info_logger, "PID: <%d> - Ejecutando: <YIELD>", pcb_actual->id );

        ejecutar_YIELD();
    }

    else if (strcmp(nombre_instruccion_actual, "EXIT") == 0) {
        log_info(info_logger, "PID: <%d> - Ejecutando: <EXIT> -", pcb_actual->id);

        ejecutar_EXIT();
    } else {
        log_info(info_logger, "PID: <%d> - NO EXISTE LA INSTRUCCION", pcb_actual->id); // log_error() ?

    }
}



void terminar_ejecucion_lectura(char* leido){ //TERMINA EL MOV_IN ESCRIBIENDO EN EL REGISTRO ASOCIADO
    //log_info(info_logger, "Recibo de memoria el valor: <%d> y lo escribo en el registro: <%s> ", leido, instruccion->param1);

   // escribir_en_registro(instruccion->param1, leido);

    //pcb_actual->programCounter++;
}

void terminar_ejecucion_escritura(){

}


void escribir_valor_en_memoria(int direccion_fisica, char valor[]) {


    log_info(info_logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento:< %d > - Dirección Fisica: <%d> - Valor: <%s>", pcb_actual->id, num_segmento, direccion_fisica, valor);

    t_paquete* paquete = crear_paquete(ACCESO_PEDIDO_ESCRITURA,info_logger);
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));

    int largo_nombre = strlen(valor) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, valor, largo_nombre);

    enviar_paquete(paquete, fd_memoria);

    eliminar_paquete(paquete,info_logger);

    // recv(, ,    , MSG_WAITALL);


   // if (respuesta < 0) {  log_info(logger, "No se pudo escribir el valor en memoria");} // log_error 


}