//
// Created by utnso on 4/7/23.
//

#include <init.h>
#include <sys/mman.h>

bool semaforosCreados = false;

int cargar_configuracion(char *path) {


    file_cfg_filesystem = config_create(path);
    cfg_filesystem->IP_MEMORIA = strdup(config_get_string_value(file_cfg_filesystem, "IP_MEMORIA"));
    log_trace(trace_logger, "IP_MEMORIA Cargada Correctamente: %s", cfg_filesystem->IP_MEMORIA);

    cfg_filesystem->PUERTO_MEMORIA = strdup(config_get_string_value(file_cfg_filesystem, "PUERTO_MEMORIA"));
    log_trace(trace_logger, "PUERTO_MEMORIA Cargada Correctamente: %s", cfg_filesystem->PUERTO_MEMORIA);

    cfg_filesystem->PUERTO_ESCUCHA = strdup(config_get_string_value(file_cfg_filesystem, "PUERTO_ESCUCHA"));
    log_trace(trace_logger, "PUERTO_ESCUCHA Cargada Correctamente: %s", cfg_filesystem->PUERTO_ESCUCHA);

    cfg_filesystem->PATH_SUPERBLOQUE = strdup(config_get_string_value(file_cfg_filesystem, "PATH_SUPERBLOQUE"));
    log_trace(trace_logger, "PATH_SUPERBLOQUE Cargada Correctamente: %s", cfg_filesystem->PATH_SUPERBLOQUE);

    cfg_filesystem->PATH_BITMAP = strdup(config_get_string_value(file_cfg_filesystem, "PATH_BITMAP"));
    log_trace(trace_logger, "PATH_BITMAP Cargada Correctamente: %s", cfg_filesystem->PATH_BITMAP);

    cfg_filesystem->PATH_BLOQUES = strdup(config_get_string_value(file_cfg_filesystem, "PATH_BLOQUES"));
    log_trace(trace_logger, "PATH_BLOQUES Cargada Correctamente: %s", cfg_filesystem->PATH_BLOQUES);

    cfg_filesystem->PATH_FCB = strdup(config_get_string_value(file_cfg_filesystem, "PATH_FCB"));
    log_trace(trace_logger, "PATH_FCB Cargada Correctamente: %s", cfg_filesystem->PATH_FCB);

    cfg_filesystem->RETARDO_ACCESO_BLOQUE = config_get_int_value(file_cfg_filesystem, "RETARDO_ACCESO_BLOQUE");
    log_trace(trace_logger, "RETARDO_ACCESO_BLOQUE Cargada Correctamente: %d",
             cfg_filesystem->RETARDO_ACCESO_BLOQUE);




    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");

    config_destroy(file_cfg_filesystem);
    return true;
}



bool crearEstructurasAdministrativas(){
    bool comp1 = crearSemaforos();
    bool comp2 = crearBitmapBloques();
    bool comp3 = crearSuperbloque();
    bool comp4 = crearArchivoBloques();
    bool comp5 = recorrerDirectorioFcb();
    return comp1 && comp2 && comp3 && comp4 && comp5;
}

bool crearSemaforos(){
    semaforosCreados = true;
    return true;
}


bool crearBitmapBloques(){
    //mmap()
    return true;
}
bool crearSuperbloque(){
    return true;
}
bool crearArchivoBloques(){
    return true;
}
bool recorrerDirectorioFcb(){
    return true;
}




bool iniciarFilesystem(){
    //TODO Si hiciera falta que exclusivamente tiene que generarse las conexiones antes que las estructuras, acá es donde hay que tocar.
    bool estructurasAdministrativas = crearEstructurasAdministrativas();
    if (!generar_conexiones()){
        //cerrar_programa();
        return false;
    }
    return estructurasAdministrativas;
}
