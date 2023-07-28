//
// Created by utnso on 4/27/23.
//
#include <clean_memory.h>


void cerrar_programa(){

    if(configCreado){
    destruirConfig();
    }
    if(cfgCreado){
    destruirCfg();
    }
    if(conexionesHechas){
        //Nada que hacer, se hace detach
    }
    if(memoriaInicializada){
    destruirMemoria();
    }
    if(listaDisponiblesCreada){
    destruirDisponibles();
    }
    if(listaUsadosCreada){
    destruirUsados();
    }
    if(tablaSegmentosCreada){
    destruirTablaSegmentos();
    }
    if(segmento0Creado){
    destruirSegmento0();
    }
    if(semaforosCreados){
    destruirSemaforos();
    }
    if(logsCreados){
        destruirLoggers();
    }
    printf("\nCierre exitoso\n");
}

void destruirLoggers(){
    log_destroy(info_logger);
    log_destroy(warning_logger);
    log_destroy(error_logger);
    log_destroy(debug_logger);
    log_trace(trace_logger,"Se liberan loggers");
    log_destroy(trace_logger);
}

void destruirConfig(){
    log_trace(trace_logger,"Se libera el file config");
    config_destroy(file_cfg_memory);
}
void destruirCfg(){
    log_trace(trace_logger,"Se libera la struct cfg");
    free(cfg_memory->ALGORITMO_ASIGNACION);
    free(cfg_memory->PUERTO_ESCUCHA);
    free(cfg_memory);

}

void destruirConexiones(){

}

void destruirMemoria(){
    log_trace(trace_logger,"Se libera la memoria contigua");
    free(espacio_contiguo);
}

void destruirDisponibles(){
    log_trace(trace_logger,"Se libera lista de huecos libres");
    list_clean_and_destroy_elements(huecosLibres,free);
    list_destroy(huecosLibres);
}
void destruirUsados(){
    log_trace(trace_logger,"Se libera lista de huecos usados");
    list_clean(huecosUsados);
    list_destroy(huecosUsados);
}

void destruirTablaSegmentos(){

    bool quitarSegmento0(t_segmento* unSegmento){
        return unSegmento->id == 0;
    }

    void quitarSegmento0DeTablas(t_tablaSegmentos* unaTabla){
        list_remove_by_condition(unaTabla->segmentos,quitarSegmento0);
    }

    void destruirTablasSegmentos(t_tablaSegmentos* tablaSegmentos){
        list_clean_and_destroy_elements(tablaSegmentos->segmentos,free);
        list_destroy(tablaSegmentos->segmentos);
        free(tablaSegmentos);
    }
    list_iterate(tablasSegmentos,quitarSegmento0DeTablas);

    list_clean_and_destroy_elements(tablasSegmentos, destruirTablasSegmentos);

    list_destroy(tablasSegmentos);
    log_trace(trace_logger,"Se liberan las tablas de segmentos");
}

void destruirSegmento0(){
    limpiarHueco(segmento0);
    log_trace(trace_logger,"Se libera el segmento 0");
}
void destruirSemaforos(){
    pthread_mutex_destroy(&mutex_espacioDisponible);
    pthread_mutex_destroy(&mutex_huecosDisponibles);
    pthread_mutex_destroy(&mutex_huecosUsados);
    pthread_mutex_destroy(&mutex_espacioContiguo);
    pthread_mutex_destroy(&mutex_tablasSegmentos);
    pthread_mutex_destroy(&mutex_idSegmento);
    log_trace(trace_logger,"Se liberan los mutex");
}

void destruirHilos(){

}