//
// Created by utnso on 4/7/23.
//

#include <init.h>
#include <sys/mman.h>
#include <dirent.h>

void* bitarraycontent;
t_bitarray* bitmap;
t_config_superbloque* cfg_superbloque;
t_bloques* archivoBloques;
t_list* lista_FCBs;
t_list* archivosUsados;


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
    reasignarPathSiEsNecesario(PATH_SUPERBLOQUE);
    cfg_filesystem->PATH_BITMAP = strdup(config_get_string_value(file_cfg_filesystem, "PATH_BITMAP"));
    log_trace(trace_logger, "PATH_BITMAP Cargada Correctamente: %s", cfg_filesystem->PATH_BITMAP);
    reasignarPathSiEsNecesario(PATH_BITMAP);
    cfg_filesystem->PATH_BLOQUES = strdup(config_get_string_value(file_cfg_filesystem, "PATH_BLOQUES"));
    log_trace(trace_logger, "PATH_BLOQUES Cargada Correctamente: %s", cfg_filesystem->PATH_BLOQUES);
    reasignarPathSiEsNecesario(PATH_BLOQUES);
    cfg_filesystem->PATH_FCB = strdup(config_get_string_value(file_cfg_filesystem, "PATH_FCB"));
    log_trace(trace_logger, "PATH_FCB Cargada Correctamente: %s", cfg_filesystem->PATH_FCB);
    reasignarPathSiEsNecesario(PATH_FCB);
    cfg_filesystem->RETARDO_ACCESO_BLOQUE = config_get_int_value(file_cfg_filesystem, "RETARDO_ACCESO_BLOQUE");
    log_trace(trace_logger, "RETARDO_ACCESO_BLOQUE Cargada Correctamente: %d", cfg_filesystem->RETARDO_ACCESO_BLOQUE);

    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");

    config_destroy(file_cfg_filesystem);
    return true;
}

bool comprobarSimbolo(char** path){
    if ((*path[0]) == '~') {
        char expandedPath[1024];
        strcpy(expandedPath, "/home/utnso");
        strcat(expandedPath, *path + 1);

        *path = malloc(sizeof(char) * (strlen(expandedPath) + 1));  // Asignar nueva memoria según el tamaño de expandedPath
        strcpy(*path, expandedPath);
        log_debug(debug_logger,"Fue necesario cambiar el path: %s",*path);
        return true;
    }
    return false;
}

void reasignarPathSiEsNecesario(tipo_path tipo){
    char* pathAux;
    char* nuevoPath;
    switch (tipo) {
        case PATH_BITMAP:
            pathAux = cfg_filesystem->PATH_BITMAP;
            if(comprobarSimbolo(&pathAux)){
                free(cfg_filesystem->PATH_BITMAP);
                cfg_filesystem->PATH_BITMAP= pathAux;
            }
            break;
        case PATH_BLOQUES:
            pathAux = cfg_filesystem->PATH_BLOQUES;
            if(comprobarSimbolo(&pathAux)){
                free(cfg_filesystem->PATH_BLOQUES);
                cfg_filesystem->PATH_BLOQUES= pathAux;
            }
            break;
        case PATH_SUPERBLOQUE:
            pathAux = cfg_filesystem->PATH_SUPERBLOQUE;
            if(comprobarSimbolo(&pathAux)){
                free(cfg_filesystem->PATH_SUPERBLOQUE);
                cfg_filesystem->PATH_SUPERBLOQUE= pathAux;
            }
            break;
        case PATH_FCB:
            pathAux = cfg_filesystem->PATH_FCB;
            if(comprobarSimbolo(&pathAux)){
                free(cfg_filesystem->PATH_FCB);
                cfg_filesystem->PATH_FCB= pathAux;
            }
            break;
        default:
            log_error(error_logger,"Tipo de PATH no reconocido, %d",tipo);
            break;

    }
}

bool iniciarFilesystem(){
    //TODO Si hiciera falta que exclusivamente tiene que generarse las conexiones antes que las estructuras, acá es donde hay que tocar.
    bool estructurasAdministrativas = iniciarEstructurasAdministrativas(cfg_filesystem->PATH_FCB);
    crearSemaforos();
    lista_FCBs = list_create();
    archivosUsados = list_create();
    lista_bloques = list_create();

    if (!generar_conexiones()){
        //cerrar_programa();
        return false;
    }



    return estructurasAdministrativas;
}

bool iniciarEstructurasAdministrativas(char* nombre_path) {

    if (!esDirectorio(nombre_path)){
        log_info(info_logger, "crear directorio %s", nombre_path);
        mkdir(nombre_path,0777);
        archivoBloques = malloc(sizeof(t_bloques));

        levantarSuperbloque();

        return crearEstructuras();
    } else {
        return levantarArchivosExistentes();
    }
}



bool crearEstructuras(){

    bool comp1 = crear_bitmap_de_bloques();
    bool comp2 = crear_archivo_de_bloques();

    crear_fcbs_del_directorio();

    return comp1 && comp2;

}

void crear_fcbs_del_directorio() {

	DIR *d;
	struct dirent *dir;
	d = opendir(cfg_filesystem->PATH_FCB);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if( strcmp( dir->d_name, "." ) != 0 && strcmp( dir->d_name, ".." ) != 0 ){

                t_config_fcb* aux_FCB = malloc(sizeof(t_config_fcb));

                char* nombre =  string_duplicate(dir->d_name);

                char* path_fcb_config = string_new();
                string_append(&path_fcb_config, cfg_filesystem->PATH_FCB);
                string_append(&path_fcb_config, "/");
                string_append(&path_fcb_config, nombre);

                t_config* aux_config = config_create(path_fcb_config);

                int tamanio_nombre_archivo = strlen( config_get_string_value(aux_config, "NOMBRE_ARCHIVO") );

                aux_FCB->NOMBRE_ARCHIVO = malloc( tamanio_nombre_archivo + 1  );
                strcpy( aux_FCB->NOMBRE_ARCHIVO, config_get_string_value(aux_config, "NOMBRE_ARCHIVO") );

                aux_FCB->TAMANIO_ARCHIVO = config_get_int_value(aux_config, "TAMANIO_ARCHIVO");
                aux_FCB->PUNTERO_DIRECTO = config_get_int_value(aux_config, "PUNTERO_DIRECTO");
                aux_FCB->PUNTERO_INDIRECTO = config_get_int_value(aux_config, "PUNTERO_INDIRECTO");

                aux_FCB->fcb_config = aux_config;

                free(path_fcb_config);

			    list_add(lista_FCBs,aux_FCB);

			}
		}
		closedir(d);
	}
}



int esDirectorio(const char* nombre){
    DIR* directorio = opendir(nombre);

    if(directorio != NULL){
     closedir(directorio);
     return 1;
    } else {
    	return 0;
    }
}

bool crear_bitmap_de_bloques(){

    int tamanio_bitmap = obtener_tamanio_en_bytes();

    int fd = open(cfg_filesystem->PATH_BITMAP, O_RDWR|O_CREAT,  S_IRUSR|S_IWUSR);
	if (fd == -1){
        log_error(error_logger,"Error al abrir/crear el archivo bitmap de bloques");
        return false;
	}else{
        log_debug(debug_logger,"Se creo el archivo bitmap de bloques");
    }

	ftruncate( fd, tamanio_bitmap);
    bitarraycontent = mmap(NULL, tamanio_bitmap, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	 if(bitarraycontent == MAP_FAILED)  {
         log_error(error_logger,"Error al mapear el archivo bitmap de bloques en memoria");
         close(fd);
         return false;
     }

    bitmap =  bitarray_create_with_mode(bitarraycontent,  tamanio_bitmap, LSB_FIRST);

     for (int i = 0; i < cfg_superbloque->BLOCK_COUNT; i++) {
        bitarray_clean_bit(bitmap, i);
     }

    msync(bitmap,  tamanio_bitmap, MS_SYNC);

    return true;

}

bool crear_archivo_de_bloques(){

    int blockCount = cfg_superbloque->BLOCK_COUNT;
    int blockSize = cfg_superbloque->BLOCK_SIZE;
    archivoBloques->tamanio = blockCount*blockSize;

    archivoBloques->fd = open(cfg_filesystem->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (archivoBloques->fd == -1){
        log_error(error_logger,"Error al abrir/crear el archivo de bloques");
        return false;
	}else{
        log_debug(debug_logger,"Se creo el archivo de bloques");
    }

	ftruncate(archivoBloques->fd, archivoBloques->tamanio);
    archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

    if(archivoBloques->archivo == MAP_FAILED) {
       log_error(error_logger,"Error al mapear el archivo en memoria");
       close(archivoBloques->fd);
       return false;
    }

    msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
    return true;
}

bool levantarArchivosExistentes() {
    return levantarSuperbloque() && levantarArchivoBloques() &&  levantarBitmapBloques();
}

bool levantarSuperbloque(){

    t_config *superBloqueConfig = config_create(cfg_filesystem->PATH_SUPERBLOQUE);
    if(superBloqueConfig == NULL){
        log_debug(debug_logger,"No se pudo crear la config para el superbloque, se usara el default");
        log_debug(debug_logger,"BLOCK_SIZE=64; BLOCK_COUNT=65536");
        cfg_superbloque = malloc(sizeof(t_config_superbloque));
        cfg_superbloque->BLOCK_COUNT=65536;
        cfg_superbloque->BLOCK_SIZE= 64;
        return true;
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


bool levantarArchivoBloques(){

    archivoBloques = malloc(sizeof (t_bloques));
    archivoBloques->fd = open(cfg_filesystem->PATH_BLOQUES, O_RDWR , 0777);
    int tamanio = cfg_superbloque->BLOCK_COUNT*cfg_superbloque->BLOCK_SIZE;
    archivoBloques->archivo = mmap(NULL, tamanio, PROT_WRITE|PROT_READ, MAP_SHARED, archivoBloques->fd, 0);

    if (archivoBloques->archivo  == MAP_FAILED) {
        perror("Error al mapear el archivo de bloques en memoria\n");
        close( archivoBloques->fd);
        return false;
    }
    log_debug(debug_logger,"Se levanto el archivo de bloques");
    return true;
}

bool levantarBitmapBloques(){

    int fd = open(cfg_filesystem->PATH_BITMAP,  O_RDWR, 0777);
    int tamanio = obtener_tamanio_en_bytes();
    bitarraycontent = mmap(0,tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    bitmap =  bitarray_create_with_mode(bitarraycontent, tamanio, LSB_FIRST);

    if (bitarraycontent == MAP_FAILED) {
            perror("Error al mapear el archivo en memoria\n");
            close(fd);
            return false;
    }

    log_debug(debug_logger,"Se levanto el archivo bitmap de bloques");
    return true;
}

int obtener_tamanio_en_bytes(){

    if(cfg_superbloque->BLOCK_COUNT%8 == 0){
        return cfg_superbloque->BLOCK_COUNT/8;
    }else {
        return cfg_superbloque->BLOCK_COUNT/8 + 1;
    }
}

bool crearSemaforos(){
    pthread_mutex_init(&mutex_cliente_socket,NULL);
    int comprobacionArchivosUsados = pthread_mutex_init(&mutex_ArchivosUsados,NULL);
    if(comprobacionArchivosUsados !=0){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }

    semaforosCreados = true;
    return true;
}