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

t_log* logger_kernel;


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
            case GESTIONAR_CONSOLA_NUEVA:
            {
                t_list* listaInstrucciones = recibirListaInstrucciones(cliente_socket);
                establecerCantidadParametrosInstrucciones(listaInstrucciones);
                t_pcb *pcbDispatch = crearPcb(listaInstrucciones);
                agregarProceso_New(pcbDispatch);
                break;

            }
                //--------------------------------CPU-------------------------------------------
            case WAIT:
            {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                manejoDeRecursos(pcbRecibida,"WAIT");
                break;
            }
            case SIGNAL:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                manejoDeRecursos(pcbRecibida,"SIGNAL");
                break;
            }

            case IO_BLOCK: {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarTiempoRafaga(pcbRecibida);
                moverProceso_ExecBloq(pcbRecibida);
                log_info(info_logger,"PID: <%d> - Bloqueado por <IO>", pcbRecibida->id);
                pthread_t atenderIO;
                pthread_create(&atenderIO,NULL,esperaIo,(void*)pcbRecibida);
                pthread_detach(atenderIO);
                break;
            }
            case YIELD:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarTiempoRafaga(pcbRecibida);
                moverProceso_ExecReady(pcbRecibida);
                break;
            }
            case SEGMENTATION_FAULT:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <SEG_FAULT>",pcbRecibida->id); //Motivo: <SUCCESS / SEG_FAULT / OUT_OF_MEMORY>
                moverProceso_ExecExit(pcbRecibida);
                eliminarPcb_TGAA_SEGFAULT(pcbRecibida);
            }
            case EXIT:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <SUCCESS>",pcbRecibida->id);
                moverProceso_ExecExit(pcbRecibida);
            }

            case CREATE_SEGMENT:
            {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);

                //Memoria necesita: PID, tamaño segmento
                t_list* listaIntsMemoria = list_create();
                list_add(listaIntsMemoria,&pcbRecibida->id);

                t_instr* instruccion = list_get(pcbRecibida->instr,pcbRecibida->programCounter-1);
                uint32_t tamSegmento = atoi(instruccion->param2);
                list_add(listaIntsMemoria,&tamSegmento);

                uint32_t idSegmento = atoi(instruccion->param1); //Lo usamos solo para el logger
                log_info(info_logger,"PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>", pcbRecibida->id,idSegmento,tamSegmento);
                enviarListaUint32_t(listaIntsMemoria,fd_memoria,info_logger,CREACION_SEGMENTOS);
                list_destroy_and_destroy_elements(listaIntsMemoria,free);
                break;
            }
            case DELETE_SEGMENT: {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);

                //Memoria necesita: PID, idSegmento
                t_list* listaIntsMemoria = list_create();
                list_add(listaIntsMemoria,&pcbRecibida->id);

                t_instr* instruccion = list_get(pcbRecibida->instr,pcbRecibida->programCounter-1);
                uint32_t idSegmento = atoi(instruccion->param1);
                list_add(listaIntsMemoria,&idSegmento);

                log_info(info_logger,"PID: <%d> - Eliminar Segmento - Id Segmento: <%d>", pcbRecibida->id, idSegmento);
                enviarListaUint32_t(listaIntsMemoria,fd_memoria,info_logger,ELIMINACION_SEGMENTOS);
                list_destroy_and_destroy_elements(listaIntsMemoria,free);
                break;
            }

            case F_OPEN: {
                //recibe largoNombreArchivo /nombreArchivo / pcb
                ejecutar_FOPEN_socket(cliente_socket);
                break;
            }

            case F_CLOSE: {
                //recibe largoNombreArchivo /nombreArchivo / pcb
                ejecutar_FCLOSE(cliente_socket);

                break;
            }

            case F_SEEK: {
                //recibe largoNombreArchivo /nombreArchivo / puntero / pcb
                ejecutar_FSEEK(cliente_socket);
                break;
            }

            case F_TRUNCATE: {
                //recibe largoNombreArchivo /nombreArchivo / tamaño / pcb
                ejecutar_FTRUNCATE(cliente_socket);
                break;
            }

            case F_READ: {
                //recibe largoNombreArchivo /nombreArchivo / direccion / largoDireccion / pcb
                ejecutar_FREAD(cliente_socket);
                break;
            }

            case F_WRITE: {
                //recibe largoNombreArchivo /nombreArchivo / direccion / largoDireccion / pcb
                ejecutar_FWRITE(cliente_socket);
                break;
            }

            //----------------------------------FILESYSTEM--------------------------------------

            case APERTURA_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                agregarEntrada_TablaGlobalArchivosAbiertos(nombreArchivo);
                break;
            }

            case APERTURA_ARCHIVO_FALLIDA: {
                char* nombreArchivo = recibirString(cliente_socket);
                enviarString(nombreArchivo, fd_filesystem, CREACION_ARCHIVO, info_logger);
                break;
            }

            case CREACION_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                agregarEntrada_TablaGlobalArchivosAbiertos(nombreArchivo);
                break;
            }
            
            case TRUNCACION_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                break;
            }

            case LECTURA_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                break;
            }

            case ESCRITURA_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                break;
            }

                //----------------------------------MEMORIA----------------------------------------
            case ESTRUCTURAS_INICALIZADAS:
            {
                t_list* listaTablaSegmentosRecibida = recibirTablasSegmentosInstrucciones(cliente_socket);
                t_tablaSegmentos* tablaSegmentos = list_get(listaTablaSegmentosRecibida,0);
                moverProceso_NewReady(tablaSegmentos);
                break;
            }
            case CREACION_SEGMENTO_EXITOSO:
            {
                uint32_t baseSegmento = recibirValor_uint32(cliente_socket,info_logger);
                creacionSegmentoExitoso(baseSegmento);
                break;
            }
            case OUT_OF_MEMORY:
            {
                recibirOrden(cliente_socket);
                t_pcb* pcbRecibida = list_get(colaExec,0); //Ya que se elimina en moverProceso
                log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <OUT_OF_MEMORY>",pcbRecibida->id);
                moverProceso_ExecExit(pcbRecibida);
                break;
            }
            case FINALIZACION_PROCESO_TERMINADA:
            {
                recibirOrden(cliente_socket);
                log_info(info_logger,"Recibo Confimarcion de FINALIZACION DE PROCESO TERMINADA");
                decrementarGradoMP();
                break;
            }
            case SE_NECESITA_COMPACTACION: {
                recibirOrden(cliente_socket);
                enviarOrden(SE_NECESITA_COMPACTACION,fd_filesystem,info_logger);
                break;
            }
            case COMPACTACION_FINALIZADA: {
                //TODO FALTA actualizar las tablas de segmentos de todos los procesos en Memoria
                log_info(info_logger,"Se finalizó el proceso de compactación");
                break;
            }
            case SEGMENTO_ELIMINADO:{
                t_list* tablaSegmentosRecibida = recibirTablasSegmentosInstrucciones(cliente_socket);
                eliminacionSegmento(tablaSegmentosRecibida);
                break;
            }

            //-------------------------FILE SYSTEM-----------------------------------------------------------

            case PUEDO_COMPACTAR: {
                recibirOrden(cliente_socket);
                log_info(info_logger,"Compactación: <Se solicitó compactación>");
                enviarOrden(COMPACTACION_SEGMENTOS,fd_memoria,info_logger);
                break;
            }

            case ESPERAR_PARA_COMPACTACION: {
                recibirOrden(cliente_socket);
                log_info(info_logger,"Compactación: <Esperando Fin de Operaciones de FS>");
                //Cuando file system termine me tiene que enviar una orden PUEDO_COMPACTAR.
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


void cortar_conexiones(){
    liberar_conexion(&fd_kernel);
    liberar_conexion(&fd_cpu);
    liberar_conexion(&fd_memoria);
    liberar_conexion(&fd_filesystem);
    log_info(info_logger,"CONEXIONES LIBERADAS");
}

void cerrar_servers(){
    close(fd_kernel);
    log_info(info_logger,"SERVIDORES CERRADOS");
}

void waitRecursoPcb(t_recurso* recurso, t_pcb* unaPcb) {
    recurso->instanciasRecurso--;
    log_info(info_logger,"PID: <%d> - Wait: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if (recurso->instanciasRecurso < 0) {
        queue_push(recurso->cola, unaPcb);
        log_info(info_logger,"PID: <%d> - Bloqueado por: <%s>",unaPcb->id,recurso->nombreRecurso);
    }
}


void signalRecursoPcb(t_recurso * recurso, t_pcb* unaPcb){
    recurso->instanciasRecurso++;
    log_info(info_logger,"PID: <%d> - Signal: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if(!queue_is_empty(recurso->cola)){
        moverProceso_BloqrecursoReady(recurso);
    }
     enviar_paquete_pcb(unaPcb,fd_cpu,SIGNAL,info_logger);
}

void manejoDeRecursos(t_pcb* unaPcb,char* orden){
    int apunteProgramCounter = unaPcb->programCounter;
    t_instr * instruccion = list_get(unaPcb->instr,apunteProgramCounter-1);
    char* recursoSolicitado = instruccion->param1;
    for(int i = 0 ; i < list_size(estadoBlockRecursos); i++){
        t_recurso* recurso = list_get(estadoBlockRecursos,i);
        if((strcmp(recurso->nombreRecurso,recursoSolicitado)) == 0){
            if((strcmp(orden,"WAIT")) == 0){
                waitRecursoPcb(recurso,unaPcb);
            }else{
                signalRecursoPcb(recurso,unaPcb);
            }
        }else{
            log_info(info_logger,"Recurso <%s> solicitado INEXISTENTE", recursoSolicitado);
            moverProceso_ExecExit(unaPcb);
        }
        }
}

void creacionSegmentoExitoso(uint32_t baseSegmento){
    pthread_mutex_lock(&mutex_colaExec);
    t_pcb* pcbExec = list_get(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);

    t_instr* instruccion = list_get(pcbExec->instr,pcbExec->programCounter-1);
    uint32_t idSegmento = atoi(instruccion->param1);
    uint32_t tamSegmento = atoi(instruccion->param2);

    t_segmento* segmento = malloc(sizeof (t_segmento));
    segmento->id = idSegmento;
    segmento->base = baseSegmento;
    segmento->limite = baseSegmento + tamSegmento;

    list_add(pcbExec->tablaSegmentos,segmento);

    enviar_paquete_pcb(pcbExec,fd_cpu,PCB,info_logger);
}

void* esperaIo(void* void_pcb){
    t_pcb* pcb = (t_pcb*) void_pcb;
    t_instr* instruccion = list_get(pcb->instr,pcb->programCounter-1);

    //EJEMPLO DE INSTRUCCION IO 10
    int tiempoEspera = atoi(instruccion->param1);
    log_info(info_logger, "PID: <%d> - Ejecuta IO: <%d>", pcb->id,tiempoEspera);
    usleep(tiempoEspera);
    moverProceso_BloqReady(pcb);
}

void eliminacionSegmento(t_list* tablaSegmentosActualizada){
    pthread_mutex_lock(&mutex_colaExec);
    t_pcb* pcbExec = list_get(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);

    pcbExec->tablaSegmentos = tablaSegmentosActualizada;
    enviar_paquete_pcb(pcbExec,fd_cpu,PCB,info_logger);
}