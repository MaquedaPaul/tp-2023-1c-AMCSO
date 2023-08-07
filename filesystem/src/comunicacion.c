//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>

int fd_filesystem;
char* ip_filesystem;
char* puerto_filesystem;
pthread_t crear_server_filesystem;

t_list* peticiones_pendientes;
bool conexionesHechas = false;
sem_t contador_peticiones;
pthread_mutex_t mutex_peticiones_pendientes;
pthread_mutex_t mutex_ArchivosUsados;
pthread_mutex_t mutex_recv;
bool kernelInicializado = false;
t_list* archivosUsados;

void procesar_conexion(void *void_args) {
    t_procesar_conexion_args_fs *args = (t_procesar_conexion_args_fs *) void_args;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    pthread_mutex_t mutex_recv = args->mutex_procesar;
    free(args);
    op_code cop;

    while (cliente_socket != -1) {
        pthread_mutex_lock(&mutex_recv);

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(info_logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case DEBUG:
                log_info(info_logger, "debug");
                pthread_mutex_unlock(&mutex_recv);
                break;
            case HANDSHAKE_FS: {
                recibirOrden(cliente_socket);
                fd_memoria = cliente_socket;
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case APERTURA_ARCHIVO: {
                pthread_t abrirArchivoHilo;

                pthread_create(&abrirArchivoHilo, NULL, (void *) abrirArchivo, &cliente_socket);

                pthread_join(abrirArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case CREACION_ARCHIVO:{
                pthread_t crearArchivoHilo;
                pthread_create(&crearArchivoHilo, NULL, (void *) crearArchivo, &cliente_socket);
                pthread_join(crearArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case TRUNCACION_ARCHIVO: {
                pthread_t truncarArchivoHilo;
                pthread_create(&truncarArchivoHilo, NULL, (void *) truncarArchivo, &cliente_socket);
                pthread_join(truncarArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case LECTURA_ARCHIVO:{
                pthread_t leerArchivoHilo;
                pthread_create(&leerArchivoHilo, NULL, (void *) leerArchivo, &cliente_socket);
                pthread_join(leerArchivoHilo,NULL);
                pthread_mutex_unlock(&mutex_recv);

                break;
            }
            case LECTURA_REALIZADA: {
                pthread_t finalizarEscrituraArchivoHilo;
                pthread_create(&finalizarEscrituraArchivoHilo, NULL, (void *) finalizarEscrituraArchivo, &cliente_socket);
                pthread_join(finalizarEscrituraArchivoHilo,NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case ESCRITURA_ARCHIVO:{
                pthread_t escribirArchivoHilo;
                pthread_create(&escribirArchivoHilo, NULL, (void *) escribirArchivo, &cliente_socket);
                pthread_join(escribirArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case ESCRITURA_REALIZADA:{
                pthread_t finalizarLecturaArchivoHilo;
                pthread_create(&finalizarLecturaArchivoHilo, NULL, (void *) finalizarLecturaArchivo, &cliente_socket);
                pthread_join(finalizarLecturaArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
            break;
            }
            case -1:
                log_error(error_logger, "Cliente desconectado de %s...", server_name);
                pthread_mutex_unlock(&mutex_recv);
                return;
            default:
                log_error(error_logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(info_logger, "Cop: %d", cop);
                pthread_mutex_unlock(&mutex_recv);
                return;
        }
    }

    log_warning(warning_logger, "El cliente se desconecto de %s server", server_name);
    return;
}


void procesar_conexion2(void *void_args) {
    t_procesar_conexion_args_fs *args = (t_procesar_conexion_args_fs *) void_args;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    pthread_mutex_t mutex_recv = args->mutex_procesar;
    free(args);
    op_code cop;

    while (cliente_socket != -1) {
        pthread_mutex_lock(&mutex_recv);

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(info_logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case DEBUG:
                log_info(info_logger, "debug");
                pthread_mutex_unlock(&mutex_recv);
                break;
            case HANDSHAKE_FS: {
                recibirOrden(cliente_socket);
                fd_memoria = cliente_socket;
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case APERTURA_ARCHIVO: {
                pthread_t abrirArchivoHilo;

                pthread_create(&abrirArchivoHilo, NULL, (void *) abrirArchivo, &cliente_socket);

                pthread_join(abrirArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case CREACION_ARCHIVO:{
                pthread_t crearArchivoHilo;
                pthread_create(&crearArchivoHilo, NULL, (void *) crearArchivo, &cliente_socket);
                pthread_join(crearArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case TRUNCACION_ARCHIVO: {
                pthread_t truncarArchivoHilo;
                pthread_create(&truncarArchivoHilo, NULL, (void *) truncarArchivo, &cliente_socket);
                pthread_join(truncarArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case LECTURA_ARCHIVO:{
                pthread_t leerArchivoHilo;
                pthread_create(&leerArchivoHilo, NULL, (void *) leerArchivo, &cliente_socket);
                pthread_join(leerArchivoHilo,NULL);
                pthread_mutex_unlock(&mutex_recv);

                break;
            }
            case LECTURA_REALIZADA: {
                pthread_t finalizarEscrituraArchivoHilo;
                pthread_create(&finalizarEscrituraArchivoHilo, NULL, (void *) finalizarEscrituraArchivo, &cliente_socket);
                pthread_join(finalizarEscrituraArchivoHilo,NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case ESCRITURA_ARCHIVO:{
                pthread_t escribirArchivoHilo;
                pthread_create(&escribirArchivoHilo, NULL, (void *) escribirArchivo, &cliente_socket);
                pthread_join(escribirArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case ESCRITURA_REALIZADA:{
                pthread_t finalizarLecturaArchivoHilo;
                pthread_create(&finalizarLecturaArchivoHilo, NULL, (void *) finalizarLecturaArchivo, &cliente_socket);
                pthread_join(finalizarLecturaArchivoHilo, NULL);
                pthread_mutex_unlock(&mutex_recv);
                break;
            }
            case -1:
                log_error(error_logger, "Cliente desconectado de %s...", server_name);
                pthread_mutex_unlock(&mutex_recv);
                return;
            default:
                log_error(error_logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(info_logger, "Cop: %d", cop);
                pthread_mutex_unlock(&mutex_recv);
                return;
        }
    }

    log_warning(warning_logger, "El cliente se desconecto de %s server", server_name);
    return;
}


int server_escuchar(t_log *logger, char *server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t atenderConexionNueva;
        t_procesar_conexion_args_fs *args = malloc(sizeof(t_procesar_conexion_args_fs));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_mutex_init(&args->mutex_procesar, NULL);
        pthread_create(&atenderConexionNueva, NULL,(void* )procesar_conexion,args);
        pthread_detach(atenderConexionNueva);
        return 1;
    }
    return 0;
}

bool generar_conexiones(){
    pthread_t conexion_con_memoria;

    pthread_create(&crear_server_filesystem,NULL, crearServidor,NULL);
    pthread_create(&conexion_con_memoria, NULL, (void*)conectarConMemoria, NULL);
    conexionesHechas = true;

    pthread_join(crear_server_filesystem, NULL);
    pthread_join(conexion_con_memoria, NULL);

    return true; //debe tomarse lo que retorna el hilo al crear el servidor
}


void* crearServidor(){
    puerto_filesystem =cfg_filesystem->PUERTO_ESCUCHA;
    fd_filesystem = iniciar_servidor(info_logger, "SERVER FILESYSTEM", ip_filesystem,puerto_filesystem);

    if (fd_filesystem == 0) {
        log_error(error_logger, "Fallo al crear el servidor FILESYSTEM, cerrando FILESYSTEM");
        return (void*)EXIT_FAILURE;
    }

    while (server_escuchar(info_logger, "SERVER FILESYSTEM", fd_filesystem));
}


void* conectarConMemoria(){
    bool comprobacion = generarConexionesConMemoria();
    if(comprobacion){
        enviarOrden(HANDSHAKE_FS, fd_memoria, info_logger);
        atenderMemoria();
    }


}

bool generarConexionesConMemoria(){
    char* ip;

    ip = strdup(cfg_filesystem->IP_MEMORIA);
    log_trace(trace_logger,"Lei la IP [%s]", ip);

    char* puerto;
    puerto = strdup(cfg_filesystem->PUERTO_MEMORIA);

    log_trace(trace_logger,"Lei el PUERTO [%s]", puerto);

    fd_memoria = crear_conexion(
            info_logger,
            "SERVER MEMORIA",
            ip,
            puerto
    );


    free(ip);
    free(puerto);

    return fd_memoria != 0;

}


bool atenderMemoria(){
    if (fd_memoria == -1){
        return EXIT_FAILURE;
    }
    pthread_t atenderMemoria;
    t_procesar_conexion_args_fs *args = malloc(sizeof(t_procesar_conexion_args_fs));
    args->fd = fd_memoria;
    args->server_name = "ATENDER_MEMORIA";
    pthread_mutex_init(&args->mutex_procesar, NULL);
    pthread_create(&atenderMemoria, NULL,(void*)procesar_conexion2,args);
    pthread_detach(atenderMemoria);
    return true;
}

//MANEJO DE PETICIONES
void* abrirArchivo(void* cliente_socket){

    int conexion = *((int*) cliente_socket);
    char* nombreArchivo = recibirString(conexion);
    //log_debug(debug_logger, "Me llego este String: %s",nombreArchivo);
    if(kernelInicializado == false){
        fd_kernel= conexion;
        kernelInicializado = true;
    }

    t_peticion* peticion_open = crear_peticion(EJECUTAR_OPEN,nombreArchivo, 0, 0, 0, 0, NULL);
    agregarPeticionAPendientes(peticion_open);
    sem_post(&contador_peticiones);
}

void* crearArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    char* nombreArchivo = recibirString(conexion);
    //log_debug(debug_logger,"nombre archivo: %s", nombreArchivo);

    t_peticion* peticion_open = crear_peticion(EJECUTAR_CREACION,nombreArchivo, 0, 0, 0, 0, NULL);
    agregarPeticionAPendientes(peticion_open);
    sem_post(&contador_peticiones);

}


void* truncarArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_archivoTruncate* archivoTruncacion = recibir_archivoTruncacion(conexion);
    t_peticion* peticion_truncar = crear_peticion(EJECUTAR_TRUNCATE,archivoTruncacion->nombreArchivo, archivoTruncacion->nuevoTamanio, 0, 0, 0, NULL);
    agregarPeticionAPendientes(peticion_truncar);
    sem_post(&contador_peticiones);
}

void* leerArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_archivoRW* archivo = recibir_archivoRW(conexion);
    lecturaArchivo(archivo->nombreArchivo,archivo->posPuntero,archivo->direcFisica, archivo->cantidadBytes);

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = buscarFCBporNombre(archivo->nombreArchivo); //solo agrego a lista los archivos que se lee o escribe
    list_add(archivosUsados, fcb);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    t_peticion* peticion_read = crear_peticion(EJECUTAR_READ,archivo->nombreArchivo, archivo->cantidadBytes, archivo->direcFisica, archivo->posPuntero, archivo->pid, NULL);
    agregarPeticionAPendientes(peticion_read);
    sem_post(&contador_peticiones);
}

void* escribirArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    //escrituraArchivo(nombreArchivo, punteroArchivo, direccionFisica, unosDatos->tamanio);
    t_archivoRW* archivo = recibir_archivoRW(conexion);

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb =buscarFCBporNombre(archivo->nombreArchivo);
    list_add(archivosUsados, fcb); //solo agrego a lista los archivos que se lee o escribe
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    t_peticion* peticion_write = crear_peticion(EJECUTAR_WRITE,archivo->nombreArchivo, archivo->cantidadBytes, archivo->direcFisica, archivo->posPuntero, archivo->pid, NULL);
    agregarPeticionAPendientes(peticion_write);
    sem_post(&contador_peticiones);

}

void* finalizarEscrituraArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(conexion, unosDatos);
    uint32_t direccionFisica = *(uint32_t*)list_get(listaInts,0);
    //uint32_t tamanio = *(uint32_t*)list_get(listaInts,1);
    //uint32_t pid = *(uint32_t*)list_get(listaInts,2);
    uint32_t punteroArchivo = *(uint32_t*)list_get(listaInts,3);

    char* nombreArchivo = obtenerPrimerArchivoUsado();
    t_peticion* peticion_write = crear_peticion(EJECUTAR_RESPUESTA_LECTURA,nombreArchivo, unosDatos->tamanio, direccionFisica, punteroArchivo, 0, unosDatos->datos);
    agregarPeticionAPendientes(peticion_write);
    sem_post(&contador_peticiones);

}


void* finalizarLecturaArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    char* nombreArchivo = obtenerPrimerArchivoUsado();
    recibirOrden(conexion);
    t_peticion* peticion_write = crear_peticion(EJECUTAR_RESPUESTA_ESCRITURA,nombreArchivo, 0, 0, 0, 0, NULL);
    agregarPeticionAPendientes(peticion_write);
    sem_post(&contador_peticiones);
}


char* obtenerPrimerArchivoUsado() {

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = list_remove(archivosUsados, 0);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    return fcb->NOMBRE_ARCHIVO;
}


void iniciar_atencion_peticiones(){
    pthread_t hilo_peticiones;
    log_info(info_logger, "Inicio atencion de peticiones");

    pthread_create(&hilo_peticiones, NULL, (void*) atender_peticiones, NULL);
    pthread_detach(hilo_peticiones);
}

void atender_peticiones(){
    while(1){
        sem_wait(&contador_peticiones);
        log_info(info_logger, "Hay peticion pendiente");
        t_peticion* peticion = sacoPeticionDePendientes();
        manejar_peticion(peticion);
    }
}

t_peticion* crear_peticion(t_operacion_fs operacion, char* nombre, uint32_t tamanio, uint32_t dir_fisica, uint32_t puntero, uint32_t pid, void* datos){
    t_peticion* peticion = malloc(sizeof(t_peticion));
    peticion->nombre = string_new();
    string_append(&peticion->nombre, nombre);
    peticion->operacion = operacion;
    //strcpy(peticion->nombre, nombre);
    peticion->tamanio = tamanio;
    peticion->direccionFisica = dir_fisica;
    peticion->puntero = puntero;
    peticion->pid = pid;
    peticion->datos = datos;
    return peticion;
}

void agregarPeticionAPendientes(t_peticion* peticion){
    pthread_mutex_lock(&mutex_peticiones_pendientes);
    list_add(peticiones_pendientes, peticion);
    pthread_mutex_unlock(&mutex_peticiones_pendientes);
}

t_peticion* sacoPeticionDePendientes(){
    pthread_mutex_lock(&mutex_peticiones_pendientes);
    t_peticion* peticion = list_remove(peticiones_pendientes, 0);
    pthread_mutex_unlock(&mutex_peticiones_pendientes);
    return peticion;
}


void manejar_peticion(t_peticion* peticion){
    t_operacion_fs cop = peticion->operacion;

    switch (cop) {
        case EJECUTAR_OPEN:
            ejecutar_fopen(peticion->nombre);
            break;
        case EJECUTAR_CREACION:
            ejecutarCreacionArchivo(peticion->nombre); //hacer otra funcion parecida a las del filesystem.c
            break;
        case EJECUTAR_READ:
            ejecutarFread(peticion->nombre, peticion->pid, peticion->puntero, peticion->tamanio, peticion->direccionFisica);
            break;
        case EJECUTAR_TRUNCATE:
            ejecutarFtruncate(peticion->nombre, peticion->tamanio);
            break;
        case EJECUTAR_WRITE:
            ejecutarFwrite(peticion->nombre, peticion->pid, peticion->puntero, peticion->tamanio, peticion->direccionFisica);
            break;
        case EJECUTAR_RESPUESTA_LECTURA:
            ejecutar_finalizarEscrituraArchivo(peticion->nombre, peticion->puntero, peticion->tamanio, peticion->direccionFisica, peticion->datos);
            break;
        case EJECUTAR_RESPUESTA_ESCRITURA:
            ejecutar_finalizarLecturaArchivo(peticion->nombre);
            break;
        default:
            log_error(error_logger, "Algo anduvo mal");
            log_info(info_logger, "Cop: %d", cop);
            return;
    }
}

