#include <gestion_cpu.h>

void ejecutar_SET(char* registro, char* valor){
    cambiar_valor_del_registroCPU(registro,valor);
    pcb_actual->programCounter++;

}

void ejecutar_MOV_IN(char* registro, int direccion_logica) {
    int cantidad_bytes = calcular_bytes_segun_registro(registro);

    //if( calcular_bytes_segun_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica,cantidad_bytes);

    if (!(direccion_fisica < 0)) {
           char* valor = leer_valor_de_memoria(direccion_fisica, cantidad_bytes,conexion_actual);
           cambiar_valor_del_registroCPU(registro,valor);
           pcb_actual->programCounter++;
    }
}


void ejecutar_MOV_OUT(int direccion_logica, char* registro ) {
    int cantidad_bytes = calcular_bytes_segun_registro(registro);
    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);

    if (!(direccion_fisica < 0)) {

        escribir_valor_en_memoria(direccion_fisica,cantidad_bytes, registro);

        pcb_actual->programCounter++;
    }
}


void ejecutar_IO(int tiempo) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(IO_BLOCK,info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    agregar_a_paquete(paquete, &tiempo, sizeof(int));
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete,info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_F_OPEN(char* nombre_archivo) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_OPEN, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_F_CLOSE(char* nombre_archivo) {

    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_CLOSE,info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_F_SEEK(char* nombre_archivo, int posicion) {

    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_SEEK, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &posicion, sizeof(int));
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);

    //eliminar_PCB(pcb_actual);
    recibirPCB();
}


void ejecutar_F_READ(char* nombre_archivo, int direccion_logica, int cantidad_bytes) {
    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {

        copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
        pcb_actual->programCounter++;
        t_paquete* paquete = crear_paquete(F_READ, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        int largo_nombre = strlen(nombre_archivo) + 1;
        agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
        agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
        agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
        agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
        enviar_paquete(paquete, fd_kernel);
        eliminar_paquete(paquete, info_logger);
    }
    cicloInstruccionesDebeEjecutarse = false;
}



void ejecutar_F_WRITE(char* nombre_archivo, int direccion_logica, int cantidad_bytes) {
    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {

        copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
        pcb_actual->programCounter++;
        t_paquete* paquete = crear_paquete(F_WRITE, info_logger);
        agregar_PCB_a_paquete(paquete, pcb_actual);
        int largo_nombre = strlen(nombre_archivo) + 1;
        agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
        agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
        agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
        agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
        enviar_paquete(paquete, fd_kernel);
        eliminar_paquete(paquete, info_logger);
    }
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_F_TRUNCATE(char* nombre_archivo, int tamanio) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_TRUNCATE, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &tamanio, sizeof(int));
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}



void ejecutar_WAIT(char* nombre_recurso) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(WAIT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_SIGNAL(char* nombre_recurso) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(SIGNAL, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    int largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_CREATE_SEGMENT(int id_del_segmento, int tamanio) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(CREATE_SEGMENT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    agregar_a_paquete(paquete, &id_del_segmento, sizeof(int));
    agregar_a_paquete(paquete, &tamanio, sizeof(int));
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    //eliminar_PCB(pcb_actual);
    recibirPCB();

}

void ejecutar_DELETE_SEGMENT(int id_del_segmento) {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(DELETE_SEGMENT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    agregar_a_paquete(paquete, &id_del_segmento, sizeof(int));
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    //eliminar_PCB(pcb_actual);
    recibirPCB();
}


void ejecutar_YIELD() {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(YIELD, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void ejecutar_EXIT() {
    copiar_registrosCPU_a_los_registroPCB(pcb_actual->registrosCpu);
    t_paquete* paquete = crear_paquete(EXIT, info_logger);
    agregar_PCB_a_paquete(paquete, pcb_actual);
    enviar_paquete(paquete, fd_kernel);
    eliminar_paquete(paquete, info_logger);
    cicloInstruccionesDebeEjecutarse = false;
}


void cambiar_valor_del_registroCPU(char* registro, char* valor) {
    if (strcmp(registro, "AX") == 0)
        memcpy(registroCPU_AX, valor, 4);

    if (strcmp(registro, "BX") == 0)
        memcpy(registroCPU_BX, valor, 4);

    if (strcmp(registro, "CX") == 0)
        memcpy(registroCPU_CX, valor,4);

    if (strcmp(registro, "DX") == 0)
        memcpy(registroCPU_DX, valor, 4);

    if (strcmp(registro, "EAX") == 0)
        memcpy(registroCPU_EAX, valor, 8);

    if (strcmp(registro, "EBX") == 0)
        memcpy(registroCPU_EBX, valor, 8);

    if (strcmp(registro, "ECX") == 0)
        memcpy(registroCPU_ECX, valor, 8);

    if (strcmp(registro, "EDX") == 0)
        memcpy(registroCPU_EDX, valor, 8);

    if (strcmp(registro, "RAX") == 0)
        memcpy(registroCPU_RAX, valor, 16);

    if (strcmp(registro, "RBX") == 0)
        memcpy(registroCPU_RBX, valor, 16);

    if (strcmp(registro, "RCX") == 0)
        memcpy(registroCPU_RCX, valor, 16);

    if (strcmp(registro, "RDX") == 0)
        memcpy(registroCPU_RDX, valor, 16);
}


int calcular_bytes_segun_registro(char* registro)  { 
    int bytes;

    if ((strcmp(registro, "AX") == 0)||(strcmp(registro, "BX") == 0)||(strcmp(registro, "CX") == 0)||(strcmp(registro, "DX") == 0))
        bytes = 4;
    if ((strcmp(registro, "EAX") == 0)||(strcmp(registro, "EBX") == 0)||(strcmp(registro, "ECX") == 0)||(strcmp(registro, "EDX") == 0))
        bytes = 8;
    if ((strcmp(registro, "RAX") == 0)||(strcmp(registro, "RBX") == 0)||(strcmp(registro, "RCX") == 0)||(strcmp(registro, "RDX") == 0))
        bytes = 16;

    return bytes;
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


char* obtener_valor_registroCPU(char* registro) { 
  
    if (strcmp(registro, "AX") == 0){  
       
         char* valor = (char *) malloc (4 + 1);
         strcpy (valor,"pppp");
         memcpy(valor,registroCPU_AX,4);

        return valor;
    }
    if (strcmp(registro, "BX") == 0) { 

         char* valor = (char *) malloc (4 + 1);
         strcpy (valor,"pppp");
        memcpy(valor,registroCPU_BX, 4);

        return valor;
     }
    if (strcmp(registro, "CX") == 0) { 

        char* valor = (char *) malloc (4 + 1);
        strcpy (valor,"pppp");
        memcpy(valor, registroCPU_CX, 4);

        return valor;
     }
    if (strcmp(registro, "DX") == 0) { 
        char* valor = (char *) malloc (4 + 1);
        strcpy (valor,"pppp");
        memcpy(valor, registroCPU_DX, 4);

        return valor;
     }
    if (strcmp(registro, "EAX") == 0) { 
        char* valor = (char *) malloc (8 + 1);
        strcpy (valor,"pppppppp");
        memcpy(valor, registroCPU_EAX, 8);

        return valor;
     }
    if (strcmp(registro, "EBX") == 0) { 
        char* valor = (char *) malloc (8 + 1);
        strcpy (valor,"pppppppp");
        memcpy(valor, registroCPU_EBX, 8);

        return valor;
     }

    if (strcmp(registro, "ECX") == 0) { 
        char* valor = (char *) malloc (8 + 1);
        strcpy (valor,"pppppppp");
        memcpy(valor, registroCPU_ECX, 8);

        return valor;
     }
    if (strcmp(registro, "EDX") == 0) { 
        char* valor = (char *) malloc (8 + 1);
        strcpy (valor,"pppppppp");
        memcpy(valor, registroCPU_EDX, 8);

        return valor;
     }
    if (strcmp(registro, "RAX") == 0) { 
        char* valor = (char *) malloc (16 + 1);
        strcpy (valor,"pppppppppppppppp");
        memcpy(valor, registroCPU_RAX, 16);

        return valor;
     }
    if (strcmp(registro, "RBX") == 0) { 
        char* valor = (char *) malloc (16 + 1);
        strcpy (valor,"pppppppppppppppp");
        memcpy(valor, registroCPU_RBX, 16);

        return valor;
     }
    if (strcmp(registro, "RCX") == 0) {
        char* valor = (char *) malloc (16 + 1);
        strcpy (valor,"pppppppppppppppp");
        memcpy(valor, registroCPU_RCX, 16);

        return valor;
      }
    if (strcmp(registro, "RDX") == 0) { 
        char* valor = (char *) malloc (16 + 1);
        strcpy (valor,"pppppppppppppppp");
        memcpy(valor, registroCPU_RDX, 16);

        return valor;
     }
}


char* leer_valor_de_memoria(int direccion_fisica, int cantidad_bytes, int conexion) {

    t_paquete* paquete = crear_paquete(ACCESO_PEDIDO_LECTURA, info_logger);
    
    int catidad_enteros = 3;
    agregar_a_paquete(paquete, &catidad_enteros, sizeof(int));
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
    agregar_a_paquete(paquete, &(pcb_actual->id), sizeof(int));

    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete, info_logger);

    char* valor = recibir_valor_de_memoria(cantidad_bytes);

    log_info(info_logger, "PID: <%d> - Acción: <LEER> - Segmento:< %d > - Dirección Fisica: <%d> - Valor: <%s>", pcb_actual->id, num_segmento, direccion_fisica, valor);
   
    return valor;
 }


/*

char* leer_valor_de_memoria(int direccion_fisica, int cantidad_bytes) {
    char* valor;
    //t_paquete* paquete = crear_paquete(ACCESO_PEDIDO_LECTURA, info_logger);
    //agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    //enviar_paquete(paquete, fd_memoria);

    t_list* listaInts = list_create();

    int *dir_fisica = &direccion_fisica;
    int *bytes = &cantidad_bytes;
    int *pid =  &pcb_actual->id;

    list_add(listaInts, dir_fisica);
    list_add(listaInts, bytes);
    list_add(listaInts, pid); //pcb_actual tiene q ser global
    enviarListaUint32_t(listaInts, fd_memoria, info_logger, ACCESO_PEDIDO_LECTURA);

    log_info(info_logger, "PID: <%d> - Acción: <LEER> - Segmento:< %d > - Dirección Fisica: <%d> - Valor: <%s>", pcb_actual->id, num_segmento, direccion_fisica, valor);

    //eliminar_paquete(paquete, info_logger);

    strcpy(valor, recibir_valor_de_memoria());

    return valor;
}
*/


void agregar_registroCPU_a_paquete(char* registro, t_paquete* paquete) {
    if (strcmp(registro, "AX") == 0)
    agregar_a_paquete(paquete, registroCPU_AX, 4);

    if (strcmp(registro, "BX") == 0)
    agregar_a_paquete(paquete, registroCPU_BX, 4);

    if (strcmp(registro, "CX") == 0)
    agregar_a_paquete(paquete, registroCPU_CX, 4);

    if (strcmp(registro, "DX") == 0)
    agregar_a_paquete(paquete, registroCPU_DX, 4);

    if (strcmp(registro, "EAX") == 0)
    agregar_a_paquete(paquete, registroCPU_EAX, 8);

    if (strcmp(registro, "EBX") == 0)
    agregar_a_paquete(paquete, registroCPU_EBX, 8);

    if (strcmp(registro, "ECX") == 0)
    agregar_a_paquete(paquete, registroCPU_ECX, 8);

    if (strcmp(registro, "EDX") == 0)
    agregar_a_paquete(paquete, registroCPU_EDX, 8);

    if (strcmp(registro, "RAX") == 0)
    agregar_a_paquete(paquete, registroCPU_EAX, 16);

    if (strcmp(registro, "RBX") == 0)
    agregar_a_paquete(paquete, registroCPU_EBX, 16);

    if (strcmp(registro, "RCX") == 0)
    agregar_a_paquete(paquete, registroCPU_ECX, 16);

    if (strcmp(registro, "RDX") == 0)
    agregar_a_paquete(paquete, registroCPU_EDX, 16);
}

char*  recibir_confirmacion_de_escritura()  {

        char* valor = (char *) malloc (2 + 1);
        int cod_op = recibir_operacion(fd_memoria);

		switch (cod_op) {
		case ESCRITURA_REALIZADA:
             recibirOrden(fd_memoria);
             valor= "OK";
			 break;
        }
        return valor;
}




void escribir_valor_en_memoria(int direccion_fisica, int cantidad_bytes, char* registro) {

    t_paquete* paquete = crear_paquete(ACCESO_PEDIDO_ESCRITURA, info_logger);
    
    int catidad_enteros = 3;
    agregar_a_paquete(paquete, &catidad_enteros, sizeof(int));
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
    agregar_a_paquete(paquete, &(pcb_actual->id), sizeof(int));

    agregar_registroCPU_a_paquete(registro , paquete );

    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete, info_logger);

    char* valor1  = obtener_valor_registroCPU(registro);
    char* valor2 = recibir_confirmacion_de_escritura() ;

    if (strcmp(valor2, "OK") == 0) {
    log_info(info_logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento:< %d > - Dirección Fisica: <%d> - Valor: <%s>", pcb_actual->id, num_segmento, direccion_fisica, valor1);
    }
 }


/*


void escribir_valor_en_memoria(int direccion_fisica, char valor[]) {

    log_info(info_logger, "PID: <%d> - Acción: <ESCRIBIR> - Segmento:< %d > - Dirección Fisica: <%d> - Valor: <%s>", pcb_actual->id, num_segmento, direccion_fisica, valor); //donde esta declarado num_segmento??

    //t_paquete* paquete = crear_paquete(ACCESO_PEDIDO_ESCRITURA, info_logger);
    //agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));

    //int largo_nombre = strlen(valor) + 1;
    //agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    //agregar_a_paquete(paquete, valor, largo_nombre);
    //enviar_paquete(paquete, fd_memoria);
    //eliminar_paquete(paquete,info_logger);
    //eliminar_paquete(paquete,info_logger);


    t_list* listaInts = list_create();

    int *dir_fisica = &direccion_fisica;
    int *pid =  &pcb_actual->id;
    t_datos* valor_a_escribir;

    valor_a_escribir->tamanio = strlen(valor) + 1;
    strcpy(valor_a_escribir->datos, valor); //con con el /0

    list_add(listaInts, dir_fisica);
    list_add(listaInts, pid);

    enviarListaIntsYDatos(listaInts, valor_a_escribir, fd_memoria, info_logger, ACCESO_PEDIDO_ESCRITURA);

    esperar_orden();

}
*/




char*  recibir_valor_de_memoria( int cantidad_bytes){

        char* valor;
        int cod_op = recibir_operacion(fd_memoria);

		switch (cod_op) {
		case LECTURA_REALIZADA:

            valor = recibir_paquete_con_cadena(fd_memoria);
			break;
        }
    return valor;
}



char* recibir_paquete_con_cadena(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void * buffer = recibir_buffer(&size, socket_cliente);

        int tamanio;
        memcpy(&tamanio, buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento+=sizeof(uint32_t);

        char* valor = malloc(tamanio+1); 

        if(tamanio == 4)
            strcpy (valor,"0000");
        if(tamanio == 8)
            strcpy (valor,"00000000");
        if(tamanio == 16)        
		    strcpy (valor,"0000000000000000");

		memcpy(valor, buffer+desplazamiento, tamanio); 
		desplazamiento+=tamanio;
        
        free(buffer);

    return valor;
}




/*


char*  recibir_valor_de_memoria()  {
char* valor;
int fd_memoria;

int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case LECTURA_REALIZADA:
            int tamanio;
			valor = (char*) recibirDatos(fd_memoria, tamanio);
			break;
}
	return valor;
}
*/


void  recibirPCB()  {

        int cod_op = recibir_operacion(fd_kernel);

		switch (cod_op) {
		case PCB:
             pcb_actual = recibir_pcb(fd_kernel) ;
			 break;
        }
}