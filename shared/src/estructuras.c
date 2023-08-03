//
// Created by utnso on 10/6/22.
//
#include <estructuras.h>



t_proceso *crearNuevoProceso()
{
    t_proceso *nuevoProceso = malloc(sizeof(t_proceso));
    if(nuevoProceso == NULL){
        //log_error(logger_kernel, "NO se pudo crear el proceso, problema con malloc");
        exit(2);
    }

    return nuevoProceso;
}
void liberarInstruccion(t_instr * instruccion){
    if(esInstruccionSinParametros(instruccion)){

    }
    if(esInstruccionConUnParametro(instruccion)){
        free(instruccion->param1);
    }
    if(esInstruccionConDosParametros(instruccion)){
        free(instruccion->param1);
        free(instruccion->param2);
    }
    if(esInstruccionConTresParametros(instruccion)){
        free(instruccion->param1);
        free(instruccion->param2);
        free(instruccion->param3);
    }
    free(instruccion->id);
    free(instruccion);
}

void liberarSegmento(t_segmento* segmento){
    free(segmento);
}

void liberarPcb(t_pcb* pcb) {
    //TODO modificar la liberacion de la tabla de segmentos. Ya no se libera como lista. Sino como cebolla
    free(pcb->registrosCpu);
    // Liberar memoria de instr y tablaSegmentos si es necesario
    list_destroy_and_destroy_elements(pcb->instr,liberarInstruccion);
    list_destroy_and_destroy_elements(pcb->tablaSegmentos->segmentos,liberarSegmento);
    list_destroy_and_destroy_elements(pcb->tablaArchivosAbiertos,free); //Ya que no corresponde que la PCB libere el archivo.
    list_destroy_and_destroy_elements(pcb->recursosTomados,free);
    free(pcb->tablaSegmentos);
    free(pcb);
}

void liberarPcbCpu(t_pcb* pcb){
    //Lo tenemos que hacer distinto pq nosotros No serializamos la TablaArchivosAbiertos y Lista recursos tomados
    //Entonces sino le estariamos haciendo free a una memoria que nunca reservamos
    free(pcb->registrosCpu);
    list_destroy_and_destroy_elements(pcb->instr,liberarInstruccion);
    list_destroy_and_destroy_elements(pcb->tablaSegmentos->segmentos,liberarSegmento);
    free(pcb->tablaSegmentos);
    free(pcb);
}

void closure_mostrarListaInstrucciones(t_instr * element) //Es compartido con consola
{
    printf("%s",element->id);
    printf(" %s",(char*) element->param1);
    printf(" %s",(char*) element->param2);
    printf(" %s\n",(char*) element->param3);
}

void liberarTablaSegmentos(t_tablaSegmentos* tablaSegmentos){
    list_destroy_and_destroy_elements(tablaSegmentos->segmentos, liberarSegmento);
    free(tablaSegmentos);
}
void liberarTablasDeSegmentos(t_list* tablasSegmentos){
    list_destroy_and_destroy_elements(tablasSegmentos, liberarTablaSegmentos);
}



bool esInstruccionSinParametros(t_instr * instruccion){
    return (strcmp(instruccion->id, "EXIT") == 0) || (strcmp(instruccion->id, "YIELD") == 0);
}

bool esInstruccionConUnParametro(t_instr * instruccion){
    bool io = (strcmp(instruccion->id, "I/O") == 0);
    bool delete_segment = (strcmp(instruccion->id, "DELETE_SEGMENT") == 0);
    bool f_open = (strcmp(instruccion->id, "F_OPEN") == 0);
    bool f_close = (strcmp(instruccion->id, "F_CLOSE") == 0);
    bool wait = (strcmp(instruccion->id, "WAIT") == 0);
    bool signal = (strcmp(instruccion->id, "SIGNAL") == 0);

    return  io || delete_segment || f_open || f_close || wait || signal;
}


bool esInstruccionConDosParametros(t_instr * instruccion){

    bool set = (strcmp(instruccion->id, "SET") == 0);
    bool mov_out = (strcmp(instruccion->id, "MOV_OUT") == 0);
    bool mov_in = (strcmp(instruccion->id, "MOV_IN") == 0);
    bool f_truncate = (strcmp(instruccion->id, "F_TRUNCATE") == 0);
    bool f_seek = (strcmp(instruccion->id, "F_SEEK") == 0);
    bool create_segment = (strcmp(instruccion->id, "CREATE_SEGMENT") == 0);
    return set || mov_out || mov_in || f_truncate || f_seek || create_segment;
}

bool esInstruccionConTresParametros(t_instr * instruccion){
    return (strcmp(instruccion->id, "F_WRITE") == 0) || (strcmp(instruccion->id, "F_READ") == 0);
}

void establecerCantidadParametrosInstrucciones(t_list* listaInstrucciones){
    void establecerCantidadParams(t_instr* unaInstruccion){

        if(esInstruccionSinParametros(unaInstruccion)){
            unaInstruccion->cantidad_parametros= 0;
        }
        if(esInstruccionConUnParametro(unaInstruccion)){
            unaInstruccion->cantidad_parametros= 1;
        }
        if(esInstruccionConDosParametros(unaInstruccion)){
            unaInstruccion->cantidad_parametros= 2;
        }
        if(esInstruccionConTresParametros(unaInstruccion)){
            unaInstruccion->cantidad_parametros= 3;
        }
    }

    list_iterate(listaInstrucciones, establecerCantidadParams);
}


void mostrarIntArray(uint32_t *array, char*message, t_log* logger){
    //CUIDADO, MANDAR %d siempre en el mensaje
    for (int i = 0; i < array[0]; ++i) {
        log_info(logger,message,array[i+1]);
    }
}





void simularRetardoSinMensaje(int retardo){
    usleep(retardo*1000);
}

void simularRetardo(char* message,char* messageFinal, t_log* logger, int retardo){
    log_info(logger, message, retardo);
    usleep(retardo*1000);
    if (strcmp(messageFinal, "") != 0){
        log_info(logger, messageFinal);
    }
}

t_list * convertirIntArrayALista(uint32_t * array){
    t_list * nuevaLista= list_create();
    for (int i = 0; i <array[0] ; ++i) {
        list_add(nuevaLista, &array[i+1]);
    }

    return nuevaLista;
}

uint32_t* convertirIntListaToArray(t_list* lista){
    uint32_t listSize = list_size(lista);
    uint32_t* nuevoArray = calloc(listSize+1,sizeof(uint32_t));
    nuevoArray[0] = listSize;
    for (int i = 0; i <nuevoArray[0] ; ++i) {
        uint32_t* elemento =list_get(lista,i);
        nuevoArray[i+1] = *elemento;
    }
    return nuevoArray;
}