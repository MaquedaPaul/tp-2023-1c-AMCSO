//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>


bool conexionesHechas = false;
t_pcb* pcb_actual;

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

            case PCB:
                pcb_actual = recibir_pcb(cliente_socket);
                log_info(info_logger,"Falla iniciar regisitros o algo de aca abajo");
                //TODO CHEQUEAR ACA
                iniciar_registros (pcb_actual->registrosCpu);
                cicloInstruccionesDebeEjecutarse = true;
                ciclo_instrucciones();
               //eliminar_PCB(pcb_actual);
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
    pthread_t crear_server_cpu;
    pthread_t conexion_con_memoria;
    pthread_create(&crear_server_cpu,NULL, (void*)crearServidor,NULL);
    pthread_create(&conexion_con_memoria,NULL, (void*)conectarConMemoria,NULL);
    pthread_join(crear_server_cpu, NULL);
    pthread_join(conexion_con_memoria, NULL);
    conexionesHechas = true;
    return true; //debe tomarse lo que retorna el hilo al crear el servidor
}


void* crearServidor(){
    puerto_cpu = cfg_cpu->PUERTO_ESCUCHA;
    fd_cpu = iniciar_servidor(info_logger, "SERVER CPU", ip_cpu,puerto_cpu);

    if (fd_cpu == 0) {
        log_error(error_logger, "Fallo al crear el servidor CPU, cerrando CPU");
        return (void*)EXIT_FAILURE;
    }

    while (server_escuchar(info_logger, "SERVER CPU", fd_cpu));
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

    ip = strdup(cfg_cpu->IP_MEMORIA);
    log_trace(trace_logger,"Lei la IP [%s]", ip);

    char* puerto;
    puerto = strdup(cfg_cpu->PUERTO_MEMORIA);

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