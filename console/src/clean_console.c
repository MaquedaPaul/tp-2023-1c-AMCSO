//
// Created by utnso on 5/1/23.
//

#include <clean_console.h>

void cerrar_programa(){
    log_trace(trace_logger, "Cerrando programa");

    if(configCreado){
        destruirConfig();
        log_trace(trace_logger, "Liberando configs");
    }
    if(cfgCreado){
        log_trace(trace_logger, "Liberando cfgs");
        destruirCfg();
    }
    if(logsCreados){
        log_trace(trace_logger, "Liberando loggers");
        destruirLoggers();
    }
    if(conexionesHechas){
        //Nada que hacer, se hace detach
    }
    printf("Cierre exitoso");

}
void destruirLoggers(){
    log_destroy(info_logger);
    log_destroy(warning_logger);
    log_destroy(error_logger);
    log_destroy(trace_logger);
    log_destroy(debug_logger);
}

void destruirConfig(){
    config_destroy(file_cfg_console);
}
void destruirCfg(){
    free(cfg_console);
}




