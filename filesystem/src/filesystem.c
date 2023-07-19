//
// Created by utnso on 4/29/23.
//

#include <filesystem.h>
int fd_memoria;
char* archivoPendienteDeEscribir;
uint32_t punteroPendiente;


void abrirArchivo(int cliente_socket){
    char* nombreArchivo = recibirString(cliente_socket);
   // bool existeFcb = existeFcbConNombre(nombreArchivo);
    if(existe_archivoFCB(nombreArchivo)){
        enviarOrden(APERTURA_ARCHIVO_EXITOSA, cliente_socket, info_logger);
    }else{
        enviarOrden(APERTURA_ARCHIVO_FALLIDA, cliente_socket, info_logger);
    }

}

void crearArchivo(int cliente_socket){
    char* nombreArchivo = recibirString(cliente_socket);
    realizarCreacionArchivo(nombreArchivo);
    enviarOrden(CREACION_ARCHIVO_EXITOSA,cliente_socket, info_logger);
}


void truncarArchivo(int cliente_socket){
    char* nombreArchivo = recibirString(cliente_socket);
    uint32_t nuevoTamanio; //TODO INCLUIR EN LA RECEPCIÓN
    realizarTruncacionArchivo(nombreArchivo, nuevoTamanio);
    enviarOrden(TRUNCACION_ARCHIVO_EXITOSA,cliente_socket, info_logger);
}

void leerArchivo(int cliente_socket){
    char* nombreArchivo;
    uint32_t puntero;
    uint32_t tamanio;
    uint32_t direccionFisica;
    uint32_t pid;
    recibirParamsParaLecturaEscrituraArchivo(nombreArchivo, puntero, tamanio, direccionFisica, pid, cliente_socket);
    t_datos* datosAEnviar = malloc(sizeof(t_datos));
    datosAEnviar->tamanio = tamanio;
    datosAEnviar->datos = realizarLecturaArchivo(nombreArchivo, puntero, tamanio);
    t_list* listaInts = list_create();
    list_add(listaInts, &direccionFisica);
    list_add(listaInts, &pid);

    lecturaArchivo(nombreArchivo,puntero,direccionFisica, tamanio);
    enviarListaIntsYDatos(listaInts,datosAEnviar,cliente_socket,info_logger,ACCESO_PEDIDO_ESCRITURA);
}


void escribirArchivo(int cliente_socket){
    char* nombreArchivo;
    uint32_t puntero;
    uint32_t tamanio;
    uint32_t pid;
    uint32_t direccion;
    recibirParamsParaLecturaEscrituraArchivo(archivoPendienteDeEscribir, punteroPendiente, tamanio, pid, direccion, cliente_socket);
    t_list* listaInts = list_create();
    list_add(listaInts,  &direccion);
    list_add(listaInts,  &tamanio);
    list_add(listaInts, &pid);

    escrituraArchivo(nombreArchivo, puntero, direccion, tamanio);
    enviarListaUint32_t(listaInts,cliente_socket,info_logger, ACCESO_PEDIDO_LECTURA);
}

void finalizarEscrituraArchivo(int cliente_socket){
     //mientras espero que memoria me de el dato en la direccion fisica, se deberia mantener el mismo archivo
    uint32_t tamanioDatos;
    uint32_t puntero;
    void* datos = recibirDatos(cliente_socket, tamanioDatos);
    realizarEscrituraArchivo(archivoPendienteDeEscribir,  puntero, datos,tamanioDatos);//escribir el dato en el archivo
    //mandar ok a kernel
}

void  finalizarLecturaArchivo(int cliente){
    //mando ok a kernel, se escrbio bien en memoria lo q se leyo del archivo
}