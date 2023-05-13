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
    bitmapDeBloques = malloc(sizeof(t_bitmapBloques));
    char* path =cfg_filesystem->PATH_BLOQUES;
    if((bitmapDeBloques->fd=open(path,O_RDWR,S_IRUSR|S_IWUSR))==-1){
        char* a;
            perror(&a);
            log_error(error_logger,"Error al abrir el bitmap de bloques");
            return false;
    }

    off_t file_size = lseek(bitmapDeBloques->fd, 0, SEEK_END); // Mueve el puntero al final del archivo
    if (file_size == -1) {
        perror("Error al obtener el tamaño del archivo");
        close(bitmapDeBloques->fd);
        return false;
    }

    if (lseek(bitmapDeBloques->fd, 0, SEEK_SET) == -1) { // Mueve el puntero al principio del archivo
        perror("Error al volver al principio del archivo");
        close(bitmapDeBloques->fd);
        return false;
    }

    char* unBitArray = malloc(file_size); //TODO

    bitarrayBitmapDeBloques = bitarray_create(unBitArray,file_size);
    bitmapDeBloques->archivo= mmap(bitarrayBitmapDeBloques->bitarray, file_size,PROT_WRITE|PROT_READ, MAP_SHARED,bitmapDeBloques->fd,0 );
    if(bitmapDeBloques->archivo != bitarrayBitmapDeBloques->bitarray){
        log_warning(warning_logger, "Se ignoro la direccion sugerida para mmap");
        return false;
    }



    if (bitmapDeBloques->archivo == MAP_FAILED) {
        perror("Error al mapear el archivo en memoria");
        close(bitmapDeBloques->fd);
        return false;
    }

    return true;
}
bool crearSuperbloque(){
    t_config *superBloqueConfig = config_create(cfg_filesystem->PATH_SUPERBLOQUE);
    if(superBloqueConfig == NULL){
        log_error(error_logger,"No se pudo crear la config para el superbloque");
        return false;
    }
    cfg_superbloque = malloc(sizeof(t_config_superbloque));
    if(cfg_superbloque == NULL){
        log_error(error_logger,"No se pudo crear la cfg para el superbloque");
        return false;
    }
    cfg_superbloque->BLOCK_COUNT= config_get_int_value(superBloqueConfig,"BLOCK_COUNT");
    cfg_superbloque->BLOCK_SIZE= config_get_int_value(superBloqueConfig,"BLOCK_SIZE");
    return true;
}
bool crearArchivoBloques(){
    if((archivoBloques->fd=open(cfg_filesystem->PATH_BLOQUES,O_RDWR,S_IRUSR|S_IWUSR))==-1){
        char* a;
        perror(&a);
        log_error(error_logger,"Error al abrir el archivo de bloques");
        return false;
    }

    off_t file_size = lseek(archivoBloques->fd, 0, SEEK_END); // Mueve el puntero al final del archivo
    if (file_size == -1) {
        perror("Error al obtener el tamaño del archivo");
        close(archivoBloques->fd);
        return false;
    }

    if (lseek(archivoBloques->fd, 0, SEEK_SET) == -1) { // Mueve el puntero al principio del archivo
        perror("Error al volver al principio del archivo");
        close(archivoBloques->fd);
        return false;
    }
    archivoBloques->archivo= mmap(NULL, file_size,PROT_WRITE|PROT_READ, MAP_SHARED,archivoBloques->fd,0 );



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
