//
// Created by utnso on 4/29/23.
//

#include <filesystem.h>

int fd_memoria;
int fd_kernel;
//bool kernelInicializado = false;


//SE EJECUTAN LAS PETICIONES

void ejecutar_fopen(char* nombreArchivo){

    if(existe_archivoFCB(nombreArchivo)){
        enviarString(nombreArchivo,fd_kernel,APERTURA_ARCHIVO_EXITOSA,info_logger);
    }else{
        enviarString(nombreArchivo,fd_kernel,APERTURA_ARCHIVO_FALLIDA,info_logger);
    }
    free(nombreArchivo);
}

void ejecutarCreacionArchivo(char* nombreArchivo){

    log_debug(debug_logger,"nombre archivo: %s", nombreArchivo);
    realizarCreacionArchivo(nombreArchivo);
    enviarString(nombreArchivo,fd_kernel,CREACION_ARCHIVO_EXITOSA,info_logger);
    free(nombreArchivo);
}

void ejecutarFwrite(char* nombreArchivo, uint32_t pid, uint32_t puntero, uint32_t tamanio, uint32_t direcFisica){

    escrituraArchivo(nombreArchivo, puntero, direcFisica, tamanio);

    t_list* listaInts = list_create();
    list_add(listaInts,  &direcFisica);
    list_add(listaInts,  &tamanio);
    list_add(listaInts, &pid);
    list_add(listaInts, &puntero);

    log_debug(debug_logger, "se solicita a memoria leer en df");
    enviarListaUint32_t(listaInts,fd_memoria,info_logger, ACCESO_PEDIDO_LECTURA);
    list_clean(listaInts);
    list_destroy(listaInts);
    free(nombreArchivo);

}

void ejecutarFread(char* nombreArchivo, uint32_t pid, uint32_t puntero, uint32_t tamanio, uint32_t direcFisica){

    lecturaArchivo(nombreArchivo,puntero,direcFisica, tamanio);

    t_datos* datosAEnviar = malloc(sizeof(t_datos));
    datosAEnviar->tamanio = tamanio;

    datosAEnviar->datos = realizarLecturaArchivo(nombreArchivo, puntero, tamanio);
    t_list* listaInts = list_create();
    list_add(listaInts, &direcFisica);
    list_add(listaInts, &pid);

    log_debug(debug_logger, "Se solicita a memoria escrbir en df");
    enviarListaIntsYDatos(listaInts,datosAEnviar,fd_memoria,info_logger,ACCESO_PEDIDO_ESCRITURA);
    list_clean(listaInts);
    list_destroy(listaInts);
    free(nombreArchivo);
    free(datosAEnviar->datos);
    free(datosAEnviar);
}

void ejecutarFtruncate(char* nombreArchivo, uint32_t tamanioNuevo){

    realizarTruncacionArchivo(nombreArchivo, tamanioNuevo);
    uint32_t size = list_size(peticiones_pendientes);
    enviarEnteroYString(size,nombreArchivo, strlen(nombreArchivo),fd_kernel,info_logger,ESCRITURA_ARCHIVO_EXITOSA);
    free(nombreArchivo);
}

void ejecutar_finalizarEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, void* datos){
    log_debug(debug_logger,"ds:%d ", direccionFisica);
    log_debug(debug_logger,"punteroArchivo: %d ",puntero);
    log_debug(debug_logger,"tamanio a escribir: %d", tamanio);

    escrituraArchivo(nombreArchivo, puntero, direccionFisica,tamanio);

    realizarEscrituraArchivo(nombreArchivo,  puntero, datos, tamanio);
    uint32_t size =list_size(peticiones_pendientes);

    //enviarString(nombreArchivo,fd_kernel,ESCRITURA_ARCHIVO_EXITOSA, info_logger); //cantidad de peticiones, kernel las recibe, lo va ir controlando
    enviarEnteroYString(size,nombreArchivo, strlen(nombreArchivo),fd_kernel,info_logger,ESCRITURA_ARCHIVO_EXITOSA);
    free(datos);

//free(nombreArchivo);
}

void ejecutar_finalizarLecturaArchivo(char* nombreArchivo){

    uint32_t size =list_size(peticiones_pendientes);
    //enviarString(nombreArchivo, fd_kernel, LECTURA_ARCHIVO_EXITOSA, info_logger); //cantidad de peticiones, kernel las recibe, lo va ir controlando
    enviarEnteroYString(size,nombreArchivo, strlen(nombreArchivo),fd_kernel,info_logger,LECTURA_ARCHIVO_EXITOSA);
}

//MANEJO PETICIONES
