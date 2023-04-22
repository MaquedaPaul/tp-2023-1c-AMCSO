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
char[16] registroRAX;
char[16] registroRCX;
char[16] registroRBX;
char[16] registroRDX;

instr_t* fetch(pcb* pcbActual){
    return list_get(pcbActual->instr, pcbActual->programCounter);
}

//DECODE
char* decode(instr_t* instruccion){
    return instruccion->id;     //nombre de la instruccion
}

//EXECUTE
void execute(char* nombre_instruccion_actual){
    
    if(strcmp(nombre_instruccion_actual, "SET") == 0) {
            //SET (Registro, Valor)
            int valor = atoi(instruccion->param2);
            if(strlen(instruccion->param2) == 4){
                escribir_en_registro_de_4_bytes(instruccion->param1, valor)
            }else if(strlen(instruccion->param2) == 8)
                escribir_en_registro_de_8_bytes(instruccion->param1, valor);
            }else if(strlen(instruccion->param2) == 16){
                escribir_en_registro_de_16_bytes(instruccion->param1, instruccion->param2); //ver como le paso el valor de 16 bytes
            }

            //Similar retardo
        }
            else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0){
                //MOV_IN (Registro, Dirección Lógica)

            } else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {
                //MOV_OUT (Dirección Lógica, Registro)


            } else if (strcmp(nombre_instruccion_actual, "F_OPEN ") == 0) {
                //F_OPEN (Nombre Archivo)
                

            } else if (strcmp(nombre_instruccion_actual, "F_CLOSE") == 0) {
                //F_CLOSE (Nombre Archivo)

            } else if (strcmp(nombre_instruccion_actual, "F_SEEK") == 0) {
               //F_SEEK (Nombre Archivo, Posición)

            } else if(strcmp(nombre_instruccion_actual, "F_READ") == 0){
                //F_READ (Nombre Archivo, Dirección Lógica, Cantidad de Bytes)

            }else if(strcmp(nombre_instruccion_actual, "F_WRITE") == 0){
                //F_WRITE (Nombre Archivo, Dirección Lógica, Cantidad de Bytes) 

            }else if(strcmp(nombre_instruccion_actual, "F_TRUNCATE") == 0){
                //F_TRUNCATE (Nombre Archivo, Tamaño)

            }else if(strcmp(nombre_instruccion_actual, "WAIT") == 0){
                //WAIT (Recurso)

            }else if(strcmp(nombre_instruccion_actual, "SIGNAL") == 0){
                //SIGNAL (Recurso)

            }else if(strcmp(nombre_instruccion_actual, "CREATE_SEGMENTC") == 0){
                //CREATE_SEGMENT (Id del Segmento, Tamaño)

            }else if(strcmp(nombre_instruccion_actual, "DELETE_SEGMENT") == 0){
                //DELETE_SEGMENT (Id del Segmento)

            }else if(strcmp(nombre_instruccion_actual, "I/O") == 0){
                //I/O (Tiempo)

            }else if(strcmp(nombre_instruccion_actual, "YIELD") == 0){
                //YIELD

            }else if(strcmp(nombre_instruccion_actual, "EXIT") == 0){
                //EXIT
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

void escribir_en_registro_de_16_bytes(char* registro, char[16] valor){
    if(strcmp(registro, "RAX") == 0){	
        registroRAX = valor;
    }else if(strcmp(registro, "RBX") == 0){	
        registroRBX = valor;
    }else if(strcmp(registro, "RCX") == 0){ 
        registroRCX = valor;
    }else if(strcmp(registro, "RDX") == 0){	
        registroRDX = valor;
    }
}