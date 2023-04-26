//
// Created by utnso on 4/7/23.
//

#include <init.h>

int cargar_configuracion(char *path) {



    file_cfg_memory = config_create(path);

    cfg_memory->PUERTO_ESCUCHA = strdup(config_get_string_value(file_cfg_memory, "PUERTO_ESCUCHA"));
    log_trace(trace_logger, "PUERTO_ESCUCHA Cargada Correctamente: %s", cfg_memory->PUERTO_ESCUCHA);

    cfg_memory->TAM_MEMORIA = config_get_int_value(file_cfg_memory, "TAM_MEMORIA");
    log_trace(trace_logger, "TAM_MEMORIA Cargada Correctamente: %d",
             cfg_memory->TAM_MEMORIA);

    cfg_memory->TAM_SEGMENTO_0 = config_get_int_value(file_cfg_memory, "TAM_SEGMENTO_0");
    log_trace(trace_logger, "TAM_SEGMENTO_0 Cargada Correctamente: %d",
             cfg_memory->TAM_SEGMENTO_0);

    cfg_memory->CANT_SEGMENTOS = config_get_int_value(file_cfg_memory, "CANT_SEGMENTOS");
    log_trace(trace_logger, "CANT_SEGMENTOS Cargada Correctamente: %d",
             cfg_memory->CANT_SEGMENTOS);

    cfg_memory->RETARDO_MEMORIA = config_get_int_value(file_cfg_memory, "RETARDO_MEMORIA");
    log_trace(trace_logger, "RETARDO_MEMORIA Cargada Correctamente: %d",
             cfg_memory->RETARDO_MEMORIA);

    cfg_memory->RETARDO_COMPACTACION = config_get_int_value(file_cfg_memory, "RETARDO_COMPACTACION");
    log_trace(trace_logger, "RETARDO_COMPACTACION Cargada Correctamente: %d",
             cfg_memory->RETARDO_COMPACTACION);

    cfg_memory->ALGORITMO_ASIGNACION = strdup(config_get_string_value(file_cfg_memory, "ALGORITMO_ASIGNACION"));
    log_trace(trace_logger, "ALGORITMO_ASIGNACION Cargada Correctamente: %s", cfg_memory->ALGORITMO_ASIGNACION);


    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");

    config_destroy(file_cfg_memory);
    return true;
}

void crearEstructurasAdministrativas(){
    crearSemaforos();
    crearEspacioContiguoDeMemoria();
    crearTablasSegmentos();
    crearListaHuecosLibres();
    crearListaHuecosUsados();
    crearSegmento0();
    cantidadMaximaSegmentos = cfg_memory->CANT_SEGMENTOS;
}

bool crearSemaforos(){
    pthread_mutex_init(&mutex_espacioContiguo,NULL);
    pthread_mutex_init(&mutex_espacioDisponible,NULL);
    pthread_mutex_init(&mutex_huecosDisponibles,NULL);
    pthread_mutex_init(&mutex_huecosUsados,NULL);
    pthread_mutex_init(&mutex_idSegmento,NULL);
    pthread_mutex_init(&mutex_tablasSegmentos,NULL);

    return true;
}


bool crearEspacioContiguoDeMemoria(){
    espacio_contiguo = malloc(cfg_memory->TAM_MEMORIA);
    espacioDisponible = cfg_memory->TAM_MEMORIA;
    memset(espacio_contiguo,0,sizeof (espacio_contiguo));
    return true;
}
bool crearTablasSegmentos(){
    tablasSegmentos = list_create();
    if(tablasSegmentos == NULL){
        return false;
    }
    return true;
}
bool crearListaHuecosLibres(){
    huecosLibres = list_create();
    if(huecosLibres == NULL){
        return false;
    }
    return true;
}

bool crearListaHuecosUsados(){
    huecosUsados = list_create();
    if(huecosUsados == NULL){
        return false;
    }
    return true;
}
bool crearSegmento0(){

    t_segmento* huecoLibreInicial = malloc(sizeof(t_segmento));
    huecoLibreInicial->base = 0;
    huecoLibreInicial->limite = cfg_memory->TAM_MEMORIA;
    huecoLibreInicial->id= idDisponible;
    idDisponible++;
    segmento0 = dividirEnDosYObtenerUtilizado(huecoLibreInicial,cfg_memory->TAM_SEGMENTO_0);
    espacioDisponible-= cfg_memory->TAM_SEGMENTO_0;
    return true;
}