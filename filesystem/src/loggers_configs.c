//
// Created by utnso on 4/9/23.
//

#include <loggers_configs.h>
t_log* trace_logger;
t_log* debug_logger;
t_log* info_logger;
t_log* warning_logger;
t_log* error_logger;
t_config* file_cfg_filesystem;
t_config_filesystem *cfg_filesystem;
char* path_config;
bool logsCreados = false;
bool configCreado = false;
bool cfgCreado = false;

int init_logs_configs(char *path_config) {
    cfg_filesystem = cfg_filesystem_start();
    if(cfg_filesystem == NULL){
        return false;
    }
    cfgCreado= true;
    trace_logger = log_create("trace_logger.log","Filesystem", true, LOG_LEVEL_TRACE);
    debug_logger = log_create("debug_logger.log","Filesystem", true, LOG_LEVEL_DEBUG);
    info_logger = log_create("info_logger.log","Filesystem", true, LOG_LEVEL_INFO);
    warning_logger = log_create("warning_logger.log","Filesystem", true, LOG_LEVEL_WARNING);
    error_logger = log_create("error_logger.log","Filesystem", true, LOG_LEVEL_ERROR);

    if(trace_logger== NULL || debug_logger== NULL || info_logger== NULL || warning_logger== NULL || error_logger== NULL){
        printf("No pude crear los loggers");
        return false;
    }
    logsCreados = true;
    file_cfg_filesystem = iniciar_config(path_config);
    if(file_cfg_filesystem == NULL){
        return false;
    }
    configCreado = true;
    return checkProperties(path_config);
}

t_config_filesystem *cfg_filesystem_start()
{
    t_config_filesystem *cfg = malloc(sizeof(t_config_filesystem));
    return cfg;
}

bool checkProperties(char *path) {

    t_config *config = config_create(path);
    if (config == NULL) {
        printf("No se pudo crear la config");
        return false;
    }


    char *properties[] = {
            "IP_MEMORIA",
            "PUERTO_MEMORIA",
            "PUERTO_ESCUCHA",
            "PATH_SUPERBLOQUE",
            "PATH_BITMAP",
            "PATH_BLOQUES",
            "PATH_FCB",
            "RETARDO_ACCESO_BLOQUE",
            NULL};

    // Falta alguna propiedad
    if (!config_has_all_properties(config, properties)) {
        log_error(error_logger, "Propiedades faltantes en el archivo de configuracion");
        return false;
    }

    config_destroy(config);

    return true;
}



void creacionArchivo(char* nombreArchivo){
    log_info(info_logger,"Crear Archivo: <%s>", nombreArchivo);
}
void aperturaArchivo(char* nombreArchivo){
    log_info(info_logger,"Abrir Archivo: <%s>", nombreArchivo);
}

void truncacionArchivo(char* nombreArchivo, uint32_t tamanioArchivo){
    log_info(info_logger,"Truncar Archivo: <%s> - Tamanio: <%d>", nombreArchivo,tamanioArchivo);
}

void accesoABitmap(uint32_t numeroBloque, uint8_t estado){
    log_info(info_logger,"Acceso a Bitmap - Bloque: <%d> - Estado: <%d>", numeroBloque,estado);
}

void lecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t direccionMemoria, uint32_t tamanioArchivo){
    log_info(info_logger,"Leer Archivo: <%s> - Puntero: <%d> - Memoria: <%d> - Tamanio: <%d>", nombreArchivo,punteroArchivo, direccionMemoria,tamanioArchivo);
}

void escrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t direccionMemoria, uint32_t tamanioArchivo){
    log_info(info_logger,"Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%d> - Tamanio: <%d>", nombreArchivo,punteroArchivo, direccionMemoria,tamanioArchivo);
}

void accesoABloqueArchivo(char* nombreArchivo, uint32_t numeroBloqueArchivo, uint32_t numeroBloqueFs){
    log_info(info_logger,"Acceso Bloque - Archivo: <NOMBRE_ARCHIVO> - Bloque Archivo: <NUMERO BLOQUE ARCHIVO> - Bloque File System <NUMERO BLOQUE FS>", nombreArchivo, numeroBloqueArchivo, numeroBloqueFs);
}
