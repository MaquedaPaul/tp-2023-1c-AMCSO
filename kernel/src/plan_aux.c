#include <plan_aux.h>


void eliminarElementoLista(t_pcb* pcbBuscado, t_list *listaX){

  t_pcb *pcbAux;
  for (int i = 0; i < list_size(listaX); i++)
  {
     pcbAux = list_get(listaX, i);
    if(pcbBuscado->id == pcbAux->id){
      list_remove(listaX, i);
    }
  }
}

// ::::::::::::::::::::PCB:::::::::::::::::::::


t_pcb* crearPcb(t_list* listaInstrucciones)
{
  t_pcb *pcb = malloc(sizeof(t_pcb));
  pcb->id = idProcesoGlobal++;
  pcb->programCounter = 0;
  pcb->rafagaAnterior = 0;
  pcb->estimacionRafaga = cfg_kernel->ESTIMACION_INICIAL;
  pcb->tiempoLlegadaReady = 0;
  pcb->tiempoEnvioExec = 0;
  pcb->registrosCpu = crearRegistroCPU();
  pcb->instr = listaInstrucciones;
  pcb->tablaSegmentos = NULL;
  pcb->tablaArchivosAbiertos = list_create();


  return pcb;
}

registros_cpu* crearRegistroCPU() {
    registros_cpu* puntero_registros = malloc(sizeof(registros_cpu)); // Asignar memoria para la estructura

    // Inicializar los registros con valores predeterminados
    strcpy(puntero_registros->registro_AX, "0000");
    strcpy(puntero_registros->registro_BX, "0000");
    strcpy(puntero_registros->registro_CX, "0000");
    strcpy(puntero_registros->registro_DX, "0000");
    strcpy(puntero_registros->registro_EAX, "00000000");
    strcpy(puntero_registros->registro_EBX, "00000000");
    strcpy(puntero_registros->registro_ECX, "00000000");
    strcpy(puntero_registros->registro_EDX, "00000000");
    strcpy(puntero_registros->registro_RAX, "0000000000000000");
    strcpy(puntero_registros->registro_RCX, "0000000000000000");
    strcpy(puntero_registros->registro_RBX, "0000000000000000");
    strcpy(puntero_registros->registro_RDX, "0000000000000000");

    return puntero_registros;
}



//Actualizar pcb

void actualizarTiempoRafaga(t_pcb* pcbEjecutado){

    time_t t_actual = time(NULL) ;
    double t_envioExec = pcbEjecutado->tiempoEnvioExec;
    
    pcbEjecutado->rafagaAnterior = t_actual - t_envioExec;
    
}

//----------------------------FileSystem----------------------------------
/*ACOTACIONES
1.FCLOSE retoma FOPEN si hay alguno encolado, sino borra la entrada en TGAA
2.Se implemento una tabla local de peticiones a FS para trackear archivo-pcb-opCode
3.FSEEK no avisa a FS o CPU
4.FTRUNCATE FREAD FWRITE bloquean el proceso

void* datosRecibidos = recibirDatos(socket_cliente, tamanioDatos);

// Suponiendo que los datos están compuestos por: posición 1 (estructura),  posición 2 (int) y posición 3(string)
// Ejemplo de uso de la estructura en posición 1
Estructura* estructuraRecibida = (Estructura*)datosRecibidos;
int enteroRecibido;
memcpy(&enteroRecibido, datosRecibidos + sizeof(Estructura)  + 1, sizeof(int));
// Ejemplo de uso del string en posición 3 --> hay que evaluarlo segun los ints/struct que tenga antes e ir sumando sizeof
char* cadenaRecibida = (char*)(datosRecibidos + (sizeof(Estructura)+sizeof(int)));
free(datosRecibidos);

FALTA

    1. verificar bloq de pcb x filesystem o devolver a exec --> AVERIGUAR que hacer con la pcb
    5. FSEEK: que tiene que avisar a FS o CPU?
    8. COORDINAR CON COMPACTACION

*/
//TODO habria que hacer free de archivo,t_archivoPeticion y t_archivoLocal

void ejecutar_FOPEN(t_pcb* pcb){
    //F_OPEN ARCHIVO

    char* nomArch = obtenerNombreArchivo(pcb);

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nomArch);

    if(pos == -1){ // En caso de que no este en la TGAA
        t_archivoPeticion* archivoPeticion = malloc(sizeof(t_archivoPeticion));
        t_archivo* archivo = malloc(sizeof(t_archivo));
        archivo->nombreArchivo = nomArch;
        pthread_mutex_init(&archivo->mutex,NULL);
        archivoPeticion->archivo = archivo;
        archivoPeticion->pcb = pcb;
        pthread_mutex_lock(&mutex_listaPeticionesArchivos);
        list_add(listaPeticionesArchivos,archivoPeticion);
        pthread_mutex_unlock(&mutex_listaPeticionesArchivos);

        enviarString(nomArch, fd_filesystem, APERTURA_ARCHIVO, info_logger);

    }

    else{//Si esta en la TGAA
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,pos);
        pthread_mutex_lock(&(archivoPeticion->archivo->mutex));
        t_archivoPeticion* peticion = malloc(sizeof (t_archivoPeticion));
        peticion->archivo = archivoPeticion->archivo;
        peticion->pcb = pcb;
        pthread_mutex_lock(&mutex_listaPeticionesArchivos);
        list_add(listaPeticionesArchivos,peticion);
        pthread_mutex_unlock(&mutex_listaPeticionesArchivos);
        pthread_mutex_unlock(&(archivoPeticion->archivo->mutex));

        t_archivoLocal* archivoLocal = malloc(sizeof (t_archivoLocal));
        archivoLocal->archivo = archivoPeticion->archivo;
        archivoLocal->ptro = 0;
        list_add(pcb->tablaArchivosAbiertos,archivoLocal);

        moverProceso_ExecBloq(pcb);
    }
    pthread_mutex_unlock(&mutex_TGAA);

}
/*
pthread_mutex_lock(&mutex_listaPeticionesArchivos);

for(int i = 0 ; i < list_size(listaPeticionesArchivos); i++){
t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
if(strcmp(archivoPeticion->archivo->nombreArchivo,nomArch)== 0){
list_remove(listaPeticionesArchivos,i);

pthread_mutex_lock(&mutex_TGAA);
list_add(tablaGlobal_ArchivosAbiertos,archivoPeticion);
pthread_mutex_unlock(&mutex_TGAA);

t_archivoLocal* archivoLocal = malloc(sizeof (t_archivoLocal));
archivoLocal->archivo = archivoPeticion->archivo;
archivoLocal->ptro = 0;
list_add(archivoPeticion->pcb->tablaArchivosAbiertos,archivoLocal);
enviar_paquete_pcb(archivoPeticion->pcb, fd_cpu, PCB, info_logger);
break;
}
}
pthread_mutex_unlock(&mutex_listaPeticionesArchivos);
*/

char* obtenerNombreArchivo(t_pcb* pcb){
    t_instr *instruccion = list_get(pcb->instr, pcb->programCounter - 1);
    char* nomArch = malloc(strlen(instruccion->param1) + 1);
    strcpy(nomArch, instruccion->param1);
    return nomArch;
}

void eliminarArchivoTGAA(char* nombreArchivo){
    pthread_mutex_lock(&mutex_TGAA);
    for(int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++){
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0){
            list_remove(tablaGlobal_ArchivosAbiertos,i);
        }
        else{
            log_error(error_logger,"El archivo: <%s> que solicitaste eliminar de la TGAA no se encuentra", nombreArchivo);
        }
    }
    pthread_mutex_unlock(&mutex_TGAA);
}

void eliminarPcbTGAA_Y_actualizarTGAA(t_pcb* pcb){
    for(int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++){
        pthread_mutex_lock(&mutex_TGAA);
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        pthread_mutex_unlock(&mutex_TGAA);
        if(archivoPeticion->pcb->id == pcb->id){
            actualizarTGAALuegoDeLiberacionDeArchivo(archivoPeticion->archivo->nombreArchivo);
        }
    }
}

void actualizarTGAALuegoDeLiberacionDeArchivo(char* nombreArchivo){
    //Esta funcion se llama luego de que un proceso libere el archivo, si fija si alguien lo esta solicitando:
    //Si lo estan solicitando se lo asigna en la TGAA al primero en solicitarlo
    //Si no lo estan solicitando eliminar el archivo de la TGAA

    bool archivoNoSolicitado = true;

    for(int i = 0; i < list_size(listaPeticionesArchivos); i++){
        t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0){
            actualizarDuenioTGAA(nombreArchivo,archivoPeticion->pcb);
            archivoNoSolicitado = false;
            break;
        }
    }
    for(int i = 0; i < list_size(listaPeticionesArchivos); i++){
        t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0 && archivoNoSolicitado){
            eliminarArchivoTGAA(nombreArchivo);
        }
    }


}

void actualizarDuenioTGAA(char* nombreArchivo, t_pcb* pcbNuevoDuenio){
    pthread_mutex_lock(&mutex_TGAA);
    for(int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++){
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0){
            archivoPeticion->pcb = pcbNuevoDuenio;
        }
        else{
            log_error(error_logger,"El archivo: <%s> que solicitaste actualizar duenio de la TGAA no se encuentra", nombreArchivo);
        }
    }
    pthread_mutex_unlock(&mutex_TGAA);
}

void eliminarArchivoTablaLocal(char* nombreArchivo, t_pcb* pcb){
    for(int i = 0; i < list_size(pcb->tablaArchivosAbiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->tablaArchivosAbiertos, i);
        if(strcmp(archivoLocal->archivo->nombreArchivo, nombreArchivo) == 0){
            list_remove(pcb->tablaArchivosAbiertos, i);
        }
    }
}

void ejecutar_FCLOSE(t_pcb* pcb) {
    char *nomArch = obtenerNombreArchivo(pcb);
    eliminarArchivoTablaLocal(nomArch, pcb);
    bool hayProcesosEsperandoPorArchivo = false;
    pthread_mutex_lock(&mutex_listaPeticionesArchivos);
    for (int i = 0; i < list_size(listaPeticionesArchivos); i++) {
        t_archivoPeticion *archivoPeticion = list_get(listaPeticionesArchivos, i);
        if (strcmp(archivoPeticion->archivo->nombreArchivo, nomArch) ==
            0) { //Si hay procesos blockeados esperando por ese archivo
            list_remove(listaPeticionesArchivos, i);
            actualizarDuenioTGAA(archivoPeticion->archivo->nombreArchivo, archivoPeticion->pcb);
            moverProceso_BloqReady(archivoPeticion->pcb);
            hayProcesosEsperandoPorArchivo = true;
            break;
        }
    }
    if (!hayProcesosEsperandoPorArchivo) {
        eliminarArchivoTGAA(nomArch);
    }
    pthread_mutex_unlock(&mutex_listaPeticionesArchivos);
    //Por lo que entiendo y el enunciado no aclara Kernel replanifica al desbloquarse el proceso
}


void ejecutar_FSEEK(t_pcb* pcb){
    char* nombreArchivo = obtenerNombreArchivo(pcb);
    t_instr *instruccion = list_get(pcb->instr, pcb->programCounter - 1);
    int ubiPuntero = atoi(instruccion->param2);

    for(int i = 0; i < list_size(pcb->tablaArchivosAbiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->tablaArchivosAbiertos, i);
        if(strcmp(archivoLocal->archivo->nombreArchivo, nombreArchivo) == 0){
            archivoLocal->ptro = ubiPuntero;
        }
    }
    log_info(info_logger,"PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>", pcb->id, nombreArchivo, ubiPuntero);
    enviar_paquete_pcb(pcb,fd_cpu,PCB,info_logger);
}



void ejecutar_FTRUNCATE(t_pcb* pcb){
    //length nombreArchivo,nombreArchivo,tamNew
    //yo te creo estructura con serializacion y desarilizacion

    char* nombreArchivo = obtenerNombreArchivo(pcb);
    t_instr *instruccion = list_get(pcb->instr, pcb->programCounter - 1);
    int nuevoTamanio = atoi(instruccion->param2);

    t_archivoTruncate* archivoParaFs = malloc(sizeof(t_archivoTruncate));
    archivoParaFs->nombreArchivoLength = strlen(nombreArchivo) + 1;
    archivoParaFs->nombreArchivo = nombreArchivo;
    archivoParaFs->nuevoTamanio = nuevoTamanio;

    moverProceso_ExecBloq(pcb);
    enviar_archivoTruncacion(archivoParaFs,fd_filesystem,TRUNCACION_ARCHIVO,info_logger);

    log_info(info_logger,"PID: <%d> - Archivo: <%s> - Tamaño: <%d>", pcb->id, nombreArchivo, nuevoTamanio);
}

uint32_t buscarPosPunteroTablaLocal(char* nombreArchivo, t_pcb* pcb){
    for(int i = 0; i < list_size(pcb->tablaArchivosAbiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->tablaArchivosAbiertos,i);
        if(strcmp(archivoLocal->archivo->nombreArchivo,nombreArchivo) == 0){
            return archivoLocal->ptro;
        }else{
            log_error(error_logger, "El archivo <%s> no se encuentra en la tabla local del PID <%d>", nombreArchivo, pcb->id);
        }
    }
}

void actualizarPunteroLocal(char* nombreArchivo, t_pcb* pcb, uint32_t cantidadBytes){
    for(int i = 0; i < list_size(pcb->tablaArchivosAbiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->tablaArchivosAbiertos,i);
        if(strcmp(archivoLocal->archivo->nombreArchivo,nombreArchivo) == 0){
            archivoLocal->ptro += cantidadBytes;
        }else{
            log_error(error_logger, "El archivo <%s> no se encuentra en la tabla local del PID <%d>", nombreArchivo, pcb->id);
        }
    }
}


void ejecutar_FREAD(t_pcb* pcb, uint32_t direccionFisica){
    char* nombreArchivo = obtenerNombreArchivo(pcb);
    t_instr *instruccion = list_get(pcb->instr, pcb->programCounter - 1);
    int cantidadBytes = atoi(instruccion->param3);

    t_archivoRW* archivoParaFs = malloc(sizeof(t_archivoRW));
    archivoParaFs->nombreArchivoLength = strlen(nombreArchivo) + 1;
    archivoParaFs->nombreArchivo = nombreArchivo;
    archivoParaFs->posPuntero = buscarPosPunteroTablaLocal(nombreArchivo,pcb);
    archivoParaFs->direcFisica = direccionFisica;
    archivoParaFs->cantidadBytes = cantidadBytes;
    archivoParaFs->pid = pcb->id;

    log_info(info_logger,"PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamanio <%d>",pcb->id,nombreArchivo,archivoParaFs->posPuntero,cantidadBytes);

    actualizarPunteroLocal(nombreArchivo,pcb,cantidadBytes);

    moverProceso_ExecBloq(pcb);
    enviar_archivoRW(archivoParaFs,fd_filesystem,LECTURA_ARCHIVO,info_logger);


}

void ejecutar_FWRITE(t_pcb* pcb, uint32_t direccionFisica){
    char* nombreArchivo = obtenerNombreArchivo(pcb);
    t_instr *instruccion = list_get(pcb->instr, pcb->programCounter - 1);
    int cantidadBytes = atoi(instruccion->param3);

    t_archivoRW* archivoParaFs = malloc(sizeof(t_archivoRW));
    archivoParaFs->nombreArchivoLength = strlen(nombreArchivo) + 1;
    archivoParaFs->nombreArchivo = nombreArchivo;
    archivoParaFs->posPuntero = buscarPosPunteroTablaLocal(nombreArchivo,pcb);
    archivoParaFs->direcFisica = direccionFisica;
    archivoParaFs->cantidadBytes = cantidadBytes;
    archivoParaFs->pid = pcb->id;

    log_info(info_logger,"PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamanio <%d>",pcb->id,nombreArchivo,archivoParaFs->posPuntero,cantidadBytes);

    actualizarPunteroLocal(nombreArchivo,pcb,cantidadBytes);

    moverProceso_ExecBloq(pcb);
    enviar_archivoRW(archivoParaFs,fd_filesystem,F_READ,info_logger);

}


int buscarArch_TablaGlobalArchivo(char* nomArch){
    int size = list_size(tablaGlobal_ArchivosAbiertos);
    t_archivoPeticion* archivoPeticion;

    for (int i = 0; i < size ; i++)
    {
        archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        if (strcmp(archivoPeticion->archivo->nombreArchivo, nomArch) == 0)
        {
            return i;
        }
    }
    
    return -1;
}

void agregarEntrada_TablaGlobalArchivosAbiertos(char* nomArch){

    pthread_mutex_lock(&mutex_listaPeticionesArchivos);

    for(int i = 0 ; i < list_size(listaPeticionesArchivos); i++){
        t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo,nomArch)== 0){
            list_remove(listaPeticionesArchivos,i);

            pthread_mutex_lock(&mutex_TGAA);
            list_add(tablaGlobal_ArchivosAbiertos,archivoPeticion);
            pthread_mutex_unlock(&mutex_TGAA);

            t_archivoLocal* archivoLocal = malloc(sizeof (t_archivoLocal));
            archivoLocal->archivo = archivoPeticion->archivo;
            archivoLocal->ptro = 0;
            list_add(archivoPeticion->pcb->tablaArchivosAbiertos,archivoLocal);
            enviar_paquete_pcb(archivoPeticion->pcb, fd_cpu, PCB, info_logger);
            break;
        }
    }
    pthread_mutex_unlock(&mutex_listaPeticionesArchivos);

}


void desbloquearPcb_porNombreArchivo (char* nombArch) {

    pthread_mutex_lock(&mutex_TGAA);
    for (int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++) {
        t_archivoPeticion *archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos, i);
        if (strcmp(archivoPeticion->archivo->nombreArchivo, nombArch) == 0) {
            moverProceso_BloqReady(archivoPeticion->pcb);
        }
    }
    pthread_mutex_unlock(&mutex_TGAA);
}