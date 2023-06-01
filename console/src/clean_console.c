//
// Created by utnso on 5/1/23.
//

#include <clean_console.h>

void cerrar_programa(){
    if(logsCreados){
        destruirLoggers();
    }
    if(configCreado){
        destruirConfig();
    }
    if(cfgCreado){
        destruirCfg();
    }
    if(conexionesHechas){
        //Nada que hacer, se hace detach
    }

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




