//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>
int fd_kernel;

void* procesar_conexion(void *void_args) {
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
            case DEBUG:
                break;
            case 10:
            {
                break;
            }
            case 100:
            {
                break;
            }
            case 1000://PROCESO_TERMINADO:
            {
                //finalizarme();
                return;
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



bool generar_conexiones() {
    char* ip;
    char* puerto;
    ip = strdup(cfg_console->IP_KERNEL);
    puerto = strdup(cfg_console->PUERTO_KERNEL);

    log_trace(trace_logger,"La IP obtenida del config es : [%s]", ip);
    log_trace(trace_logger,"El PUERTO obtenido del config es : [%s]", puerto);

    fd_kernel = crear_conexion(
            trace_logger,
            "SERVER KERNEL",
            ip,
            puerto
    );

    free(ip);
    free(puerto);
    return fd_kernel != 0;
}