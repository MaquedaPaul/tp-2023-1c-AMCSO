//
// Created by utnso on 4/9/23.
//

#include <loggers_configs.h>
t_log* trace_logger;
t_log* debug_logger;
t_log* info_logger;
t_log* warning_logger;
t_log* error_logger;
t_config* file_cfg_cpu;
t_config_cpu *cfg_cpu;
char* path_config;

bool logsCreados = false;
bool configCreado = false;
bool cfgCreado = false;

int init_logs_configs(char *path_config) {
    cfg_cpu = cfg_cpu_start();
    if(cfg_cpu == NULL){
        return  false;
    }
    cfgCreado = true;
    trace_logger = log_create("trace_logger.log","Cpu", true, LOG_LEVEL_TRACE);
    debug_logger = log_create("debug_logger.log","Cpu", true, LOG_LEVEL_DEBUG);
    info_logger = log_create("info_logger.log","Cpu", true, LOG_LEVEL_INFO);
    warning_logger = log_create("warning_logger.log","Cpu", true, LOG_LEVEL_WARNING);
    error_logger = log_create("error_logger.log","Cpu", true, LOG_LEVEL_ERROR);

    if(trace_logger== NULL || debug_logger== NULL || info_logger== NULL || warning_logger== NULL || error_logger== NULL){
        printf("No pude crear los loggers");
        return false;
    }
    logsCreados = true;
    file_cfg_cpu = iniciar_config(path_config);
    if(file_cfg_cpu == NULL){
        return false;
    }
    configCreado = true;
    return checkProperties(path_config);
}

t_config_cpu *cfg_cpu_start()
{
    t_config_cpu *cfg = malloc(sizeof(t_config_cpu));
    return cfg;
}

bool checkProperties(char *path) {

    t_config *config = config_create(path);
    if (config == NULL) {
        printf("No se pudo crear la config");
        return false;
    }

    char *properties[] = {
            "RETARDO_INSTRUCCION",
            "IP_MEMORIA",
            "PUERTO_MEMORIA",
            "PUERTO_ESCUCHA",
            "TAM_MAX_SEGMENTO",
            NULL};

    // Falta alguna propiedad
    if (!config_has_all_properties(config, properties)) {
        log_error(error_logger, "Propiedades faltantes en el archivo de configuracion");
        return false;
    }

    config_destroy(config);

    return true;
}