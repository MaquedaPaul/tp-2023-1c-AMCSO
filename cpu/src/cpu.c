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

pcb* pcb_actual;
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


typedef struct{

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

}registros_cpu;


typedef struct{
	uint32_t id;
	t_list* instr;
    registros_cpu* registrosCpu;
	uint32_t programCounter;
    t_list* tablaSegmentos;
} pcb;


typedef struct {
    uint8_t idLength;
    char* id;
    uint8_t cantidad_parametros;
	uint8_t param1Length;
    char* param1;
	uint8_t param2Length;
    char* param2;
    uint8_t param3Length;
    char* param3;
} instr_t;

typedef struct t_segmento{
    u_int32_t nro_segmento;
    u_int32_t direccion_base;
    u_int32_t tamanio_segmento;

} t_segmento;



bool bandera = true;
instr_t* instruccion;
char* nombre_instruccion_actual;
pcb *pcb_actual;

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




void iniciar_registros (registros_cpu* registros_PCB) {

//memcpy(registroCpu_AX,registros_PCB->AX,4);

strcpy( registroCpu_AX, registros_PCB->AX );
strcpy( registroCpu_BX, registros_PCB->BX );
strcpy( registroCpu_CX, registros_PCB->CX );
strcpy( registroCpu_DX, registros_PCB->DX );
strcpy( registroCpu_EAX, registros_PCB->EAX );
strcpy( registroCpu_EBX, registros_PCB->EBX );
strcpy( registroCpu_ECX, registros_PCB->ECX );
strcpy( registroCpu_EDX, registros_PCB->EDX );
strcpy( registroCpu_RAX, registros_PCB->RAX );
strcpy( registroCpu_RBX, registros_PCB->RBX );
strcpy( registroCpu_RCX, registros_PCB->RCX );
strcpy( registroCpu_RDX, registros_PCB->RDX );

}


void ciclo_instrucciones(){
    
// instr_t* instruccion;

    while(bandera){      // && (pcb->programCounter < list_size(pcb->instr))

        instruccion = fetch();

        nombre_instruccion_actual = decode(); // decode(instruccion);

        execute();  }  // execute(instruccion);

    bandera = true;
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
        log_info(logger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%s>\n", pcb_actual->id, registro, valor);  

        ejecutar_SET(registro, valor);
    }

    else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) {
        char* registro = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(logger, "PID: <%d> - Ejecutando: <MOV_IN> - <%s> - <%d>\n", pcb_actual->id, registro, direccion_logica);  

        ejecutar_MOV_IN(registro, direccion_logica);
    }

    else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {
        int direccion_logica = atoi(instruccion->param1);
        char* registro = instruccion->param2;
        log_info(logger, "PID: <%d> - Ejecutando: <MOV_OUT> - <%d> - <%s>\n", pcb_actual->id, direccion_logica, registro );  

        ejecutar_MOV_OUT(direccion_logica, registro);
    }

    else if (strcmp(nombre_instruccion_actual, "I/O") == 0) {
        
        int tiempo = instruccion->param1;
        log_info(logger, "PID: <%d> - Ejecutando <I/O> - <%d>\n", pcb_actual->id, tiempo);
        ejecutar_IO(tiempo);
    }

        else if (strcmp(nombre_instruccion_actual, "F_OPEN") == 0) {
        char* nombre_archivo = instruccion->param1;
        log_info(logger, "PID: <%d> - Ejecutando <F_OPEN> - <%s>\n", pcb_actual->id, nombre_archivo);

        ejecutar_F_OPEN(nombre_archivo);
    }
    else if (strcmp(nombre_instruccion_actual, "F_CLOSE") == 0) {
        char* nombre_archivo = instruccion->param1;
        log_info(logger, "PID: <%d> - Ejecutando <F_CLOSE> - <%s>\n", pcb_actual->id, nombre_archivo);

        ejecutar_F_CLOSE(nombre_archivo);
    }
    else if (strcmp(nombre_instruccion_actual, "F_SEEK") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(logger, "PID: <%d> - Ejecutando: <F_SEEK> - <%s> - <%d>\n", pcb_actual->id, nombre_archivo, direccion_logica );  

        ejecutar_F_SEEK(nombre_archivo, direccion_logica);
    }
    else if (strcmp(nombre_instruccion_actual, "F_READ") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
       int cantidad_de_bytes = atoi(instruccion->param3);
        log_info(logger, "PID: <%d> - Ejecutando: <F_READ> - <%s> - <%d> - <%d>\n", pcb_actual->id, nombre_archivo, direccion_logica, cantidad_de_bytes );  

        ejecutar_F_READ(nombre_archivo, direccion_logica, cantidad_de_bytes);
    }
    else if (strcmp(nombre_instruccion_actual, "F_WRITE") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
       int cantidad_de_bytes = atoi(instruccion->param3);
        log_info(logger, "PID: <%d> - Ejecutando: <F_WRITE> - <%s> - <%d> - <%d>\n", pcb_actual->id, nombre_archivo, direccion_logica, cantidad_de_bytes );  

        ejecutar_F_WRITE(nombre_archivo, direccion_logica, cantidad_de_bytes);
    }
    else if (strcmp(nombre_instruccion_actual, "F_TRUNCATE") == 0) {
        char* nombre_archivo = instruccion->param1;
        int tamanio = atoi(instruccion->param2);
        log_info(logger, "PID: <%d> - Ejecutando: <F_TRUNCATE> - <%s> - <%d>\n", pcb_actual->id, nombre_archivo, tamanio );  

        ejecutar_F_TRUNCATE(nombre_archivo, tamanio);
    }
    else if (strcmp(nombre_instruccion_actual, "WAIT") == 0) {
        char* recurso = instruccion->param1;
        log_info(logger, "PID: <%d> - Ejecutando <WAIT> - <%s>\n", pcb_actual->id, recurso);

        ejecutar_WAIT(recurso);
    }
    else if (strcmp(nombre_instruccion_actual, "SIGNAL") == 0) {
        char* recurso = instruccion->param1;
        log_info(logger, "PID: <%d> - Ejecutando <SIGNAL> - <%s>\n", pcb_actual->id, recurso);

        ejecutar_SIGNAL(recurso);
    }
    else if (strcmp(nombre_instruccion_actual, "CREATE_SEGMENT") == 0) {
        int id_del_segmento = atoi(instruccion->param1);
        int tamanio = atoi(instruccion->param2);
        log_info(logger, "PID: <%d> - Ejecutando: <CREATE_SEGMENT> - <%d> - <%d>\n", pcb_actual->id, id_del_segmento, tamanio );  

        ejecutar_CREATE_SEGMENT(id_del_segmento, tamanio);
    }
    else if (strcmp(nombre_instruccion_actual, "DELETE_SEGMENT") == 0) {
        int id_del_segmento = atoi(instruccion->param1);
        log_info(logger, "PID: <%d> - Ejecutando: <DELETE_SEGMENT> - <%d>\n", pcb_actual->id, id_del_segmento );  

        ejecutar_DELETE_SEGMENT(id_del_segmento);
    }
    else if (strcmp(nombre_instruccion_actual, "YIELD") == 0) {
        log_info(logger, "PID: <%d> - Ejecutando: <YIELD>\n", pcb_actual->id );  

        ejecutar_YIELD();
    }

     else if (strcmp(nombre_instruccion_actual, "EXIT") == 0) {
        log_info(logger, "PID: <%d> - Ejecutando: <EXIT> -\n", pcb_actual->id);

        ejecutar_EXIT();
    } else {

        log_info(logger, "PID: <%d> - NO EXISTE LA INSTRUCCION\n", pcb_actual->id); // log_error() ?


    }
}





void copiar_registros(registros_cpu* registros_PCB) {

//memcpy(registros_PCB->AX,registroCpu_AX,4);
//memcpy(registros_PCB->AX,registroCpu_BX,4);
//memcpy(registros_PCB->AX,registroCpu_CX,4);
//memcpy(registros_PCB->AX,registroCpu_DX,4);
//memcpy(registros_PCB->AX,registroCpu_EAX,8);
//memcpy(registros_PCB->AX,registroCpu_EBX,8);
//memcpy(registros_PCB->AX,registroCpu_ECX,8);
//memcpy(registros_PCB->AX,registroCpu_EDX,8);
//memcpy(registros_PCB->AX,registroCpu_RAX,16);
//memcpy(registros_PCB->AX,registroCpu_RBX,16);
//memcpy(registros_PCB->AX,registroCpu_RCX,16);
//memcpy(registros_PCB->AX,registroCpu_RDX,16);

strcpy( registros_PCB->AX , registroCpu_AX );
strcpy( registros_PCB->BX , registroCpu_BX );
strcpy( registros_PCB->CX , registroCpu_CX );
strcpy( registros_PCB->DX , registroCpu_DX );
strcpy( registros_PCB->EAX , registroCpu_EAX );
strcpy( registros_PCB->EBX , registroCpu_EBX );
strcpy( registros_PCB->ECX , registroCpu_ECX );
strcpy( registros_PCB->EDX , registroCpu_EDX );
strcpy( registros_PCB->RAX , registroCpu_RAX );
strcpy( registros_PCB->RBX , registroCpu_RBX );
strcpy( registros_PCB->RCX , registroCpu_RCX );
strcpy( registros_PCB->RDX , registroCpu_RDX );

}


void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio) {
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
    memcpy(paquete->buffer->stream + paquete->buffer->size, valor, tamanio);
    paquete->buffer->size += tamanio;
}

void agregar_instrucciones_a_paquete(t_paquete *paquete, t_list *instrucciones) {

    int cantidad_instrucciones = list_size(instrucciones);

    agregar_a_paquete(paquete, &cantidad_instrucciones, sizeof(int));

for (int i = 0; i < cantidad_instrucciones; i++) {
        t_instr *instruccion = list_get(instrucciones, i);

        agregar_a_paquete(paquete, &(instruccion->idLength), sizeof(int));
        agregar_a_paquete(paquete, instruccion->id, instruccion->idLength);

        agregar_a_paquete(paquete, &(instruccion->cantidad_parametros), sizeof(int));

        if(instruccion->cantidad_parametros == 1){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(int));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length);          }

        if(instruccion->cantidad_parametros == 2){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(int));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length);
            agregar_a_paquete(paquete,  &(instruccion->param2Length), sizeof(int));
            agregar_a_paquete(paquete, instruccion->param2, instruccion->param2Length);        }

    }
}


void agregar_segmentos_a_paquete(t_paquete *paquete, t_list *segmentos) {

     int cantidad_segmentos = list_size(segmentos);

    agregar_a_paquete(paquete, &cantidad_segmentos, sizeof(int));

    for (int i = 0; i < cantidad_segmentos; i++) {
        t_segmento *segmen = list_get(segmentos, i);
        agregar_a_paquete(paquete, &(segmen->id), sizeof(u_int32_t));
        agregar_a_paquete(paquete, &(segmen->tamanio), sizeof(u_int32_t));
        agregar_a_paquete(paquete, &(segmen->direccion_base), sizeof(u_int32_t));
    }
}

agregar_registros_a_paquete(t_paquete *paquete, registros_cpu *registro) {

int tamanioAX = strlen(registro->AX) + 1;
agregar_a_paquete(paquete, &tamanioAX, sizeof(int));
agregar_a_paquete(paquete, (registro->AX), tamanioAX);

int tamanioBX = strlen(registro->BX) + 1;
agregar_a_paquete(paquete, &(tamanioBX), sizeof(int));
agregar_a_paquete(paquete, (registro->BX), tamanioBX);

int tamanioCX = strlen(registro->CX) + 1;
agregar_a_paquete(paquete, &(tamanioCX), sizeof(int));
agregar_a_paquete(paquete, (registro->CX), tamanioCX);

int tamanioDX = strlen(registro->DX) + 1;
agregar_a_paquete(paquete, &(tamanioDX), sizeof(int));
agregar_a_paquete(paquete, (registro->DX), tamanioDX);

int tamanioEAX = strlen(registro->EAX) + 1;
agregar_a_paquete(paquete, &tamanioEAX, sizeof(int));
agregar_a_paquete(paquete, (registro->EAX), tamanioEAX);

int tamanioEBX = strlen(registro->EBX) + 1;
agregar_a_paquete(paquete, &(tamanioEBX), sizeof(int));
agregar_a_paquete(paquete, (registro->EBX), tamanioEBX);

int tamanioECX = strlen(registro->ECX) + 1;
agregar_a_paquete(paquete, &(tamanioECX), sizeof(int));
agregar_a_paquete(paquete, (registro->ECX), tamanioECX);

int tamanioEDX = strlen(registro->EDX) + 1;
agregar_a_paquete(paquete, &(tamanioEDX), sizeof(int));
agregar_a_paquete(paquete, (registro->EDX), tamanioEDX);

int tamanioRAX = strlen(registro->RAX) + 1;
agregar_a_paquete(paquete, &tamanioRAX, sizeof(int));
agregar_a_paquete(paquete, (registro->RAX), tamanioRAX);

int tamanioRBX = strlen(registro->RBX) + 1;
agregar_a_paquete(paquete, &(tamanioRBX), sizeof(int));
agregar_a_paquete(paquete, (registro->RBX), tamanioRBX);

int tamanioRCX = strlen(registro->RCX) + 1;
agregar_a_paquete(paquete, &(tamanioRCX), sizeof(int));
agregar_a_paquete(paquete, (registro->RCX), tamanioRCX);

int tamanioRDX = strlen(registro->RDX) + 1;
agregar_a_paquete(paquete, &(tamanioRDX), sizeof(int));
agregar_a_paquete(paquete, (registro->RDX), tamanioRDX);

}


void agregar_PCB_a_paquete(t_paquete *paquete) {
    agregar_a_paquete(paquete, &(pcb_actual->id), sizeof(u_int32_t));
    agregar_a_paquete(paquete, &(pcb_actual->programCounter), sizeof(u_int32_t));
    agregar_instrucciones_a_paquete(paquete, pcb_actual->instr);
    agregar_segmentos_a_paquete(paquete, pcb_actual->tablaSegmentos);
    agregar_registros_a_paquete(paquete, pcb_actual->registrosCpu) 


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



void ejecutar_MOV_IN(char* registro, int direccion_logica) {

    int cantidad_bytes = buscar_registro(registro);

    //if( buscar_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica,cantidad_bytes);
    if (!(direccion_fisica < 0)) {


    }


}



void ejecutar_MOV_OUT(char* nombre_archivo, int direccion_logica) {

    int cantidad_bytes = buscar_registro(registro);

    //if( buscar_registro(registro) < 0 )

    int direccion_fisica = traducir_direccion_logica(direccion_logica, cantidad_bytes);
    if (!(direccion_fisica < 0)) {

    }


}


void ejecutar_IO(int tiempo) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(IO_BLOCK);
    agregar_PCB_a_paquete(paquete);
    agregar_a_paquete(paquete, &tiempo, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}





void ejecutar_F_OPEN(char* nombre_archivo) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_OPEN);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}


void ejecutar_F_CLOSE(char* nombre_archivo) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_CLOSE);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}

void ejecutar_F_SEEK(char* nombre_archivo, int posicion) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->programCounter++;
    t_paquete* paquete = crear_paquete(F_SEEK);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &posicion, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}





int traducir_direccion_logica(int direccion_logica, int cantidad_de_bytes ) {

    int num_segmento = floor(direccion_logica / tam_max_segmento);
    int desplazamiento_segmento = direccion_logica % tam_max_segmento;

   if (error_segmentationFault(num_segmento, desplazamiento_segmento, cantidad_de_bytes)) {

            copiar_registros(pcb_actual->registrosCpu);
            t_paquete* paquete = crear_paquete(SEGMENTATION_FAULT);
            agregar_PCB_a_paquete(paquete);
            enviar_paquete(paquete, cliente_servidor);
            eliminar_paquete(paquete);
            return -1;
        }

    int direccion_fisica = segmento->direccion_base + desplazamiento_segmento;
    
    return direccion_fisica;
}




bool error_segmentationFault(int numero_segmento, int desplazamiento_segmento, int cantidad_bytes) {

    //if (numero_segmento >= list_size(pcb_actual->tabla_segmentos))  return true;

    segmento = list_get(pcb_actual->tablaSegmentos, numero_segmento);
    
    return ((desplazamiento_segmento + cantidad_bytes) > segmento->tamanio_segmento);
}




void ejecutar_F_READ(char* nombre_archivo, int direccion_logica, int cantidad_bytes) {
    int direccion_fisica = traducir_direccion_logica(direccion_logica);
    if (!(direccion_fisica < 0)) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_READ);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    }

    bandera = false;

}



void ejecutar_F_WRITE(char* nombre_archivo, int direccion_logica, int cantidad_bytes) {
    int direccion_fisica = traducir_direccion_logica(direccion_logica);
    if (!(direccion_fisica < 0)) {

    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_WRITE);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &cantidad_bytes, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    }

    bandera = false;

}


void ejecutar_F_TRUNCATE(char* nombre_archivo, int tamanio) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_TRUNCATE);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_archivo) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_archivo, largo_nombre);
    agregar_a_paquete(paquete, &tamanio, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}



void ejecutar_WAIT(char* nombre_recurso) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(WAIT);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}


void ejecutar_SIGNAL(char* nombre_recurso) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(SIGNAL);
    agregar_PCB_a_paquete(paquete);
    int largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete(paquete, &largo_nombre, sizeof(int));
    agregar_a_paquete(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}


void ejecutar_CREATE_SEGMENT(int id_del_segmento, int tamanio) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(CREATE_SEGMENT);
    agregar_PCB_a_paquete(paquete);
    agregar_a_paquete(paquete, &id_del_segmento, sizeof(int));
    agregar_a_paquete(paquete, &tamanio, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}

void ejecutar_DELETE_SEGMENT(int id_del_segmento) {
    
    copiar_registros(pcb_actual->registrosCpu);
    pcb_actual->program_counter++;
    t_paquete* paquete = crear_paquete(DELETE_SEGMENT);
    agregar_PCB_a_paquete(paquete);
    agregar_a_paquete(paquete, &id_del_segmento, sizeof(int));
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}



void ejecutar_YIELD() {
    copiar_registros(pcb_actual->registrosCpu);
    t_paquete* paquete = crear_paquete(YIELD);
    agregar_PCB_a_paquete(paquete);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}


void ejecutar_EXIT() {
    copiar_registros(pcb_actual->registrosCpu);
    t_paquete* paquete = crear_paquete(EXIT);
    agregar_PCB_a_paquete(paquete);
    enviar_paquete(paquete, cliente_servidor);
    eliminar_paquete(paquete);
    bandera = false;
}
