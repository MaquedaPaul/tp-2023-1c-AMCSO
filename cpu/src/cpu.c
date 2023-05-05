#include <cpu.h>


//Instruccion a ejecutar actual
instr_t* instruccion;

bool cicloInstruccionesDebeEjecutarse = true;
instr_t* instruccion;
char* nombre_instruccion_actual;
t_pcb *pcb_actual;
uint32_t tam_max_segmento;
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

int cliente_servidor;

t_segmento* segmento;

//Registros temporales
uint32_t dir_logica_actual;
uint32_t dir_fisica_actual;


int traducir_direccion_logica_a_fisica(int logica);


void iniciar_registros (registros_cpu* registros_PCB) {

//memcpy(registroCpu_AX,registros_PCB->AX,4);

    strcpy( registro_AX, registros_PCB->registroAX );
    strcpy( registro_BX, registros_PCB->registroBX );
    strcpy( registro_CX, registros_PCB->registroCX );
    strcpy( registro_DX, registros_PCB->registroDX );
    strcpy( registro_EAX, registros_PCB->registroEAX );
    strcpy( registro_EBX, registros_PCB->registroEBX );
    strcpy( registro_ECX, registros_PCB->registroECX );
    strcpy( registro_EDX, registros_PCB->registroEDX );
    strcpy( registro_RAX, registros_PCB->registroRAX );
    strcpy( registro_RBX, registros_PCB->registroRBX );
    strcpy( registro_RCX, registros_PCB->registroRCX );
    strcpy( registro_RDX, registros_PCB->registroRDX );

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
        /*
         *
        //MOV_OUT (Dirección Lógica, Registro)

        dir_logica_actual = obtener_direccion_logica();
        dir_fisica_actual = obtener_direccion_fisica(dir_logica_actual);

        ejecutar_escritura();
         */

        int direccion_logica = atoi(instruccion->param1);
        char* registro = instruccion->param2;
        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_OUT> - <%d> - <%s>", pcb_actual->id, direccion_logica, registro );

        ejecutar_MOV_OUT(direccion_logica, registro);
    }

    else if (strcmp(nombre_instruccion_actual, "I/O") == 0) {

        int tiempo = instruccion->param1;
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



void escribir_en_registro(char* registro, char valor[16]){
    strcpy(registro, valor);
}

char* leer_valor_de_registro(char* registro){
    char* valor_registro = NULL;

    if(strcmp(registro, "AX") == 0){
        valor_registro = registro_AX;
    }else if(strcmp(registro, "BX") == 0){
        valor_registro = registro_BX;
    }else if(strcmp(registro, "CX") == 0){
        valor_registro = registro_CX;
    }else if(strcmp(registro, "DX") == 0){
        valor_registro = registro_DX;
    }else if(strcmp(registro, "EAX") == 0){
        valor_registro = registro_EAX;
    }else if(strcmp(registro, "EBX") == 0){
        valor_registro = registro_EBX;
    }else if(strcmp(registro, "ECX") == 0){
        valor_registro = registro_ECX;
    }else if(strcmp(registro, "EDX") == 0){
        valor_registro = registro_EDX;
    }else if(strcmp(registro, "RAX") == 0){
        valor_registro = registro_RAX;
    }else if(strcmp(registro, "RBX") == 0){
        valor_registro = registro_RBX;
    }else if(strcmp(registro, "RCX") == 0){
        valor_registro = registro_RCX;
    }else if(strcmp(registro, "RDX") == 0){
        valor_registro = registro_RDX;
    }

    return valor_registro;
}

int tamaño_a_escribir(char* tipo_registro){
    if(strlen(tipo_registro) == 2){
        return 4;
    } else if(strcmp(tipo_registro, "EAX") || strcmp(tipo_registro, "EBX") || strcmp(tipo_registro, "ECX") || strcmp(tipo_registro, "EDX")){
        return 8;
    }else{
        return 16;
    }
}

void ejecutar_lectura(){
    int array_datos[] = {2, pcb_actual->id, dir_fisica_actual};

    //log_info(info_logger,  "Pido a Memoria el valor de la Direccion Fisica: <%d> para lectura", dir_fisica_actual);

    enviar_int_array(array_datos, fd_memoria, ACCESO_PEDIDO_LECTURA, info_logger);
}

void terminar_ejecucion_lectura(char* leido){ //TERMINA EL MOV_IN ESCRIBIENDO EN EL REGISTRO ASOCIADO
    //log_info(info_logger, "Recibo de memoria el valor: <%d> y lo escribo en el registro: <%s> ", leido, instruccion->param1);

    escribir_en_registro(instruccion->param1, leido);

    pcb_actual->programCounter++;
}

void terminar_ejecucion_escritura(){

}

void ejecutar_escritura(){ //ver si recibe o no el valor
    char* valor = leer_valor_de_registro(instruccion->param2);

    //int array_datos[] = {3, pcb_actual->id, dir_fisica_actual, valor}; espera el ultima valor un tipo de dato int y quiero mandar char*

    //enviar_int_array(array_datos, fd_memoria, PEDIDO_ESCRITURA, info_logger);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////












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



void ejecutar_MOV_OUT(char* nombre_archivo, int direccion_logica) {
    char* registro;
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
