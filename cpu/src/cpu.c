#include <cpu.h>




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