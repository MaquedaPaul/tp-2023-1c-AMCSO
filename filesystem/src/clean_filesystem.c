//
// Created by utnso on 5/4/23.
//

#include <clean_filesystem.h>

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
    if(lista_FCBsCreado){
        destruirListaFCBs();
    }
    if(archivosUsadosCreado){
        destruirArchivosUsados();
    }
    if(listaBloquesCreado){
        destruirListaBloques();
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
    free(cfg_filesystem->PATH_FCB);
    free(cfg_filesystem->PATH_BLOQUES);
    free(cfg_filesystem->PATH_BITMAP);
    free(cfg_filesystem->PATH_SUPERBLOQUE);
    free(cfg_filesystem);
}

void destruirConexiones(){

}

void destruirSemaforos(){
    pthread_mutex_destroy(&mutex_ArchivosUsados);
}

void destruirListaFCBs(){
    void destruirFcbs(t_config_fcb* unFcb){
        free(unFcb->NOMBRE_ARCHIVO);
        //free(&unFcb->PUNTERO_INDIRECTO);
        //free(&unFcb->PUNTERO_DIRECTO);
        config_destroy(unFcb->fcb_config);
        free(unFcb);
    }
    list_clean_and_destroy_elements(lista_FCBs, destruirFcbs);
    list_destroy(lista_FCBs);

}
void destruirArchivosUsados(){
    list_clean(archivosUsados);
    list_destroy(archivosUsados);
}
void destruirListaBloques(){
    list_clean_and_destroy_elements(lista_bloques,free);
    list_destroy(lista_bloques);
}