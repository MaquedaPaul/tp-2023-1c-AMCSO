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
    char* nombreArchivo;
    uint32_t nuevoTamanio;
    uint32_t tamanioNombreArchivo;

    //recibir: 1 tamanio a modificar, 2 tam nombre archivo, 3 nombre archivo
    /*
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, conexion);
    void* datos;
    memcpy(&nuevoTamanio, buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento+=sizeof(uint32_t);
    memcpy(&tamanioNombreArchivo, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    nombreArchivo= malloc(tamanioNombreArchivo);
    memcpy(nombreArchivo, buffer + desplazamiento, tamanioNombreArchivo);
    free(buffer);
*/
    realizarTruncacionArchivo(nombreArchivo, nuevoTamanio);
    enviarString(nombreArchivo,conexion,TRUNCACION_ARCHIVO_EXITOSA,info_logger);
}

void* leerArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    char* nombreArchivo;
    uint32_t puntero;
    uint32_t tamanio;
    uint32_t direccionFisica;
    uint32_t pid;
    recibirParamsParaLecturaEscrituraArchivo(nombreArchivo, puntero, tamanio, direccionFisica, pid, conexion);
    t_datos* datosAEnviar = malloc(sizeof(t_datos));
    datosAEnviar->tamanio = tamanio;
    datosAEnviar->datos = realizarLecturaArchivo(nombreArchivo, puntero, tamanio);
    t_list* listaInts = list_create();
    list_add(listaInts, &direccionFisica);
    list_add(listaInts, &pid);

    lecturaArchivo(nombreArchivo,puntero,direccionFisica, tamanio);

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo); //solo agrego a lista los archivos que se lee o escribe

    pthread_mutex_lock(&mutex_ArchivosUsados);
    list_add(archivosUsados, fcb);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    enviarListaIntsYDatos(listaInts,datosAEnviar,fd_memoria,info_logger,ACCESO_PEDIDO_ESCRITURA);
}


void* escribirArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);
    char* nombreArchivo;
    uint32_t puntero;
    uint32_t tamanio;
    uint32_t pid;
    uint32_t direccion;
    recibirParamsParaLecturaEscrituraArchivo(nombreArchivo, punteroPendiente, tamanio, pid, direccion, conexion);
    t_list* listaInts = list_create();
    list_add(listaInts,  &direccion);
    list_add(listaInts,  &tamanio);
    list_add(listaInts, &pid);

    escrituraArchivo(nombreArchivo, puntero, direccion, tamanio);

    t_config_fcb* fcb =buscarFCBporNombre(nombreArchivo);
    pthread_mutex_lock(&mutex_ArchivosUsados);
    list_add(archivosUsados, fcb); //solo agrego a lista los archivos que se lee o escribe
    pthread_mutex_unlock(&mutex_ArchivosUsados);

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
