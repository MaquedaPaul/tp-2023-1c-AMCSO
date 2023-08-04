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
                pcbDispatch->fd_consola=cliente_socket;
                agregarProceso_New(pcbDispatch);

                mostrarEstadoColas();
                break;

            }
                //--------------------------------CPU-------------------------------------------
            case WAIT:
            {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                manejoDeRecursos("WAIT");
                break;
            }
            case SIGNAL:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                manejoDeRecursos("SIGNAL");
                break;
            }

            case IO_BLOCK: {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                t_pcb* pcbActualizada = obtenerPcbExec();
                moverProceso_ExecBloq(pcbActualizada);
                log_info(info_logger,"PID: <%d> - Bloqueado por <IO>", pcbActualizada->id);
                pthread_t atenderIO;
                pthread_create(&atenderIO,NULL,esperaIo,(void*)pcbActualizada);
                pthread_detach(atenderIO);
                break;
            }
            case YIELD:{

                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                t_pcb* pcbActualizada = obtenerPcbExec();
                moverProceso_ExecReady(pcbActualizada);
                break;
            }
            case SEGMENTATION_FAULT:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                t_pcb* pcbActualizada = obtenerPcbExec();
                log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <SEG_FAULT>",pcbActualizada->id); //Motivo: <SUCCESS / SEG_FAULT / OUT_OF_MEMORY>
                moverProceso_ExecExit(pcbActualizada);
                break;
            }
            case EXIT:{
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                t_pcb* pcbActualizada = obtenerPcbExec();
                log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <SUCCESS>",pcbActualizada->id);
                moverProceso_ExecExit(pcbActualizada);
                break;
            }

            case CREATE_SEGMENT:
            {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                t_pcb* pcbActualizada = obtenerPcbExec();
                solicitarCreacionSegmentoMemoria(pcbActualizada);
                break;
            }
            case DELETE_SEGMENT: {
                t_pcb* pcbRecibida = recibir_pcb(cliente_socket);
                actualizarPcbExec(pcbRecibida);
                t_pcb* pcbActualizada = obtenerPcbExec();

                //Memoria necesita: PID, idSegmento
                t_list* listaIntsMemoria = list_create();
                list_add(listaIntsMemoria,&pcbActualizada->id);

                t_instr* instruccion = list_get(pcbActualizada->instr,pcbActualizada->programCounter-1);
                uint32_t idSegmento = atoi(instruccion->param1);
                list_add(listaIntsMemoria,&idSegmento);

                log_info(info_logger,"PID: <%d> - Eliminar Segmento - Id Segmento: <%d>", pcbActualizada->id, idSegmento);
                enviarListaUint32_t(listaIntsMemoria,fd_memoria,info_logger,ELIMINACION_SEGMENTOS);
                break;
            }

            case F_OPEN: {
                //recibe largoNombreArchivo /nombreArchivo / pcb
                t_pcb* unPcb = recibir_pcb(cliente_socket);
                actualizarPcbExec(unPcb);
                t_pcb* pcbActualizada = obtenerPcbExec();
                ejecutar_FOPEN(pcbActualizada);
                break;
            }

            case F_CLOSE: {
                //recibe largoNombreArchivo /nombreArchivo / pcb
                t_pcb* unPcb = recibir_pcb(cliente_socket);
                actualizarPcbExec(unPcb);
                t_pcb* pcbActualizada = obtenerPcbExec();
                ejecutar_FCLOSE(pcbActualizada);
                break;
            }

            case F_SEEK: {
                //recibe largoNombreArchivo /nombreArchivo / puntero / pcb
                t_pcb* unPcb = recibir_pcb(cliente_socket);
                actualizarPcbExec(unPcb);
                t_pcb* pcbActualizada = obtenerPcbExec();
                ejecutar_FSEEK(pcbActualizada);
                break;
            }

            case F_TRUNCATE: {
                //recibe largoNombreArchivo /nombreArchivo / tamaño / pcb
                t_pcb* unPcb = recibir_pcb(cliente_socket);
                actualizarPcbExec(unPcb);
                t_pcb* pcbActualizada = obtenerPcbExec();
                ejecutar_FTRUNCATE(pcbActualizada);
                break;
            }

            case F_READ: {
                //recibe largoNombreArchivo /nombreArchivo / direccion / largoDireccion / pcb
                u_int32_t direccionFisica;
                t_pcb* unPcb = recibir_pcb_direccion(cliente_socket, &direccionFisica);
                actualizarPcbExec(unPcb);
                t_pcb* pcbActualizada = obtenerPcbExec();
                ejecutar_FREAD(pcbActualizada,direccionFisica);
                break;
            }

            case F_WRITE: {
                //recibe largoNombreArchivo /nombreArchivo / direccion / largoDireccion / pcb
                u_int32_t direccionFisica;
                t_pcb* unPcb = recibir_pcb_direccion(cliente_socket, &direccionFisica);
                actualizarPcbExec(unPcb);
                t_pcb* pcbActualizada = obtenerPcbExec();
                ejecutar_FWRITE(pcbActualizada, direccionFisica);
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
                free(nombreArchivo);
                break;
            }

            case CREACION_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                agregarEntrada_TablaGlobalArchivosAbiertos(nombreArchivo);
                //NO VA LIBERACION NOMARCHIVO
                break;
            }
            
            case TRUNCACION_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                desbloquearPcb_porNombreArchivo(nombreArchivo);
                break;
            }

            case LECTURA_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                desbloquearPcb_porNombreArchivo(nombreArchivo);
                break;
            }

            case ESCRITURA_ARCHIVO_EXITOSA: {
                char* nombreArchivo = recibirString(cliente_socket);
                desbloquearPcb_porNombreArchivo(nombreArchivo);
                break;
            }

                //----------------------------------MEMORIA----------------------------------------
            case ESTRUCTURAS_INICALIZADAS: {
                t_list *listaTablaSegmentosRecibida = recibirTablasSegmentosInstrucciones(cliente_socket);
                t_tablaSegmentos *tablaSegmentos = list_get(listaTablaSegmentosRecibida, 0);
                list_clean(listaTablaSegmentosRecibida);
                list_destroy(listaTablaSegmentosRecibida);
                moverProceso_NewReady(tablaSegmentos);

                break;
            }
            case CREACION_SEGMENTO_EXITOSO:
            {
                uint32_t baseSegmento = recibirValor_uint32(cliente_socket,info_logger);
                creacionSegmentoExitoso(baseSegmento);
                break;
            }
            case SIN_ESPACIO_DISPONIBLE:
            {
                recibirOrden(cliente_socket);
                t_pcb* pcbExec = obtenerPcbExec(); //Ya que se elimina en moverProceso
                log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <OUT_OF_MEMORY>",pcbExec->id);
                moverProceso_ExecExit(pcbExec);
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
                t_list* listaTablaSegmentosRecibida = recibirTablasSegmentosInstrucciones(cliente_socket);
                actualizarTablasSegmentosProcesos(listaTablaSegmentosRecibida);
                log_info(info_logger,"Se finalizó el proceso de compactación");
                t_pcb* pcbExec = obtenerPcbExec();
                solicitarCreacionSegmentoMemoria(pcbExec);
                break;
            }
            case SEGMENTO_ELIMINADO:{
                t_list* listaTablaSegmentosRecibida = recibirTablasSegmentosInstrucciones(cliente_socket);
                t_tablaSegmentos* tablaSegmentos = list_get(listaTablaSegmentosRecibida,0);
                eliminacionSegmento(tablaSegmentos);
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
    log_trace(trace_logger,"CONEXIONES LIBERADAS");
}

void cerrar_servers(){
    close(fd_kernel);
    log_trace(trace_logger,"SERVIDORES CERRADOS");
}

void waitRecursoPcb(t_recurso* recurso, t_pcb* unaPcb) {
    recurso->instanciasRecurso--;
    list_add(unaPcb->recursosTomados,recurso);
    log_info(info_logger,"PID: <%d> - Wait: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if (recurso->instanciasRecurso < 0) {
        list_add(recurso->cola, unaPcb);
        log_info(info_logger,"PID: <%d> - Bloqueado por: <%s>",unaPcb->id,recurso->nombreRecurso);
        bloquearProcesoPorRecurso(recurso);
    }else{
        enviar_paquete_pcb(unaPcb,fd_cpu,PCB,info_logger);
    }
}

bool list_remove_element(t_list *self, void *element) {
    bool _is_the_element(void *data) {
        return element == data;
    }
    return list_remove_by_condition(self, _is_the_element) != NULL;
}

void signalRecursoPcb(t_recurso* recurso, t_pcb* unaPcb){
    recurso->instanciasRecurso++;
    list_remove_element(unaPcb->recursosTomados,recurso);
    log_info(info_logger,"PID: <%d> - Signal: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if(!list_is_empty(recurso->cola)){
        moverProceso_BloqrecursoReady(recurso);
    }
     enviar_paquete_pcb(unaPcb,fd_cpu,PCB,info_logger);
}

void manejoDeRecursos(char* orden){
    t_pcb* unaPcb = obtenerPcbExec();
    t_instr * instruccion = list_get(unaPcb->instr,unaPcb->programCounter-1);
    char* recursoSolicitado = instruccion->param1;
    mostrarEstadoRecursos();
    bool coincideConSolicitado(t_recurso* unRecurso){
        return strcmp(unRecurso->nombreRecurso,recursoSolicitado) == 0;
    }
    t_recurso* recursoEncontrado = list_find(estadoBlockRecursos, coincideConSolicitado);
    if(recursoEncontrado == NULL){
        log_info(info_logger,"Recurso <%s> solicitado INEXISTENTE", recursoSolicitado);
        log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>",unaPcb->id);
        moverProceso_ExecExit(unaPcb);
    }else{
        if(strcmp(orden,"WAIT") == 0){
            waitRecursoPcb(recursoEncontrado, unaPcb);
        } else{
            signalRecursoPcb(recursoEncontrado, unaPcb);
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
    segmento->limite = tamSegmento;

    list_add(pcbExec->tablaSegmentos->segmentos,segmento);
    log_debug(debug_logger,"base del segmento: %d", segmento->base);
    log_debug(debug_logger,"limite del segmento: %d", segmento->limite);
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

void eliminacionSegmento(t_tablaSegmentos* tablaSegmentosActualizada){
    pthread_mutex_lock(&mutex_colaExec);
    t_pcb* pcbExec = list_get(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);

    pcbExec->tablaSegmentos = tablaSegmentosActualizada;
    enviar_paquete_pcb(pcbExec,fd_cpu,PCB,info_logger);
}


void actualizarTablasSegmentosProcesos(t_list* listaTablasSegmentosProcesos){
    //Los procesos pueden estar en READY,EXEC,BLOCK,BLOKEDRECURSO
    for (int i = 0 ; i < list_size(listaTablasSegmentosProcesos); i++){
        t_tablaSegmentos* tablaSegmentos = list_get(listaTablasSegmentosProcesos,i);

        buscarDuenioTablaSegEnListaYActualizar(colaReady,tablaSegmentos);
        buscarDuenioTablaSegEnListaYActualizar(colaExec,tablaSegmentos);
        buscarDuenioTablaSegEnListaYActualizar(colaBloq,tablaSegmentos);

        for (int j = 0; j < list_size(estadoBlockRecursos); j++){
            t_recurso* recurso = list_get(estadoBlockRecursos,j);
            buscarDuenioTablaSegEnListaYActualizar(recurso->cola,tablaSegmentos);
        }
    }
}

void buscarDuenioTablaSegEnListaYActualizar(t_list* listaEstado,t_tablaSegmentos* tablaSegmentos){
    for(int j = 0; j < list_size(listaEstado); j++){
        t_pcb* pcb = list_get(listaEstado,j);
        if(pcb->id == tablaSegmentos->pid){
            pcb->tablaSegmentos = tablaSegmentos;
        }
    }
}

void solicitarCreacionSegmentoMemoria(t_pcb* pcb){
    //Memoria necesita: PID,idSeg ,tamaño segmento
    t_list* listaIntsMemoria = list_create();
    list_add(listaIntsMemoria,&pcb->id);

    t_instr* instruccion = list_get(pcb->instr,pcb->programCounter-1);
    uint32_t idSegmento = atoi(instruccion->param1);
    list_add(listaIntsMemoria,&idSegmento);

    uint32_t tamSegmento = atoi(instruccion->param2);
    list_add(listaIntsMemoria,&tamSegmento);

    log_info(info_logger,"PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>", pcb->id,idSegmento,tamSegmento);
    enviarListaUint32_t(listaIntsMemoria,fd_memoria,info_logger,CREACION_SEGMENTOS);
    list_clean(listaIntsMemoria);
    list_destroy(listaIntsMemoria);
}

void actualizarPcbExec(t_pcb* pcbRecibida){
    //Actualizo los campos PC,Registros de la pcbRecibida. Se lo asigno a la pcb vieja que se encuentra
    pthread_mutex_lock(&mutex_colaExec);
    t_pcb* pcbExec = list_get(colaExec,0);

    registros_cpu* registrosAux = pcbExec->registrosCpu;
    pcbExec->programCounter = pcbRecibida->programCounter;
    pcbExec->registrosCpu = pcbRecibida->registrosCpu;
    pcbRecibida->registrosCpu = registrosAux;
    liberarPcbCpu(pcbRecibida);
    pthread_mutex_unlock(&mutex_colaExec);

}

t_pcb* obtenerPcbExec(){
    pthread_mutex_lock(&mutex_colaExec);
    t_pcb* unaPcb = list_get(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);
    return unaPcb;
}
