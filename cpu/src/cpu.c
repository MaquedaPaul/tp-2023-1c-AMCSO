#include <cpu.h>


//Instruccion a ejecutar
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
        int valor = atoi(instruccion->param2);

        escribir_en_registro(instruccion->param1, instruccion->param1);

            //pcb_actual->programCounter++
                                             
        } else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) { // Simular retardo
            //MOV_IN (Registro, Dirección Lógica)

            int direccionLogica = obtener_direccion_logica();

            int direccion_fisica = obtener_direccion_fisica(direccionLogica);

            int tamaño_a_leer = tamaño_a_escribir(instruccion->param1); //mando el tipo de registro


        if(es_segmetation_fault(direccion_fisica, direccionLogica, tamaño_a_leer)){
            //comunicarse con kernel para que finalice el proceso por SEG_FAULT
            return 1;
        }

        int obtener_valor_en_memoria(){
            //mandarle direccion_fisica a memoria
            return 1;
        }
        //pcb_actual->programCounter++


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


void escribir_en_registro(char* registro, char valor[16]){
    strcpy(registro, valor);
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



