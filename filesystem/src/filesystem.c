//
// Created by utnso on 4/29/23.
//

#include <filesystem.h>
#include <pthread.h>

int fd_memoria;
int fd_kernel;
bool kernelInicializado = false;
pthread_mutex_t mutex_ArchivosUsados;

void* abrirArchivo(void* cliente_socket){

    int conexion = *((int*) cliente_socket);
    char* nombreArchivo = recibirString(conexion);

    if(kernelInicializado == false){
        fd_kernel= conexion;
        kernelInicializado = true;
    }
    if(existe_archivoFCB(nombreArchivo)){
        enviarString(nombreArchivo,conexion,APERTURA_ARCHIVO_EXITOSA,info_logger);
    }else{
        enviarString(nombreArchivo,conexion,APERTURA_ARCHIVO_FALLIDA,info_logger);
    }
    free(nombreArchivo);
}

void* crearArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    char* nombreArchivo = recibirString(conexion);
    log_debug(debug_logger,"nombre archivo: %s", nombreArchivo);
    realizarCreacionArchivo(nombreArchivo); //TODO ACA ARROJA SISEGV
    enviarString(nombreArchivo,conexion,CREACION_ARCHIVO_EXITOSA,info_logger);
    free(nombreArchivo);
}


void* truncarArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_archivoTruncate* archivoTruncacion = recibir_archivoTruncacion(conexion);
    realizarTruncacionArchivo(archivoTruncacion->nombreArchivo, archivoTruncacion-> nuevoTamanio);
    enviarString(archivoTruncacion->nombreArchivo,conexion,TRUNCACION_ARCHIVO_EXITOSA,info_logger);
    free(archivoTruncacion->nombreArchivo);
    free(archivoTruncacion);
}

void* leerArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_archivoRW* archivo = recibir_archivoRW(conexion);
    lecturaArchivo(archivo->nombreArchivo,archivo->posPuntero,archivo->direcFisica, archivo->cantidadBytes);

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = buscarFCBporNombre(archivo->nombreArchivo); //solo agrego a lista los archivos que se lee o escribe
    list_add(archivosUsados, fcb);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    uint32_t pid = archivo->pid;
    t_datos* datosAEnviar = malloc(sizeof(t_datos));
    datosAEnviar->tamanio = archivo->cantidadBytes;

    log_debug(debug_logger, "puntero archivo: %d", archivo->posPuntero);
    log_debug(debug_logger, "cantidad de bytes a leer: %d", archivo->cantidadBytes);
    datosAEnviar->datos = realizarLecturaArchivo(archivo->nombreArchivo, archivo->posPuntero, archivo->cantidadBytes);
    t_list* listaInts = list_create();
    list_add(listaInts, &archivo->direcFisica);
    list_add(listaInts, &pid);

    log_debug(debug_logger, "se solicita a memoria escrbir en df");
    enviarListaIntsYDatos(listaInts,datosAEnviar,fd_memoria,info_logger,ACCESO_PEDIDO_ESCRITURA);
    list_clean(listaInts);
    list_destroy(listaInts);
    free(archivo->nombreArchivo);
    free(archivo);
    free(datosAEnviar->datos);
    free(datosAEnviar);
}

void* escribirArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    t_archivoRW* archivo = recibir_archivoRW(conexion);

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb =buscarFCBporNombre(archivo->nombreArchivo);
    list_add(archivosUsados, fcb); //solo agrego a lista los archivos que se lee o escribe
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    uint32_t pid = archivo->pid;
    t_list* listaInts = list_create();
    list_add(listaInts,  &archivo->direcFisica);
    list_add(listaInts,  &archivo->cantidadBytes);
    list_add(listaInts, &pid);
    list_add(listaInts, &archivo->posPuntero);

    log_debug(debug_logger, "se solicita a memoria leer en df");
    enviarListaUint32_t(listaInts,fd_memoria,info_logger, ACCESO_PEDIDO_LECTURA);
    list_clean(listaInts);
    list_destroy(listaInts);
    free(archivo->nombreArchivo);
    free(archivo);

}

void* finalizarEscrituraArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(conexion, unosDatos);
    uint32_t* direccionFisica = list_get(listaInts,0);
    uint32_t pid = *(uint32_t*)list_get(listaInts,1);
    uint32_t punteroArchivo = *(uint32_t*)list_get(listaInts,2);

    char* nombreArchivo = obtenerPrimerArchivoUsado();
    escrituraArchivo(nombreArchivo, punteroArchivo, *direccionFisica, unosDatos->tamanio);
    realizarEscrituraArchivo(nombreArchivo,  punteroArchivo, unosDatos->datos, unosDatos->tamanio);
    enviarString(nombreArchivo,fd_kernel,ESCRITURA_ARCHIVO_EXITOSA, info_logger);
    free(unosDatos->datos);
    free(unosDatos);
    free(nombreArchivo);
}


void* finalizarLecturaArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    char* nombreArchivo = obtenerPrimerArchivoUsado();
    recibirOrden(conexion);

    enviarString(nombreArchivo, fd_kernel, LECTURA_ARCHIVO_EXITOSA, info_logger);
}


char* obtenerPrimerArchivoUsado() {

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = list_remove(archivosUsados, 0);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    return fcb->NOMBRE_ARCHIVO;
}
