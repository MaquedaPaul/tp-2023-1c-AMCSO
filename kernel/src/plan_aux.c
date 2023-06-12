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
4.FTRUNCATE no bloquea el proceso, actualemennte todos los procesos se 
    bloquean --> se envia lista [tamArch,nombreArch]
5.FREAD no bloquea el proceso, actualemennte todos los procesos se 
    bloquean --> se envia lista [ptro,largo,dl,nombreArch]
6.FWRITE no bloquea el proceso, actualemennte todos los procesos se 
    bloquean --> se envia lista [largo,dF,nombreArch]

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

    1. VERIFICAR como se reciben los paquetes --> Agregar campo a pcb o crear nuevo 
        recibir (300lineas) [depende el caso]
    2. importar .c .h
    3. Aplicar mutex sobre tablaGlobal
    4. verificar bloq de pcb x filesystem o devolver a exec --> AVERIGUAR que hacer con la pcb
    5. FSEEK: que tiene que avisar a FS o CPU?
    7. FTRUNCATE FREAD FWRITE no bloquea el proceso en si,ya que actualemente todos los procesos se bloquean
    8. COORDINAR CON COMPACTACION
    9. Log fclose y fopen cuando efectivamente se cierra/abre o cuando es llamado?

*/



void ejecutar_FOPEN(t_pcb* pcbRecibido,char* nomArch){
    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nomArch);
    t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);

    if(pos == -1){
        enviarString(nomArch, fd_filesystem, APERTURA_ARCHIVO, info_logger);
        t_peticionesFS* peticion;
        peticion->pcb=pcbRecibido;
        peticion->nombreArchivo=nomArch;
        list_add(tabla_PeticionesFS,peticion);

    }
    else{
        if(archivo->enUso){
            list_add(archivo->lista_espera_pcbs,pcbRecibido);
        }
        else{
            archivo->enUso = true;
            archivo->id_pcb_en_uso = pcbRecibido->id;
            archivo->ptro=0;
            list_replace(tablaGlobal_ArchivosAbiertos,pos, archivo);
            list_add(pcbRecibido->tablaArchivosAbiertos,nomArch); //VERIFICAR
            enviar_paquete_pcb(pcbRecibido,fd_cpu,APERTURA_ARCHIVO_EXITOSA,info_logger);
        }
    }

    pthread_mutex_unlock(&mutex_TGAA);

    log_info(info_logger, "PID: <%d> - Abrir Archivo: <%s>", 
                archivo->id_pcb_en_uso, nomArch);

}



void ejecutar_FCLOSE(char* nombreArchivo){
    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nombreArchivo);
    t_TablaArchivos* archivo =  list_get(tablaGlobal_ArchivosAbiertos,pos);

    log_info(info_logger, "PID: <%d> - Cerrar Archivo: <%s>", 
                archivo->id_pcb_en_uso, nombreArchivo);

    if(list_is_empty(archivo->lista_espera_pcbs)){
        list_remove(tablaGlobal_ArchivosAbiertos,pos);
        pthread_mutex_unlock(&mutex_TGAA);
    }
    else{
        t_pcb* pcbBuscado = list_get(archivo->lista_espera_pcbs,0);
        list_remove(archivo->lista_espera_pcbs,0);
        list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);

        pthread_mutex_unlock(&mutex_TGAA);
        ejecutar_FOPEN(pcbBuscado, nombreArchivo);
    }

}


void ejecutar_FSEEK(char* nombreArchivo, int punteroRecibido){

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nombreArchivo);
    t_TablaArchivos* archivo = list_get(tablaGlobal_ArchivosAbiertos,pos);
    archivo->ptro=punteroRecibido;
    list_replace(tablaGlobal_ArchivosAbiertos,pos,archivo);
    pthread_mutex_unlock(&mutex_TGAA);

    log_info(info_logger, "PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>", 
                archivo->id_pcb_en_uso, nombreArchivo, punteroRecibido);
    
}


void ejecutar_FTRUNCATE(char* nomArchivo, int tamanioArch){

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
    
}


void ejecutar_FREAD(char* nomArchivo, int largArch, int dlArch){
   
    uint32_t tamDatos = sizeof(int) + sizeof(int) + strlen(nomArchivo) + 1;
    void* datos = malloc(tamDatos);
    memcpy(datos, &largArch, sizeof(int));
    memcpy(datos + sizeof(int) , &dlArch, sizeof(int));
    memcpy(datos + sizeof(int) + sizeof(int) , nomArchivo, strlen(nomArchivo) + 1);
    enviarDatos(datos,tamDatos, LECTURA_ARCHIVO,fd_filesystem , info_logger);
    free(datos);

    pthread_mutex_lock(&mutex_TGAA);
    t_TablaArchivos* archivo = buscarEntrada_TablaGlobalArchivo(nomArchivo);
    pthread_mutex_unlock(&mutex_TGAA);
    log_info(info_logger, "PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Direccion Memoria <%d> - Tamaño <%d>", 
                archivo->id_pcb_en_uso, nomArchivo, archivo->ptro,dlArch,largArch);
  
}

void ejecutar_FWRITE(char* nomArchivo, int largArch, int dfArch){
   
    uint32_t tamDatos = sizeof(int) + sizeof(int) + strlen(nomArchivo) + 1;
    void* datos = malloc(tamDatos);
    memcpy(datos, &largArch, sizeof(int));
    memcpy(datos + sizeof(int) , &dfArch, sizeof(int));
    memcpy(datos + sizeof(int) + sizeof(int) , nomArchivo, strlen(nomArchivo) + 1);
    enviarDatos(datos,tamDatos, ESCRITURA_ARCHIVO,fd_filesystem , info_logger);
    free(datos);

    pthread_mutex_lock(&mutex_TGAA);
    t_TablaArchivos* archivo = buscarEntrada_TablaGlobalArchivo(nomArchivo);
    pthread_mutex_unlock(&mutex_TGAA);
    log_info(info_logger, "PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Direccion Memoria <%d> - Tamaño <%d>", 
                archivo->id_pcb_en_uso, nomArchivo, archivo->ptro,dfArch,largArch);
  
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
    t_peticionesFS* peticion;
    t_pcb* pcbEncontrado;

    for (size_t i = 0; i < size ; i++)
    {
        peticion = list_get(tabla_PeticionesFS,i);
        if (strcmp(peticion->nombreArchivo, nombreArchBuscado) == 0)
        {
            list_remove(tabla_PeticionesFS,i);
            pcbEncontrado = peticion->pcb;
            return pcbEncontrado;
        }
    }

    pcbEncontrado = peticion->pcb;

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
    archivo->nombreArchivo = nomArch;
    archivo->enUso = true;
    archivo->id_pcb_en_uso = pcbBuscado->id;
    archivo->ptro=0;
    archivo->lista_espera_pcbs = list_create();
    
    list_add(tablaGlobal_ArchivosAbiertos, archivo);
    list_add_in_index(pcbBuscado->tablaArchivosAbiertos,0,nomArch); //VERIFICAR
    enviar_paquete_pcb(pcbBuscado,fd_cpu,APERTURA_ARCHIVO_EXITOSA,info_logger);
}
