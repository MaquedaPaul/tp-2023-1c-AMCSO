#include <cpu.h>


//Instruccion a ejecutar actual
instr_t* instruccion;


//Registros temporales 
char registroAX[4];
char registroBX[4];
char registroCX[4];
char registroDX[4];
char registroEAX[8];
char registroEBX[8];
char registroECX[8];
char registroEDX[8];
char registroRAX[16];
char registroRCX[16];
char registroRBX[16];
char registroRDX[16];


uint32_t dir_logica_actual;
uint32_t dir_fisica_actual;


int traducir_direccion_logica_a_fisica(int logica);

instr_t* fetch(pcb* pcbActual){
    return list_get(pcbActual->instr, pcbActual->programCounter);
}

//DECODE
char* decode(instr_t* instruccion){
    return instruccion->id;     //nombre de la instruccion
}

//EXECUTE
int execute(char* nombre_instruccion_actual) {

    if (strcmp(nombre_instruccion_actual, "SET") == 0) {
        //SET (Registro, Valor) set ax/eax/rax 5

        escribir_en_registro(instruccion->param1, instruccion->param2);

            //pcb_actual->programCounter++
                                             
        } else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) { // Simular retardo
            //MOV_IN (Registro, Dirección Lógica)

            dir_logica_actual = obtener_direccion_logica();
            dir_fisica_actual = obtener_direccion_fisica(dir_logica_actual);
            int tamaño_a_leer = tamaño_a_escribir(instruccion->param1); //mando el tipo de registro

        if(es_segmetation_fault(dir_fisica_actual, dir_logica_actual, tamaño_a_leer)){
            //comunicarse con kernel para que finalice el proceso por SEG_FAULT
            return 1;
        }

        ejecutar_lectura(); //el tamaño del valor esperado va a depender del tipo de registro

        //pcb_actual->programCounter++


    } else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {
      //MOV_OUT (Dirección Lógica, Registro)

      dir_logica_actual = obtener_direccion_logica();
      dir_fisica_actual= obtener_direccion_fisica(dir_logica_actual);

      ejecutar_escritura();

    } else if (strcmp(nombre_instruccion_actual, "F_OPEN ") == 0) {
      //F_OPEN (Nombre Archivo)


    } else if (strcmp(nombre_instruccion_actual, "F_CLOSE") == 0) {
      //F_CLOSE (Nombre Archivo)

    } else if (strcmp(nombre_instruccion_actual, "F_SEEK") == 0) {
      //F_SEEK (Nombre Archivo, Posición)

    } else if (strcmp(nombre_instruccion_actual, "F_READ") == 0) {
      //F_READ (Nombre Archivo, Dirección Lógica, Cantidad de Bytes)

    } else if (strcmp(nombre_instruccion_actual, "F_WRITE") == 0) {
      //F_WRITE (Nombre Archivo, Dirección Lógica, Cantidad de Bytes)

    } else if (strcmp(nombre_instruccion_actual, "F_TRUNCATE") == 0) {
      //F_TRUNCATE (Nombre Archivo, Tamaño)

    } else if (strcmp(nombre_instruccion_actual, "WAIT") == 0) {
      //WAIT (Recurso)

    } else if (strcmp(nombre_instruccion_actual, "SIGNAL") == 0) {
      //SIGNAL (Recurso)

    } else if (strcmp(nombre_instruccion_actual, "CREATE_SEGMENTC") == 0) {
      //CREATE_SEGMENT (Id del Segmento, Tamaño)

    } else if (strcmp(nombre_instruccion_actual, "DELETE_SEGMENT") == 0) {
        //DELETE_SEGMENT (Id del Segmento)

    } else if (strcmp(nombre_instruccion_actual, "I/O") == 0) {
        //I/O (Tiempo)

    } else if (strcmp(nombre_instruccion_actual, "YIELD") == 0) {
        //YIELD

    } else if (strcmp(nombre_instruccion_actual, "EXIT") == 0) {
      //EXIT
    }

}


void escribir_en_registro(char* registro, char valor[16]){
    strcpy(registro, valor);
}

char* leer_valor_de_registro(char* registro){
    char* valor_registro = NULL;

    if(strcmp(registro, "AX") == 0){
        valor_registro = registroAX;
    }else if(strcmp(registro, "BX") == 0){
        valor_registro = registroBX;
    }else if(strcmp(registro, "CX") == 0){
        valor_registro = registroCX;
    }else if(strcmp(registro, "DX") == 0){
        valor_registro = registroDX;
    }else if(strcmp(registro, "EAX") == 0){
        valor_registro = registroEAX;
    }else if(strcmp(registro, "EBX") == 0){
        valor_registro = registroEBX;
    }else if(strcmp(registro, "ECX") == 0){
        valor_registro = registroECX;
    }else if(strcmp(registro, "EDX") == 0){
        valor_registro = registroEDX;
    }else if(strcmp(registro, "RAX") == 0){
        valor_registro = registroRAX;
    }else if(strcmp(registro, "RBX") == 0){
        valor_registro = registroRBX;
    }else if(strcmp(registro, "RCX") == 0){
        valor_registro = registroRCX;
    }else if(strcmp(registro, "RDX") == 0){
        valor_registro = registroRDX;
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

    enviar_int_array(array_datos, fd_memoria, PEDIDO_LECTURA, info_logger);
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



