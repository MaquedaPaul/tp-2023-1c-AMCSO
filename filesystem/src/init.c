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
    log_trace(trace_logger, "RETARDO_ACCESO_BLOQUE Cargada Correctamente: %d",
             cfg_filesystem->RETARDO_ACCESO_BLOQUE);


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


bool crearEstructurasAdministrativas(){
    bool comp1 = crearSemaforos();
    bool comp2 = levantarSuperbloque();
    bool comp3 = levantarBitmapBloques();
    bool comp4 = levantarArchivoBloques();
    bool comp5 = recorrerDirectorioFcb();
    return comp1 && comp2 && comp3 && comp4 && comp5;
}

bool crearSemaforos(){
    semaforosCreados = true;
    return true;
}

//LEVANTO SUPERBLOQUE, siempre esta creado de antemano

bool levantarSuperbloque(){
    t_config *superBloqueConfig = config_create(cfg_filesystem->PATH_SUPERBLOQUE);
    if(superBloqueConfig == NULL){
        log_debug(debug_logger,"No se pudo crear la config para el superbloque, se usara el default");
        log_debug(debug_logger,"BLOCK_SIZE=64; BLOCK_COUNT=65536");
        cfg_superbloque = malloc(sizeof(t_config_superbloque));
        cfg_superbloque->BLOCK_COUNT=65536;
        cfg_superbloque->BLOCK_SIZE=64;
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

//LEVANTO BITMAP DE BLOQUES

bool levantarBitmapBloques(){

    bitmapDeBloques = malloc(sizeof(t_bitmapBloques));
    struct stat file_st;
    char* path = cfg_filesystem->PATH_BITMAP;



    if (existeArchivo(&bitmapDeBloques->fd, path)){

        fstat(bitmapDeBloques->fd, &file_st);
        bitmapDeBloques->tamanio = file_st.st_size;
        char *bitarraycontent = mmap(NULL, bitmapDeBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, bitmapDeBloques->fd, 0);

        if (bitarraycontent == MAP_FAILED) {
            perror("Error al mapear el archivo en memoria\n");
            close(bitmapDeBloques->fd);
            return false;
        }

        bitarrayBitmapDeBloques = bitarray_create(bitarraycontent,bitmapDeBloques->fd);//t_bitarray*bitarrayBitmapDeBloques; definido en gestion_filesystem

        return true;

    }else{
        //si no existe el archivo ;
        return crearBitmapBloques();
    }
}

bool crearBitmapBloques(){

    char* path = cfg_filesystem->PATH_BITMAP;
    int fd_bitMapBloques;

    if((fd_bitMapBloques = open(path,O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) == -1){ 	//se crea el archivo con O_CREAT
        log_error(error_logger,"Error al crear el archivo bitmap de bloques");
        return false;
    };

    int tamaño_del_bitarray = obtener_tamanio_en_bytes(); //lo obtengo del superbloque
    ftruncate(fd_bitMapBloques, tamaño_del_bitarray);
    char *bitarraycontent = (char*) mmap(NULL, tamaño_del_bitarray, PROT_WRITE|PROT_READ, MAP_SHARED, fd_bitMapBloques, 0);

    if (bitarraycontent == MAP_FAILED) {
        perror("Error al mapear el archivo en memoria\n");
        close(fd_bitMapBloques);
        return false;
    }

    bitarrayBitmapDeBloques = bitarray_create(bitarraycontent, tamaño_del_bitarray);
    inicializarBitmap(tamaño_del_bitarray); //se escribe el bitmapDeBloques con 0 en cada posicion del array
    msync(bitarraycontent, fd_bitMapBloques, MS_SYNC); //se guarda los cambios del archivo

    return true;
}

bool existeArchivo(int* fd, char *path){
    if((*fd = open(path, O_RDWR, S_IRUSR|S_IWUSR))==-1){ //si existe el archivo, O_RDWR: se solicita la apertura del archivo en modo lectura/escritura
        log_warning(warning_logger,"El archivo no existe con path:%s , debera ser creado", path);
        return false;
    }else {
        return true;
    }
}



int existe_archivoFCB(char *nombre_archivo) {

    int tamanio_lista_FCBs = list_size(lista_fcbs);

    for (int i = 0; i < tamanio_lista_FCBs; i++)  {

        t_config_fcb *aux_FCB = list_get(lista_fcbs, i);

        if (strcmp(aux_FCB->nombre_archivo, nombre_archivo) == 0)  {

            log_info(info_logger, "Abrir archivo: <%s>", nombre_archivo);

            return 1;
        }
    }

    return 0;
}





int obtener_tamanio_en_bytes(){ //esto capaz va en otro archivo

    if(cfg_superbloque->BLOCK_COUNT%8 == 0){
        return cfg_superbloque->BLOCK_COUNT/8;
    }

    return cfg_superbloque->BLOCK_COUNT/8 + 1; //block_count = 10 .. 10/8 = 1 .. return 2 Bytes
}

void inicializarBitmap(int tamaño_del_bitarray){
    int i;
    for(i=0; i< tamaño_del_bitarray * 8; i++){
        bitarrayBitmapDeBloques->bitarray[i] = 0;
    }
}

//ARCHIVO DE BLOQUES

bool levantarArchivoBloques(){
    archivoBloques = malloc(sizeof(t_bloques)); //definido en gestion_filesytem.c
    struct stat file_st;

    if(existeArchivo(&archivoBloques->fd, cfg_filesystem->PATH_BLOQUES)){

        fstat(archivoBloques->fd, &file_st);
        int tamanio= file_st.st_size;
        archivoBloques->archivo= mmap(NULL, tamanio, PROT_WRITE|PROT_READ, MAP_SHARED, archivoBloques->fd, 0);

        return true;
    }

    return crearArchivoDeBloques();
}

bool crearArchivoDeBloques(){

    if((archivoBloques->fd = open(cfg_filesystem->PATH_BLOQUES, O_RDWR| O_CREAT, S_IRUSR|S_IWUSR))==-1){

        log_error(error_logger,"Error al crear el archivo de bloques");
        return false;
    }else {

        int tamanio = cfg_superbloque-> BLOCK_COUNT * cfg_superbloque->BLOCK_SIZE;
        ftruncate(archivoBloques->fd, tamanio);
        archivoBloques->archivo = mmap(NULL, tamanio, PROT_WRITE|PROT_READ, MAP_SHARED, archivoBloques->fd, 0); //devuelve un void*

        //aca el archivo por el ftrucate esta lleno de basura, ver si lo inizalizamos y como
        return true;
    }
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
