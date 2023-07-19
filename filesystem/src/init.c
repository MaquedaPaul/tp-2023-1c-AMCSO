//
// Created by utnso on 4/7/23.
//

#include <init.h>
#include <sys/mman.h>

void* bloques;
void* puntero_al_bitmap;
t_bitarray* bitmap;


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




void crear_fcbs_del_directorio() {

	DIR *d;
	struct dirent *dir;
	d = opendir(punto_PATH_FCB);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if( strcmp( dir->d_name, "." ) != 0 && strcmp( dir->d_name, ".." ) != 0 ){

           t_config_fcb* aux_FCB = malloc(sizeof(t_config_fcb));

            char* nombre =  string_duplicate(dir->d_name);

            char* path_fcb_config = string_new();
            string_append(&path_fcb_config, punto_PATH_FCB);
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

			list_add(lista_fcbs,aux_FCB);

			}
		}
		closedir(d);
	}

}


void iniciar_filesystem(char* nombre_path) {


if (!esDirectorio(nombre_path)){
    printf("crear directorio %s\n", nombre_path);

	 	mkdir(nombre_path,0777);
		crear_bitmap_de_bloques();
		crear_archivo_de_bloques();

	} else {

		levantar_superbloque_existente();

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



void crear_bitmap_de_bloques(){

    int tamanio_bitmap = obtener_tamanio_en_bytes();

	int fd = open(cfg_filesystem->PATH_BITMAP, O_CREAT| O_RDWR, 0777);
	if (fd < 0){
		  printf("Error al abrir/crear el archivo");
		  exit(1);
	}

	ftruncate(fd, tamanio_bitmap);

	puntero_al_bitmap = mmap(NULL,tamanio_bitmap, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

	 if(puntero_al_bitmap == MAP_FAILED)  {
			printf("Error en el mmap del bitmap");
			exit(1);
		  }

    bitmap =  bitarray_create_with_mode(puntero_al_bitmap, tamanio_bitmap , LSB_FIRST);

        for (int i = 0; i < bLOCK_COUNT; i++) {
        bitarray_clean_bit(bitmap, i); 
    }

    
     msync(puntero_al_bitmap, tamanio_bitmap, MS_SYNC);

}

void crear_archivo_de_bloques(){

    int tamanio_archivo_bloques = bLOCK_COUNT*bLOCK_SIZE;

	int fd_bloques = open(cfg_filesystem->PATH_BLOQUES, O_CREAT| O_RDWR, 0777);
	if (fd_bloques < 0){
		  printf("Error al abrir/crear el archivo");
		  exit(1);
	}

	ftruncate(fd_bloques, tamanio_archivo_bloques);

	bloques = mmap(NULL,tamanio_archivo_bloques, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloques, 0); 

	 if(bloques == MAP_FAILED) {
			printf("Error en el mmap del archivo de bloques");
			exit(1);
		  }

     msync(bloques, tamanio_archivo_bloques, MS_SYNC);

}



void levantar_superbloque_existente() {

	//--------------------------------------------------------------------------

    int tamanio_bitmap = obtener_tamanio_en_bytes();

	fd = open(cfg_filesystem->PATH_BITMAP,  O_RDWR, 0777);

	puntero_al_bitmap = mmap(0,tamanio_bitmap, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	bitmap =  bitarray_create_with_mode(puntero_al_bitmap, tamanio_bitmap, LSB_FIRST);

	//-------------------------BLOCKS-----------------------------------------

	fd_bloques = open(cfg_filesystem->PATH_BLOQUES, O_RDWR , 0777);

	bloques = mmap(NULL, cfg_superbloque->BLOCK_COUNT*cfg_superbloque->BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloques, 0);

}
