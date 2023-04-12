//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>
char* ip_kernel;
char* puerto_kernel;
int fd_kernel;
static void procesar_conexion(void *void_args) {
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
            case 10:
            {
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