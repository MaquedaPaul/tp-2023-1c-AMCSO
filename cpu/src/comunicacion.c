//
// Created by utnso on 4/7/23.
//

#include <comunicacion.h>
//int conexion_actual; //TODO fijarse si hace falta eliminar

bool conexionesHechas = false;
t_pcb* pcb_actual;
int pid;

void procesar_conexion(void *void_args) {
    pid = 2000000000; //TODO no es una solución elegante utilizar números aleatorios grandes
    while (1) {
        op_code cop = recibir_operacion(fd_kernel);

        if(cop>0) {
            switch (cop) {
                case PCB: {

                    pcb_actual = recibir_pcb(fd_kernel);

                    if (pcb_actual->id == pid) {
                        ciclo_de_instruccion();
                        //eliminar_PCB(pcb_actual);
                        break;
                    } else {
                        pid = pcb_actual->id;
                        copiar_registroPCB_a_los_registrosCPU(pcb_actual->registrosCpu);
                        ciclo_de_instruccion();
                        //eliminar_PCB(pcb_actual);
                        break;


                    }
                }
                case -1:
                    log_error(error_logger, "Cliente desconectado de %s...", "SERVER CPU");
                    return;
                default:
                    log_error(error_logger, "Algo anduvo mal en el server de %s", "SERVER CPU");
                    log_info(info_logger, "Cop: %d", cop);
                    return;
            }
        }
    }
    log_warning(warning_logger, "El cliente se desconecto de %s server", "SERVER CPU");
    return;
}


bool generar_conexiones() {
        pthread_t conexion_con_memoria;
        pthread_create(&conexion_con_memoria, NULL, (void *) conectarConMemoria, NULL);
        if(!crearServidor()){
            return false;
        }

        pthread_join(conexion_con_memoria, NULL);
        conexionesHechas = true;
        return true; //debe tomarse lo que retorna el hilo al crear el servidor
}

bool crearServidor() {
    puerto_cpu = cfg_cpu->PUERTO_ESCUCHA;
    fd_cpu = iniciar_servidor(info_logger, "SERVER CPU", ip_cpu, puerto_cpu);

    if (fd_cpu == 0) {
        log_error(error_logger, "Fallo al crear el servidor CPU, cerrando CPU");
        return EXIT_FAILURE;
    }
    fd_kernel = esperar_cliente(info_logger, "SERVER CPU", fd_cpu);
    pthread_t atenderPeticionesKernel;
    pthread_create(&atenderPeticionesKernel, NULL, (void*)procesar_conexion, NULL);
    pthread_join(atenderPeticionesKernel, NULL);
}


void *conectarConMemoria() {
        bool comprobacion = generarConexionesConMemoria();
        if (comprobacion) {
            enviarOrden(HANDSHAKE_CPU, fd_memoria, info_logger);
            op_code cod = recibir_operacion(fd_memoria);
            if(cod == HANDSHAKE_CPU){
                recibirOrden(fd_memoria);
                log_info(info_logger,"HANDSHAKE con Memoria acontecido");
            }
            else{
                log_error(error_logger,"FALLO en el recibo del HANDSHAKE de Memoria");
            }

        }

}

bool generarConexionesConMemoria() {
        char *ip;

        ip = strdup(cfg_cpu->IP_MEMORIA);
        log_trace(trace_logger, "Lei la IP [%s]", ip);

        char *puerto;
        puerto = strdup(cfg_cpu->PUERTO_MEMORIA);

        log_trace(trace_logger, "Lei el PUERTO [%s]", puerto);

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


