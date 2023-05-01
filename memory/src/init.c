//
// Created by utnso on 4/7/23.
//

#include <init.h>

int cargar_configuracion(char *path) {


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

    return true;
}

bool crearEstructurasAdministrativas(){
    bool comp1 = crearSemaforos();
    bool comp2 = crearEspacioContiguoDeMemoria();
    bool comp3 = crearTablasSegmentos();
    bool comp4 = crearListaHuecosLibres();
    bool comp5 = crearListaHuecosUsados();
    bool comp6 = crearSegmento0();
    cantidadMaximaSegmentos = cfg_memory->CANT_SEGMENTOS;
    return comp1 && comp2 && comp3 && comp4 && comp5 && comp6;
}

bool crearSemaforos(){
    int comprobacionEspacioContiguo = pthread_mutex_init(&mutex_espacioContiguo,NULL);
    int comprobacionEspacioDisponiblbe = pthread_mutex_init(&mutex_espacioDisponible,NULL);
    int comprobacionHuecosDisponibles = pthread_mutex_init(&mutex_huecosDisponibles,NULL);
    int comprobacionHuecosUsados = pthread_mutex_init(&mutex_huecosUsados,NULL);
    int comprobacionIdSegmento = pthread_mutex_init(&mutex_idSegmento,NULL);
    int comprobacionTablasSegmentos = pthread_mutex_init(&mutex_tablasSegmentos,NULL);

    bool noEsIgualACero(int numero){
        return numero != 0;
    }
    if(noEsIgualACero(comprobacionEspacioContiguo) || noEsIgualACero(comprobacionEspacioDisponiblbe) || noEsIgualACero(comprobacionHuecosDisponibles)){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }
    if(noEsIgualACero(comprobacionHuecosUsados) || noEsIgualACero(comprobacionIdSegmento) || noEsIgualACero(comprobacionTablasSegmentos)){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }
    semaforosCreados = true;
    return true;
}


bool crearEspacioContiguoDeMemoria(){
    espacio_contiguo = malloc(cfg_memory->TAM_MEMORIA);
    espacioDisponible = cfg_memory->TAM_MEMORIA;
    memset(espacio_contiguo,0,sizeof (espacio_contiguo));
    //Sin comprobación posible aparente
    memoriaInicializada = true;
    return true;
}
bool crearTablasSegmentos(){
    tablasSegmentos = list_create();
    if(tablasSegmentos == NULL){
        return false;
    }
    tablaSegmentosCreada = true;
    return true;
}
bool crearListaHuecosLibres(){
    huecosLibres = list_create();
    if(huecosLibres == NULL){
        return false;
    }
    listaDisponiblesCreada = true;
    return true;
}

bool crearListaHuecosUsados(){
    huecosUsados = list_create();
    if(huecosUsados == NULL){
        return false;
    }
    listaUsadosCreada = true;
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
    //Sin comprobación posible
    segmento0Creado = true;
    return true;
}

bool iniciarMemoria(){
    //TODO Si hiciera falta que exclusivamente tiene que generarse las conexiones antes que las estructuras, acá es donde hay que tocar.
    bool estructurasAdministrativas = crearEstructurasAdministrativas();
    if (!generar_conexiones()){
        //cerrar_programa();
        return false;
    }
    return estructurasAdministrativas;
}