//
// Created by utnso on 5/4/23.
//

#include "clean_filesystem.h"



void cerrar_programa(){
    if(logsCreados){
        destruirLoggers();
    }
    if(configCreado){
        //destruirConfig();
    }
    if(cfgCreado){
        destruirCfg();
    }
    if(conexionesHechas){
        //Nada que hacer, se hace detach
    }
    if(semaforosCreados){
        destruirSemaforos();
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
    config_destroy(file_cfg_filesystem);
}
void destruirCfg(){
    free(cfg_filesystem);
}

void destruirConexiones(){

}

void destruirSemaforos(){
}
