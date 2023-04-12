//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>
pthread_t crear_server_cpu;
int fd_cpu;
char* ip_cpu;
char* puerto_cpu;


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
            case 10:
                //proceso_iniciado(cliente_socket);

                break;
            case 100:
                //proceso_terminado(cliente_socket);
                break;
            case 1000:
                //solicitud_marco(cliente_socket);
                break;
            case 20:
                //pedido_escritura(cliente_socket);
                break;
            case 200:
                //pedido_lectura(cliente_socket);
                break;
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

    pthread_create(&crear_server_cpu,NULL, crearServidor,NULL);
    pthread_join(crear_server_cpu, NULL);
    return true; //debe tomarse lo que retorna el hilo al crear el servidor
}


void* crearServidor(){
    puerto_cpu=cfg_cpu->PUERTO_ESCUCHA;
    fd_cpu = iniciar_servidor(info_logger, "SERVER CPU", ip_cpu,puerto_cpu);

    if (fd_cpu == 0) {
        log_error(error_logger, "Fallo al crear el servidor CPU, cerrando CPU");
        return (void*)EXIT_FAILURE;
    }

    while (server_escuchar(info_logger, "SERVER CPU", fd_cpu));
}
