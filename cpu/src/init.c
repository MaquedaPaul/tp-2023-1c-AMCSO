//
// Created by utnso on 4/7/23.
//

#include <init.h>
bool semaforosCreados = false;

int cargar_configuracion(char *path) {

    cfg_cpu->RETARDO_INSTRUCCION = config_get_int_value(file_cfg_cpu, "RETARDO_INSTRUCCION");
    log_trace(trace_logger, "RETARDO_INSTRUCCION cargado correctamente: %d", cfg_cpu->RETARDO_INSTRUCCION);

    cfg_cpu->IP_MEMORIA = config_get_string_value(file_cfg_cpu, "IP_MEMORIA");
    log_trace(trace_logger, "IP_MEMORIA Cargada Correctamente: %s", cfg_cpu->IP_MEMORIA);

    cfg_cpu->PUERTO_MEMORIA = config_get_string_value(file_cfg_cpu, "PUERTO_MEMORIA");
    log_trace(trace_logger, "PUERTO_MEMORIA Cargado Correctamente: %s", cfg_cpu->PUERTO_MEMORIA);

    cfg_cpu->PUERTO_ESCUCHA = config_get_string_value(file_cfg_cpu, "PUERTO_ESCUCHA");
    log_trace(trace_logger, "PUERTO_ESCUCHA cargado correctamente: %s", cfg_cpu->PUERTO_ESCUCHA);

    cfg_cpu->TAM_MAX_SEGMENTO = config_get_int_value(file_cfg_cpu, "TAM_MAX_SEGMENTO");
    log_trace(trace_logger, "TAM_MAX_SEGMENTO cargado correctamente: %d", cfg_cpu->TAM_MAX_SEGMENTO);

    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");

    return true;
}