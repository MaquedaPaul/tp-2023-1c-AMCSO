//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>
char* ip_kernel;
char* puerto_kernel;
int fd_kernel;
int fd_memoria;
int fd_cpu;
int fd_filesystem;
void procesar_conexion(void *void_args) {
    t_procesar_conexion_args *args = (t_procesar_conexion_args *) void_args;
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_trace(trace_logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            //----------------------------------------CONSOLA-----------------------
            case DEBUG:
            {
                break;
            }
            case GESTIONAR_CONSOLA_NUEVA:
            {
                t_list* instrucciones = recibirListaInstrucciones(cliente_socket);
                break;

            }
            case 100:
            {

                break;
            }
            case 1000:
            {
                break;
            }

                //--------------------------------CPU-------------------------------------------

            case 10000:
            {

                break;
            }
            case 20:{
                break;
            }

            case 200:
            {
                break;
            }
            case 2000: {
                break;
            }
            case 20000: {
                break;
            }

            case 200000: {
                break;
            }

            case 30:
            {

                break;
            }

                //----------------------------------MEMORIA----------------------------------------
            case 300:
            {
                break;
            }

            case 3000:
            {
                break;
            }
            case 30000:
            {
                break;
            }
            case 300000:
            {
                break;
            }
            case 40:
            {
                break;
            }

            case -1:
                log_error(error_logger, "Cliente desconectado de %s...", server_name);
                return;
            default:
                log_error(error_logger, "Algo anduvo mal en el server de %s", server_name);
                log_trace(trace_logger, "Cop: %d", cop);
                return;
        }
    }
    log_warning(warning_logger, "El cliente se desconecto de %s server", server_name);
    return;
}

void* crearServidor(){
    puerto_kernel=cfg_kernel->PUERTO_ESCUCHA;
    fd_kernel = iniciar_servidor(info_logger, "SERVER KERNEL", ip_kernel, puerto_kernel);
    if (fd_kernel == 0) {
        log_error(error_logger, "Fallo al crear el servidor, cerrando KERNEL");
        return (void* )EXIT_FAILURE;
    }
    while (server_escuchar(info_logger, "SERVER KERNEL", (uint32_t)fd_kernel));
}

int server_escuchar(t_log *logger, char *server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t atenderProcesoNuevo;
        t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&atenderProcesoNuevo, NULL,(void*) procesar_conexion,args);
        pthread_detach(atenderProcesoNuevo);
        return 1;
    }
    return 0;
}


bool generar_conexiones(){
    pthread_t conexion_con_consola;
    pthread_t conexion_con_cpu;
    pthread_t conexion_con_memoria;
    pthread_t conexion_con_filesystem;
    pthread_create(&conexion_con_consola, NULL,(void*)crearServidor, NULL);
    pthread_create(&conexion_con_cpu, NULL, (void*)conectarConCPU, NULL);
    pthread_create(&conexion_con_memoria, NULL, (void*)conectarConMemoria, NULL);
    pthread_create(&conexion_con_filesystem, NULL, (void*)conectarConFileSystem, NULL);

    pthread_join(conexion_con_consola, NULL);
    pthread_join(conexion_con_cpu, NULL);
    pthread_join(conexion_con_memoria, NULL);
    pthread_join(conexion_con_filesystem, NULL);
    return true;
}


void* conectarConCPU(){
    int comprobacion = generarConexionesConCPU();

    if(comprobacion){
    atenderCpu();
     }


}

bool atenderCpu(){
    if (fd_cpu == -1){
        return EXIT_FAILURE;
    }
    pthread_t atenderDispatch;
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->fd = fd_cpu;
    args->server_name = "ATENDER_CPU";
    pthread_create(&atenderDispatch, NULL,(void*)procesar_conexion,args);
    pthread_detach(atenderDispatch);
    return true;
}


bool generarConexionesConCPU() {

    char *ip;

    ip = strdup(cfg_kernel->IP_CPU);
    log_trace(trace_logger, "Lei la IP [%s]", ip);

    char *puerto;
    puerto = strdup(cfg_kernel->PUERTO_CPU);

    log_trace(trace_logger, "Lei el PUERTO [%s]", puerto);

    fd_cpu = crear_conexion(
            info_logger,
            "SERVER CPU",
            ip,
            puerto
    );

    free(ip);
    free(puerto);

    return fd_cpu != 0;
}


void* conectarConMemoria(){
    bool comprobacion = generarConexionesConMemoria();
    atenderMemoria();

}


bool generarConexionesConMemoria(){
    char* ip;

    ip = strdup(cfg_kernel->IP_MEMORIA);
    log_trace(trace_logger,"Lei la IP [%s]", ip);

    char* puerto;
    puerto = strdup(cfg_kernel->PUERTO_MEMORIA);

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


void* conectarConFileSystem(){
    bool comprobacion = generarConexionesConFilesystem();

    if(comprobacion){
        atenderFilesystem();
    }

}


bool generarConexionesConFilesystem(){
    char* ip;

    ip = strdup(cfg_kernel->IP_FILESYSTEM);
    log_trace(trace_logger,"Lei la IP [%s]", ip);

    char* puerto;
    puerto = strdup(cfg_kernel->PUERTO_FILESYSTEM);

    log_trace(trace_logger,"Lei el PUERTO [%s]", puerto);

    fd_filesystem = crear_conexion(
            info_logger,
            "SERVER FILESYSTEM",
            ip,
            puerto
    );

    free(ip);
    free(puerto);

    return fd_filesystem != 0;

}


bool atenderFilesystem(){
    if (fd_filesystem == -1){
        return EXIT_FAILURE;
    }
    pthread_t atenderFilesystem;
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->fd = fd_filesystem;
    args->server_name = "ATENDER_FILESYSTEM";
    pthread_create(&atenderFilesystem, NULL,(void*)procesar_conexion,args);
    pthread_detach(atenderFilesystem);
    return true;
}





