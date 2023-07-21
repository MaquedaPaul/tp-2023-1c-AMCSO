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
  //pcb->tablaArchivosAbiertos = list_create();


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


void ejecutar_FOPEN_socket(int socket_entrada){

    int size;
	void * buffer = recibir_buffer(&size, socket_entrada);
    uint32_t* desplazamiento = 0;
    
    uint32_t largoNomArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
    char* nomArch;

    memcpy(nomArch, buffer + *desplazamiento, largoNomArch + 1);
    desplazamiento += largoNomArch + 1;

    t_pcb *pcbRecibido = recibir_paquete_con_PCB(desplazamiento, buffer);

	free(buffer);

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nomArch);

    if(pos == -1){ // En caso de que no este en la TGAA
        enviarString(nomArch, fd_filesystem, APERTURA_ARCHIVO, info_logger);
        list_add(tabla_PeticionesFS,pcbRecibido);

    }
    else{//Si esta en la TGAA
        t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);
        if(archivo->enUso){ //Si esta en uso x otro pcb
            list_add(archivo->lista_espera_pcbs,pcbRecibido);

            moverProceso_ExecBloq(pcbRecibido);
        }
        else{//Si no esta en uso, se pasa al siguiente pcb
            archivo->enUso = true;
            archivo->id_pcb_en_uso = pcbRecibido->id;
            archivo->ptro=0;
            list_replace(tablaGlobal_ArchivosAbiertos,pos, archivo);
           //TODO list_add(pcbRecibido->tablaArchivosAbiertos,nomArch); //VERIFICAR
            enviar_paquete_pcb(pcbRecibido,fd_cpu,PCB,info_logger);

            log_info(info_logger, "PID: <%d> - Abrir Archivo: <%s>", 
                        archivo->id_pcb_en_uso, nomArch);
        }
    }

    pthread_mutex_unlock(&mutex_TGAA);

}

void ejecutar_FOPEN(t_pcb* pcbRecibido){

    t_instr* instruccion = list_get(pcbRecibido->instr,pcbRecibido->programCounter-1);
    char* nomArch;
    strcpy(nomArch, instruccion->param1);
    
    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nomArch);

    if(pos == -1){ // En caso de que no este en la TGAA
        enviarString(nomArch, fd_filesystem, APERTURA_ARCHIVO, info_logger);
        list_add(tabla_PeticionesFS,pcbRecibido);

    }
    else{//Si esta en la TGAA
        t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);

        if(archivo->enUso){ //Si esta en uso x otro pcb
            list_add(archivo->lista_espera_pcbs,pcbRecibido);
            moverProceso_ExecBloq(pcbRecibido);
        }
        else{//Si no esta en uso, se pasa al siguiente pcb
            archivo->enUso = true;
            archivo->id_pcb_en_uso = pcbRecibido->id;
            archivo->ptro=0;
            list_replace(tablaGlobal_ArchivosAbiertos,pos, archivo);
           //TODO list_add(pcbRecibido->tablaArchivosAbiertos,nomArch); //VERIFICAR
            enviar_paquete_pcb(pcbRecibido,fd_cpu,PCB,info_logger);

            log_info(info_logger, "PID: <%d> - Abrir Archivo: <%s>", 
                        archivo->id_pcb_en_uso, nomArch);
        }
    }

    pthread_mutex_unlock(&mutex_TGAA);

}



void ejecutar_FCLOSE(t_pcb*  pcbRecibido){

    t_instr* instruccion = list_get(pcbRecibido->instr,pcbRecibido->programCounter-1);
    char* nombreArchivo;
    strcpy(nombreArchivo, instruccion->param1);

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nombreArchivo);
    t_TablaArchivos* archivo =  list_get(tablaGlobal_ArchivosAbiertos,pos);
    archivo->enUso = false;

    log_info(info_logger, "PID: <%d> - Cerrar Archivo: <%s>", 
                archivo->id_pcb_en_uso, nombreArchivo);

    
    enviar_paquete_pcb(pcbRecibido,fd_cpu,PCB,info_logger);

    if(list_is_empty(archivo->lista_espera_pcbs)){
        list_remove(tablaGlobal_ArchivosAbiertos,pos);
        pthread_mutex_unlock(&mutex_TGAA);
    }
    else{
        t_pcb* pcbRecibido2 = list_get(archivo->lista_espera_pcbs,0);
        list_remove(archivo->lista_espera_pcbs,0);
        list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);

        pthread_mutex_unlock(&mutex_TGAA);
        ejecutar_FOPEN(pcbRecibido2);
        moverProceso_BloqReady(pcbRecibido2);
    }

}

void ejecutar_FCLOSE_porNombreArchivo(t_pcb* pcbBuscado, char* nombreArchivo){

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nombreArchivo);
    t_TablaArchivos* archivo =  list_get(tablaGlobal_ArchivosAbiertos,pos);
    archivo->enUso = false;

    log_info(info_logger, "PID: <%d> - Cerrar Archivo: <%s>", 
                archivo->id_pcb_en_uso, nombreArchivo);
    
    enviar_paquete_pcb(pcbBuscado,fd_cpu,PCB,info_logger);

    if(list_is_empty(archivo->lista_espera_pcbs)){
        list_remove(tablaGlobal_ArchivosAbiertos,pos);
        pthread_mutex_unlock(&mutex_TGAA);
    }
    else{
        t_pcb* pcbBuscado2 = list_get(archivo->lista_espera_pcbs,0);
        list_remove(archivo->lista_espera_pcbs,0);
        list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);

        pthread_mutex_unlock(&mutex_TGAA);
        ejecutar_FOPEN(pcbBuscado2);
        moverProceso_BloqReady(pcbBuscado2);
    }

}


void ejecutar_FSEEK(t_pcb* pcbRecibido){

    t_instr* instruccion = list_get(pcbRecibido->instr,pcbRecibido->programCounter-1);
    char* nombreArchivo;
    strcpy(nombreArchivo, instruccion->param1);

    uint32_t punteroRecibido = instruccion->param2;

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nombreArchivo);
    t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);
    archivo->ptro=punteroRecibido;
    list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);
    pthread_mutex_unlock(&mutex_TGAA);

    log_info(info_logger, "PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>", 
                archivo->id_pcb_en_uso, nombreArchivo, punteroRecibido);

    enviar_paquete_pcb(pcbRecibido,fd_cpu,PCB,info_logger);
    
}


void ejecutar_FTRUNCATE(t_pcb* pcbRecibido){


    t_instr* instruccion = list_get(pcbRecibido->instr,pcbRecibido->programCounter-1);
    char* nomArchivo;
    strcpy(nomArchivo, instruccion->param1);
    uint32_t tamanioArch = instruccion->param2;

    uint32_t tamDatos = sizeof(int) + strlen(nomArchivo) + 1;
    void* datos = malloc(tamDatos);
    memcpy(datos, &tamanioArch, sizeof(int));
    memcpy(datos + sizeof(int)  , nomArchivo, strlen(nomArchivo) + 1);
    enviarDatos(datos,tamDatos, TRUNCACION_ARCHIVO,fd_filesystem , info_logger);
    free(datos);

    pthread_mutex_lock(&mutex_TGAA);
    t_TablaArchivos* archivo = buscarEntrada_TablaGlobalArchivo(nomArchivo);
    pthread_mutex_unlock(&mutex_TGAA);
    log_info(info_logger, "PID: <%d> - Truncar Archivo: <%s> - Tamaño <%d>", 
                archivo->id_pcb_en_uso, nomArchivo, tamanioArch);

    
    moverProceso_ExecBloq(pcbRecibido);
    
}


void ejecutar_FREAD(int socket_entrada){

    int size;
	void * buffer = recibir_buffer(&size, socket_entrada);
    uint32_t *desplazamiento = 0;
    
    uint32_t largoNomArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
    char* nomArchivo;

    memcpy(nomArchivo, buffer + *desplazamiento, largoNomArch + 1);
    desplazamiento += largoNomArch + 1;

    uint32_t dlArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
   
    uint32_t largArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
    
    t_pcb *pcbRecibido = recibir_paquete_con_PCB(desplazamiento, buffer);

	free(buffer);
   
    uint32_t tamDatos = sizeof(int) + sizeof(int) + strlen(nomArchivo) + 1;
    void* datos = malloc(tamDatos);
    memcpy(datos, &largArch, sizeof(int));
    memcpy(datos + sizeof(int) , &dlArch, sizeof(int));
    memcpy(datos + sizeof(int) + sizeof(int) , nomArchivo, strlen(nomArchivo) + 1);
    enviarDatos(datos,tamDatos, LECTURA_ARCHIVO,fd_filesystem , info_logger);
    free(datos);

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nomArchivo);
    t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);
    log_info(info_logger, "PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Direccion Memoria <%d> - Tamaño <%d>", 
                archivo->id_pcb_en_uso, nomArchivo, archivo->ptro,dlArch,largArch);
    
    //Se actualiza posicion del puntero
    archivo->ptro += largArch;
    list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);
    pthread_mutex_unlock(&mutex_TGAA);

    moverProceso_ExecBloq(pcbRecibido);
  
}

void ejecutar_FWRITE(int socket_entrada){

    int size;
	void * buffer = recibir_buffer(&size, socket_entrada);
    uint32_t *desplazamiento = 0;
    
    uint32_t largoNomArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
    char* nomArchivo;

    memcpy(nomArchivo, buffer + *desplazamiento, largoNomArch + 1);
    desplazamiento += largoNomArch + 1;

    uint32_t dfArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
    
    uint32_t largArch = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento);
    
    t_pcb *pcbRecibido = recibir_paquete_con_PCB(desplazamiento, buffer);

	free(buffer);
   
    uint32_t tamDatos = sizeof(int) + sizeof(int) + strlen(nomArchivo) + 1;
    void* datos = malloc(tamDatos);
    memcpy(datos, &largArch, sizeof(int));
    memcpy(datos + sizeof(int) , &dfArch, sizeof(int));
    memcpy(datos + sizeof(int) + sizeof(int) , nomArchivo, strlen(nomArchivo) + 1);
    enviarDatos(datos,tamDatos, ESCRITURA_ARCHIVO,fd_filesystem , info_logger);
    free(datos);

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nomArchivo);
    t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);
    log_info(info_logger, "PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Direccion Memoria <%d> - Tamaño <%d>", 
                archivo->id_pcb_en_uso, nomArchivo, archivo->ptro,dfArch,largArch);
  
    //Se actualiza posicion del puntero
    archivo->ptro += largArch;
    list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);
    pthread_mutex_unlock(&mutex_TGAA);

    moverProceso_ExecBloq(pcbRecibido);
}



int buscarArch_TablaGlobalArchivo(char* nomArch){
    int size = list_size(tablaGlobal_ArchivosAbiertos);
    t_TablaArchivos* archivo;

    for (size_t i = 0; i < size ; i++)
    {
        archivo = list_get(tablaGlobal_ArchivosAbiertos,i);
        if (strcmp(archivo->nombreArchivo, nomArch) == 0)
        {
            return i;
        }
    }
    
    return -1;
}


t_pcb* buscarPcb_enTablaPeticionesFS(char* nombreArchBuscado){

    int size = list_size(tabla_PeticionesFS);
    t_pcb* pcbEncontrado;
    t_instr* instruccion;

    for (size_t i = 0; i < size ; i++)
    {
        pcbEncontrado = list_get(tabla_PeticionesFS,i);
        instruccion = list_get(pcbEncontrado->instr,pcbEncontrado->programCounter-1);
      
        if (strcmp(instruccion->param1, nombreArchBuscado) == 0)
        {
            list_remove(tabla_PeticionesFS,i);
            return pcbEncontrado;
        }
    }

    pcbEncontrado = list_get(tabla_PeticionesFS,0);

  return pcbEncontrado;
}

t_TablaArchivos* buscarEntrada_TablaGlobalArchivo(char* nomArch){
    int size = list_size(tablaGlobal_ArchivosAbiertos);
    t_TablaArchivos *archivo;

    archivo = list_get(tablaGlobal_ArchivosAbiertos,0);
    for (size_t i = 0; i < size ; i++)
    {
        archivo = list_get(tablaGlobal_ArchivosAbiertos,i);
        return archivo;
    }
    
    return archivo;
}

void agregarEntrada_TablaGlobalArchivosAbiertos(char* nomArch){

    t_pcb* pcbBuscado = buscarPcb_enTablaPeticionesFS(nomArch);
    t_TablaArchivos* archivo;
    strcpy(archivo->nombreArchivo, nomArch);
    archivo->enUso = true;
    archivo->id_pcb_en_uso = pcbBuscado->id;
    archivo->ptro=0;
    archivo->lista_espera_pcbs = list_create();
    
    list_add(tablaGlobal_ArchivosAbiertos, archivo);
    //TODO list_add(pcbBuscado->tablaArchivosAbiertos,nomArch); //VERIFICAR
    enviar_paquete_pcb(pcbBuscado,fd_cpu,PCB,info_logger);
}

void eliminarPcb_TGAA_SEGFAULT(t_pcb* pcbBuscado){

    pthread_mutex_lock(&mutex_TGAA);
    int size = list_size(tablaGlobal_ArchivosAbiertos);
    t_TablaArchivos *archivo;
    int id = pcbBuscado->id;

    for (size_t i = 0; i < size ; i++)
    {
        archivo = list_get(tablaGlobal_ArchivosAbiertos,i);

        if(archivo->id_pcb_en_uso == id){//valido primero si esta en uso
            ejecutar_FCLOSE(pcbBuscado);
        }
        else{//valido la lista de espera para eliminarlo
            int count = list_size(archivo->lista_espera_pcbs);
            int id_aux;
            for (size_t x = 0; x < count; x++)
                {
                    id_aux = *(int*)list_get(archivo->lista_espera_pcbs, x);
                    if(id == id_aux){  
                        list_remove(archivo->lista_espera_pcbs,x);
                        x = count;
                    }
                }
            }
    }

    pthread_mutex_unlock(&mutex_TGAA);

}


void desbloquearPcb_porNombreArchivo (char* nombArch){

    t_TablaArchivos *archivo = buscarEntrada_TablaGlobalArchivo(nombArch);
    int id_buscado = archivo->id_pcb_en_uso;
    t_pcb* pcbBuscado;

    pthread_mutex_lock(&mutex_colaBloq);
    int count = list_size(colaBloq);
    for (size_t i = 0; i < count; i++)
    {
        pcbBuscado = list_get(colaBloq, i);
        if (id_buscado == pcbBuscado->id)
        {
            i = count;
        }
        
    }

    pthread_mutex_unlock(&mutex_colaBloq);
    moverProceso_BloqReady(pcbBuscado);

}