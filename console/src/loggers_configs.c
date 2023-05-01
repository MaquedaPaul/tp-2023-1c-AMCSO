//
// Created by utnso on 4/8/23.
//

#include <loggers_configs.h>
t_log* trace_logger;
t_log* debug_logger;
t_log* info_logger;
t_log* warning_logger;
t_log* error_logger;
t_config* file_cfg_console;
t_config_console *cfg_console;
char* path_config;
bool logsCreados = false;
bool configCreado = false;
bool cfgCreado = false;

bool init_logs_configs(char* path_config)
{
    /*
    if(cfg_console == NULL){
    }
     */
    cfg_console = cfg_console_start();
    if(cfg_console == NULL){
        return  false;
    }
    cfgCreado = true;
    trace_logger = log_create("trace_logger.log","Console", true, LOG_LEVEL_TRACE);
    debug_logger = log_create("debug_logger.log","Console", true, LOG_LEVEL_DEBUG);
    info_logger = log_create("info_logger.log","Console", true, LOG_LEVEL_INFO);
    warning_logger = log_create("warning_logger.log","Console", true, LOG_LEVEL_WARNING);
    error_logger = log_create("error_logger.log","Console", true, LOG_LEVEL_ERROR);

    if(trace_logger== NULL || debug_logger== NULL || info_logger== NULL || warning_logger== NULL || error_logger== NULL){
        printf("No pude crear los loggers");
        return false;
    }
    logsCreados = true;
    file_cfg_console = iniciar_config(path_config);
    if(file_cfg_console == NULL){
        return  false;
    }
    configCreado = true;
    return checkProperties(path_config);
    //la funcion de aca arriba genera leaks still reacheables pero como es generado por las commons
    //no se si se puede solucionar o si hace verdaderamente falta
}

bool checkProperties(char *path) {

    t_config *config = config_create(path);
    if (config == NULL) {
        log_error(error_logger,"No se pudo crear la config para checkProperties");
        return false;
    }

    char *properties[] = {
            "IP_KERNEL",
            "PUERTO_KERNEL",
            NULL};

    if (!config_has_all_properties(config, properties))
    {
        log_error(error_logger, "Propiedades faltantes en el archivo de configuracion");
        return false;
    }
    log_trace(trace_logger,"Archivo de configuracion comprobado correctamente");
    config_destroy(config);
    return true;
}



t_config_console *cfg_console_start()
// Inicializo en NULL para no tener basura
{
    t_config_console *cfg = malloc(sizeof(t_config_console));
    return cfg;
}