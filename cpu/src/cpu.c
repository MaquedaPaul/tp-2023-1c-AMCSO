#include <cpu.h>
t_instr* instruccion;
char* nombre_instruccion_actual;


void copiar_registroPCB_a_los_registrosCPU (registros_cpu* registro) {

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


void ciclo_de_instruccion(){

    cicloInstruccionesDebeEjecutarse = true;
    
    while(cicloInstruccionesDebeEjecutarse){    //   && (pcb->programCounter < list_size(pcb->instr)){

        instruccion = fetch();

        nombre_instruccion_actual = decode();

        execute();
    }  

    
}


t_instr* fetch(){
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
        log_debug(debug_logger, "el valor del registro AX: %s", registroCPU_AX);
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
        
        ejecutar_MOV_OUT(direccion_logica, registro);
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

