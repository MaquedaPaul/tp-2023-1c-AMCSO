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

void init_logs_configs(char* path_config)
{
    /*
    if(cfg_console == NULL){
    }
     */
    cfg_console = cfg_console_start();
    trace_logger = log_create("trace_logger.log","Console", true, LOG_LEVEL_TRACE);
    debug_logger = log_create("debug_logger.log","Console", true, LOG_LEVEL_DEBUG);
    info_logger = log_create("info_logger.log","Console", true, LOG_LEVEL_INFO);
    warning_logger = log_create("warning_logger.log","Console", true, LOG_LEVEL_WARNING);
    error_logger = log_create("error_logger.log","Console", true, LOG_LEVEL_ERROR);

    if(trace_logger== NULL || debug_logger== NULL || info_logger== NULL || warning_logger== NULL || error_logger== NULL){
        printf("No pude crear los loggers");
        exit(2);
    }
    file_cfg_console = iniciar_config(path_config);
    //la funcion de aca arriba genera leaks still reacheables pero como es generado por las commons
    //no se si se puede solucionar o si hace verdaderamente falta
}

t_config_console *cfg_console_start()
// Inicializo en NULL para no tener basura
{
    t_config_console *cfg = malloc(sizeof(t_config_console));
    return cfg;
}