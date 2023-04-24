#include <cpu.h>

//Instruccion a ejecutar
instr_t* instruccion;


//Registros temporales 
uint32_t registroAX;
uint32_t registroBX;
uint32_t registroCX;
uint32_t registroDX;
uint64_t registroEAX;
uint64_t registroEBX;
uint64_t registroECX;
uint64_t registroEDX;
char registroRAX[16];
char registroRCX[16];
char registroRBX[16];
char registroRDX[16];


uint32_t dir_logica_actual;


instr_t* fetch(pcb* pcbActual){
    return list_get(pcbActual->instr, pcbActual->programCounter);
}

//DECODE
char* decode(instr_t* instruccion){
    return instruccion->id;     //nombre de la instruccion
}

//EXECUTE
void execute(char* nombre_instruccion_actual) {

    if (strcmp(nombre_instruccion_actual, "SET") == 0) {
        //SET (Registro, Valor) set ax/eax/rax 5
        int valor = atoi(instruccion->param2);

        if (strlen(instruccion->param2) == 4) { // o (strcmp(instruccion-> param1, "AX") == 0)
            escribir_en_registro_de_4_bytes(instruccion->param1, valor);
        } else if (strlen(instruccion->param2) == 8) {
            escribir_en_registro_de_8_bytes(instruccion->param1, valor);
        } else if (strlen(instruccion->param2) == 16) {
            escribir_en_registro_de_16_bytes(instruccion->param1,
                                             instruccion->param2); //ver como le paso el valor de 16 bytes
        } else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) { // Simular retardo
            //MOV_IN (Registro, Dirección Lógica)

            int direccionLogica = obtener_direccion_logica();

            //direccion_fisica = traducir_direccion_logica_a_fisica(direccionLogica);?
            //valor = leer_de_memoria(direccion_fisica);

            if (strlen(valor) == 4) { //si viene en formato char* el valor guardado en memoria
                escribir_en_registro_de_4_bytes(instruccion->param1, valor);
            } else if (strlen(valor) == 8) {
                escribir_en_registro_de_8_bytes(instruccion->param1, valor);
            } else if (strlen(valor) == 16) {
                escribir_en_registro_de_16_bytes(instruccion->param1, valor);
            }

        } else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {
            //MOV_OUT (Dirección Lógica, Registro)

            dir_logica_actual = obtener_direccion_logica();
            //traducir_direccion_logica_a_fisica(direccionLogica);?


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
}

void escribir_en_registro_de_4_bytes(char* registro, uint32_t valor){
    if(strcmp(registro, "AX") == 0){
        registroAX = valor;
    }else if(strcmp(registro, "BX") == 0){
        registroBX = valor;
    }else if(strcmp(registro, "CX") == 0){
        registroCX = valor;
    }else if(strcmp(registro, "DX") == 0){
        registroDX = valor;
    }
}

void escribir_en_registro_de_8_bytes(char* registro, uint64_t valor){
    if(strcmp(registro, "EAX") == 0){
        registroEAX = valor;
    }else if(strcmp(registro, "EBX") == 0){	
        registroEBX = valor;       
    }else if(strcmp(registro, "ECX") == 0){	
        registroECX = valor;
    }else if(strcmp(registro, "EDX") == 0){	
        registroEDX = valor;            
    }
}

void escribir_en_registro_de_16_bytes(char* registro, char valor[16]){
    if(strcmp(registro, "RAX") == 0){	
        //registroRAX = valor; //TODO SE USA STRCOPY O ALGO SIMILAR
    }else if(strcmp(registro, "RBX") == 0){	
        //registroRBX = valor;
    }else if(strcmp(registro, "RCX") == 0){ 
        //registroRCX = valor;
    }else if(strcmp(registro, "RDX") == 0){	
        //registroRDX = valor;
    }
}


int obtener_direccion_logica(){
    int dl;
    char* nombre_instruccion_actual; //TODO GLOBAL O LO QUE SEA
    if(strcmp(nombre_instruccion_actual, "MOV_IN") == 0){
        dl = atoi(instruccion->param2);
    }else if(strcmp(nombre_instruccion_actual, "MOV_OUT") == 0){
        dl = atoi(instruccion->param1);
    }
    return dl;
}