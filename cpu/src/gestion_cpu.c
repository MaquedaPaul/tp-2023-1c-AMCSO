//
// Created by utnso on 5/5/23.
//

#include <gestion_cpu.h>

//Instruccion a ejecutar actual
instr_t* instruccion;

bool cicloInstruccionesDebeEjecutarse = true;
instr_t* instruccion;
char* nombre_instruccion_actual;

//Registros temporales
uint32_t dir_logica_actual;
uint32_t dir_fisica_actual;


void ejecutar_SET(char* registro, char* valor){
//TODO
}

void ejecutar_MOV_IN(char* registro, int direccion_logica) {


    /*
     *
     *
            dir_logica_actual = obtener_direccion_logica();
            dir_fisica_actual = obtener_direccion_fisica(dir_logica_actual);
            int tamaño_a_leer = tamaño_a_escribir(instruccion->param1); //mando el tipo de registro

        if(es_segmetation_fault(dir_fisica_actual, dir_logica_actual, tamaño_a_leer)){
            //comunicarse con kernel para que finalice el proceso por SEG_FAULT
            return 1;
        }

        ejecutar_lectura(); //el tamaño del valor esperado va a depender del tipo de registro

        //pcb_actual->programCounter++

     */

    int cantidad_bytes = buscar_registro(registro);

    //if( buscar_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica,cantidad_bytes);
    if (!(direccion_fisica < 0)) {


    }


}



void ejecutar_MOV_OUT(char* registro, int direccion_logica) {
    int cantidad_bytes = buscar_registro(registro);

    //if( buscar_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {

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
