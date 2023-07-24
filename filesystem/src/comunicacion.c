//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>

int fd_filesystem;
char* ip_filesystem;
char* puerto_filesystem;
pthread_t crear_server_filesystem;

bool conexionesHechas = false;


void procesar_conexion(void *void_args) {
    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);
    op_code cop;

    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(info_logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case DEBUG:
                log_info(info_logger, "debug");
                break;
            case HANDSHAKE_FS:
                recibirOrden(cliente_socket);
                fd_memoria = cliente_socket;
                break;
            case APERTURA_ARCHIVO: {
                pthread_t abrirArchivoHilo;
                pthread_create(&abrirArchivoHilo, NULL, (void *) abrirArchivo, &cliente_socket);
                pthread_detach(abrirArchivoHilo);
                break;
            }
            case CREACION_ARCHIVO:{
                pthread_t crearArchivoHilo;
                pthread_create(&crearArchivoHilo, NULL, (void *) crearArchivo, &cliente_socket);
                pthread_detach(crearArchivoHilo);
                break;
            }
            case TRUNCACION_ARCHIVO: {
                pthread_t truncarArchivoHilo;
                pthread_create(&truncarArchivoHilo, NULL, (void *) truncarArchivo, &cliente_socket);
                pthread_detach(truncarArchivoHilo);
                break;
            }
            case LECTURA_ARCHIVO:{
                pthread_t leerArchivoHilo;
                pthread_create(&leerArchivoHilo, NULL, (void *) leerArchivo, &cliente_socket);
                pthread_detach(leerArchivoHilo);
                break;
            }
            case LECTURA_REALIZADA: {
                pthread_t finalizarEscrituraArchivoHilo;
                pthread_create(&finalizarEscrituraArchivoHilo, NULL, (void *) finalizarEscrituraArchivo, &cliente_socket);
                pthread_detach(finalizarEscrituraArchivoHilo);
                break;
            }
            case ESCRITURA_ARCHIVO:{
                pthread_t escribirArchivoHilo;
                pthread_create(&escribirArchivoHilo, NULL, (void *) escribirArchivo, &cliente_socket);
                pthread_detach(escribirArchivoHilo);
                break;
            }
            case ESCRITURA_REALIZADA:{
                pthread_t finalizarLecturaArchivoHilo;
                pthread_create(&finalizarLecturaArchivoHilo, NULL, (void *) finalizarLecturaArchivo, &cliente_socket);
                pthread_detach(finalizarLecturaArchivoHilo);
            break;
            }
            case -1:
                log_error(error_logger, "Cliente desconectado de %s...", server_name);
                return;
            default:
                log_error(error_logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(info_logger, "Cop: %d", cop);
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
        t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
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
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->fd = fd_memoria;
    args->server_name = "ATENDER_MEMORIA";
    pthread_create(&atenderMemoria, NULL,(void*)procesar_conexion,args);
    pthread_detach(atenderMemoria);
    return true;
}



