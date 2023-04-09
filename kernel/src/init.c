//
// Created by utnso on 4/7/23.
//

#include <init.h>

int cargar_configuracion(char *path) {

    file_cfg_kernel = config_create(path);
    cfg_kernel->IP_MEMORIA = strdup(config_get_string_value(file_cfg_kernel, "IP_MEMORIA"));
    log_info(trace_logger, "IP_MEMORIA Cargada Correctamente: %s", cfg_kernel->IP_MEMORIA);

    cfg_kernel->PUERTO_MEMORIA = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_MEMORIA"));
    log_info(trace_logger, "PUERTO_MEMORIA Cargada Correctamente: %s", cfg_kernel->PUERTO_MEMORIA);

    cfg_kernel->IP_FILESYSTEM = strdup(config_get_string_value(file_cfg_kernel, "IP_FILESYSTEM"));
    log_info(trace_logger, "IP_FILESYSTEM Cargada Correctamente: %s", cfg_kernel->IP_FILESYSTEM);

    cfg_kernel->PUERTO_FILESYSTEM = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_FILESYSTEM"));
    log_info(trace_logger, "PUERTO_FILESYSTEM Cargada Correctamente: %s", cfg_kernel->PUERTO_FILESYSTEM);


    cfg_kernel->IP_CPU = strdup(config_get_string_value(file_cfg_kernel, "IP_CPU"));
    log_info(trace_logger, "IP_CPU Cargada Correctamente: %s", cfg_kernel->IP_CPU);

    cfg_kernel->PUERTO_CPU = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_CPU"));
    log_info(trace_logger, "PUERTO_CPU Cargada Correctamente: %s", cfg_kernel->PUERTO_CPU);

    cfg_kernel->PUERTO_ESCUCHA = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_ESCUCHA"));
    log_info(trace_logger, "PUERTO_ESCUCHA Cargada Correctamente: %s", cfg_kernel->PUERTO_ESCUCHA);

    cfg_kernel->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(file_cfg_kernel, "ALGORITMO_PLANIFICACION"));
    log_info(trace_logger, "ALGORITMO_PLANIFICACION Cargada Correctamente: %s", cfg_kernel->ALGORITMO_PLANIFICACION);

    cfg_kernel->ESTIMACION_INICIAL = config_get_int_value(file_cfg_kernel, "ESTIMACION_INICIAL");
    log_info(trace_logger, "ESTIMACION_INICIAL Cargada Correctamente: %d",
             cfg_kernel->ESTIMACION_INICIAL);

    cfg_kernel->HRRN_ALFA = config_get_double_value(file_cfg_kernel, "HRRN_ALFA");
    log_info(trace_logger, "HRRN_ALFA Cargada Correctamente: %d",
             cfg_kernel->HRRN_ALFA);


    cfg_kernel->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(file_cfg_kernel, "GRADO_MAX_MULTIPROGRAMACION");
    log_info(trace_logger, "GRADO_MAX_MULTIPROGRAMACION Cargada Correctamente: %d",
             cfg_kernel->GRADO_MAX_MULTIPROGRAMACION);


    cfg_kernel->RECURSOS = config_get_array_value(file_cfg_kernel, "RECURSOS");
    //log_info(logger_kernel, "DISPOSITIVOS_IO Cargada Correctamente: %s", cfg_kernel->DISPOSITIVOS_IO);

    cfg_kernel->INSTANCIAS_RECURSOS = config_get_array_value(file_cfg_kernel, "INSTANCIAS_RECURSOS");
    //log_info(logger_kernel, "TIEMPOS_IO Cargada Correctamente: %s", cfg_kernel->TIEMPOS_IO);


    log_info(logger_kernel, "Archivo de configuracion cargado correctamente");

    config_destroy(file_cfg_kernel);
    return true;
}

