//
// Created by utnso on 10/6/22.
//
#include <utils/estructuras.h>



t_proceso *crearNuevoProceso()
{
    t_proceso *nuevoProceso = malloc(sizeof(t_proceso));
    if(nuevoProceso == NULL){
        log_error(logger_kernel, "NO se pudo crear el proceso, problema con malloc");
        exit(2);
    }

    return nuevoProceso;
}

void liberarPcb(pcb* pcbALiberar)
{
    
    void liberarListaDeInstrucciones(instr_t* instruccion)
    {
        free(instruccion->id);
        free(instruccion->param1);
        free(instruccion->param2);
        free(instruccion);
    }
    void liberarTablaSegmentos(segmento* seg){
        free(seg);
    }
    
    //list_destroy_and_destroy_elements(pcbALiberar->instr, (void*)liberarListaDeInstrucciones);
    //TODO ROMPE list_destroy_and_destroy_elements(pcbALiberar->tablaSegmentos, (void*)liberarTablaSegmentos);
    free(pcbALiberar->registrosCpu);
    free(pcbALiberar);
}

void closure_mostrarListaInstrucciones(instr_t* element) //Es compartido con consola
{
    printf("%s",element->id);
    printf(" %s",(char*) element->param1);
    printf(" %s\n",(char*) element->param2);
}

void closure_mostrarTamanioSegmentos(segmento* segmento){
    printf(" %d ,",(uint16_t) segmento->tamanioSegmento);
}

void closure_mostrarIndiceTablaPaginas(segmento* segmento){
    printf(" %d ,", (uint16_t) segmento->indiceTablaPaginas);
}

void mostrarPcb(pcb* pcbProceso){
    printf("\n--------------------------------------------------");
    printf("\nLa PCB del proceso es: \n");
    printf("La id es %d: ", pcbProceso->id);
    printf("\nLas instrucciones son: \n");
    list_iterate(pcbProceso->instr, closure_mostrarListaInstrucciones);
    printf("\nLa lista de segmentos es: \n[");
    list_iterate(pcbProceso->tablaSegmentos, closure_mostrarTamanioSegmentos);
    printf(" ]\nLos indices a la tabla de paginas es: \n[ ");
    list_iterate(pcbProceso->tablaSegmentos, closure_mostrarIndiceTablaPaginas);
    printf("]\n ---------------------------------------------------\nFIN MOSTRAR PCB\n\n");


}

void mostrarIntArray(uint32_t *array, char*message, t_log* logger){
    //CUIDADO, MANDAR %d siempre en el mensaje
    for (int i = 0; i < array[0]; ++i) {
        log_info(logger,message,array[i+1]);
    }
}

int retornarEnSegundos(int milisegundos){
    return milisegundos/1000; //TODO MEJOR UTILIZAR USLEEP
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