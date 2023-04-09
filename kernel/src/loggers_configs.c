//
// Created by utnso on 4/9/23.
//

#include "../include/loggers_configs.h"
t_log* trace_logger;
t_log* debug_logger;
t_log* info_logger;
t_log* warning_logger;
t_log* error_logger;
t_config* file_cfg_kernel;
t_config_console *cfg_kernel;

char* path_config;


int init_logs_configs(char *path_config) {
    cfg_kernel = cfg_kernel_start();
    trace_logger = log_create("trace_logger.log","Kernel", true, LOG_LEVEL_TRACE);
    debug_logger = log_create("debug_logger.log","Kernel", true, LOG_LEVEL_DEBUG);
    info_logger = log_create("info_logger.log","Kernel", true, LOG_LEVEL_INFO);
    warning_logger = log_create("warning_logger.log","Kernel", true, LOG_LEVEL_WARNING);
    error_logger = log_create("error_logger.log","Kernel", true, LOG_LEVEL_ERROR);

    if(trace_logger== NULL || debug_logger== NULL || info_logger== NULL || warning_logger== NULL || error_logger== NULL){
        printf("No pude crear los loggers");
        return false;
    }
    file_cfg_kernel = iniciar_config(path_config);
    return checkProperties(path_config);
}

t_config_kernel *cfg_kernel_start()
{
    t_config_kernel *cfg = malloc(sizeof(t_config_kernel));
    return cfg;
}