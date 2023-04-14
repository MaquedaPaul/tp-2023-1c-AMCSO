//
// Created by utnso on 4/7/23.
//

#include <init.h>

char* path_pseudo;




bool cargar_configuracion(char *path)
{
    file_cfg_console = config_create(path);
    cfg_console->IP_KERNEL = strdup(config_get_string_value(file_cfg_console, "IP_KERNEL"));
    log_trace(trace_logger, "IP Cargada Correctamente: %s", cfg_console->IP_KERNEL);
    cfg_console->PUERTO_KERNEL = strdup(config_get_string_value(file_cfg_console, "PUERTO_KERNEL"));
    log_trace(trace_logger, "Puerto Cargado Correctamente: %s", cfg_console->PUERTO_KERNEL);
    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");
    config_destroy(file_cfg_console);
    return true;
}
