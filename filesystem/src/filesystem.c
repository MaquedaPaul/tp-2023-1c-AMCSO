//
// Created by utnso on 4/29/23.
//

#include <filesystem.h>
#include <pthread.h>

int fd_memoria;
int fd_kernel;
uint32_t punteroPendiente;
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
}

void* crearArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    char* nombreArchivo = recibirString(conexion);
    realizarCreacionArchivo(nombreArchivo);
    enviarString(nombreArchivo,conexion,CREACION_ARCHIVO_EXITOSA,info_logger);
}


void* truncarArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_archivoTruncate* archivoTruncacion = recibir_archivoTruncacion(conexion);
    realizarTruncacionArchivo(archivoTruncacion->nombreArchivo, archivoTruncacion-> nuevoTamanio);
    enviarString(archivoTruncacion->nombreArchivo,conexion,TRUNCACION_ARCHIVO_EXITOSA,info_logger);
}

void* leerArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_archivoRW* archivo = recibir_archivoRW(conexion);
    lecturaArchivo(archivo->nombreArchivo,archivo->posPuntero,archivo->direcFisica, archivo->cantidadBytes);

    t_config_fcb* fcb = buscarFCBporNombre(archivo->nombreArchivo); //solo agrego a lista los archivos que se lee o escribe

    pthread_mutex_lock(&mutex_ArchivosUsados);
    list_add(archivosUsados, fcb);
    pthread_mutex_unlock(&mutex_ArchivosUsados);


    uint32_t pid=0;
    t_datos* datosAEnviar = malloc(sizeof(t_datos));
    datosAEnviar->tamanio = archivo->cantidadBytes;
    datosAEnviar->datos = realizarLecturaArchivo(archivo->nombreArchivo, archivo->posPuntero, archivo->cantidadBytes);
    t_list* listaInts = list_create();
    list_add(listaInts, &archivo->direcFisica);
    list_add(listaInts, &pid);


    enviarListaIntsYDatos(listaInts,datosAEnviar,fd_memoria,info_logger,ACCESO_PEDIDO_ESCRITURA);
}


void* escribirArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    t_archivoRW* archivo = recibir_archivoRW(conexion);
    escrituraArchivo(archivo->nombreArchivo, archivo->posPuntero, archivo->direcFisica, archivo->cantidadBytes);

    t_config_fcb* fcb =buscarFCBporNombre(archivo->nombreArchivo);
    pthread_mutex_lock(&mutex_ArchivosUsados);
    list_add(archivosUsados, fcb); //solo agrego a lista los archivos que se lee o escribe
    pthread_mutex_unlock(&mutex_ArchivosUsados);


    uint32_t pid=0;

    t_list* listaInts = list_create();
    list_add(listaInts,  &archivo->direcFisica);
    list_add(listaInts,  &archivo->cantidadBytes);
    list_add(listaInts, &pid);

    enviarListaUint32_t(listaInts,fd_memoria,info_logger, ACCESO_PEDIDO_LECTURA);
}

void* finalizarEscrituraArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    uint32_t tamanioDatos;
    uint32_t puntero;
    void* datos = recibirDatos(conexion, tamanioDatos);
    char* nombreArchivo = obtenerPrimerArchivoUsado();

    realizarEscrituraArchivo(nombreArchivo,  puntero, datos, tamanioDatos);
    enviarString(nombreArchivo,fd_kernel,ESCRITURA_ARCHIVO_EXITOSA, info_logger);
}

void* finalizarLecturaArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    char* nombreArchivo = obtenerPrimerArchivoUsado();
    enviarString(nombreArchivo, fd_kernel, LECTURA_ARCHIVO_EXITOSA, info_logger);
}


char* obtenerPrimerArchivoUsado() {

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = list_get(archivosUsados, 0);
    list_remove(archivosUsados,0);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    return fcb->NOMBRE_ARCHIVO;
}
