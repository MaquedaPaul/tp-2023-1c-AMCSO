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