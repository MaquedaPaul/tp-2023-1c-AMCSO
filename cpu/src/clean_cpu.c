//
// Created by utnso on 5/5/23.
//

#include <clean_cpu.h>


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
    config_destroy(file_cfg_cpu);
}
void destruirCfg(){
    //free(cfg_cpu->PUERTO_ESCUCHA);
    //free(cfg_cpu->PUERTO_MEMORIA);
    //free(cfg_cpu->IP_MEMORIA);
    free(cfg_cpu);
}

void destruirConexiones(){
}

void destruirSemaforos(){
}
