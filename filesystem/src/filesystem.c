//
// Created by utnso on 4/29/23.
//

#include <filesystem.h>
int fd_memoria;
char* archivoPendienteDeEscribir;
uint32_t punteroPendiente;



char* recibirString();
bool existeFcbConNombre(char* nombreArchivo);

void abrirArchivo(int cliente_socket){
    char* nombreArchivo = recibirString(cliente_socket);
    bool existeFcb = existeFcbConNombre(nombreArchivo);
    if(existeFcb){
        enviarOrden(APERTURA_ARCHIVO_EXITOSA, cliente_socket, info_logger);
    }else{
        enviarOrden(APERTURA_ARCHIVO_FALLIDA, cliente_socket, info_logger);
    }

}


void realizarCreacionArchivo(char* nombreArchivo);

void crearArchivo(int cliente_socket){
    char* nombreArchivo = recibirString(cliente_socket);
    realizarCreacionArchivo(nombreArchivo);
    enviarOrden(CREACION_ARCHIVO_EXITOSA,cliente_socket, info_logger);
}
void realizarTruncacionArchivo(char* nombreArchivo);

void truncarArchivo(int cliente_socket){
    char* nombreArchivo = recibirString(cliente_socket);
    realizarTruncacionArchivo(nombreArchivo);
    enviarOrden(TRUNCACION_ARCHIVO_EXITOSA,cliente_socket, info_logger);
}
void recibirParamsParaLecturaArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid);
void* realizarLecturaArchivo(char* nombreArchivo, uint32_t puntero, uint32_t  tamanio);
void leerArchivo(int cliente_socket){
    char* nombreArchivo;
    uint32_t puntero;
    uint32_t tamanio;
    uint32_t direccionFisica;
    uint32_t pid;
    recibirParamsParaLecturaArchivo(nombreArchivo, puntero, tamanio, direccionFisica, pid);
    t_datos* datosAEnviar = malloc(sizeof(t_datos));
    datosAEnviar->tamanio = tamanio;
    datosAEnviar->datos = realizarLecturaArchivo(nombreArchivo, puntero, tamanio);
    t_list* listaInts = list_create();
    list_add(listaInts, direccionFisica);
    list_add(listaInts, pid);
    enviarListaIntsYDatos(listaInts,datosAEnviar,cliente_socket,info_logger,ACCESO_PEDIDO_ESCRITURA);
}
void recibirParamsParaEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t pid,  uint32_t direccionFisica);

void escribirArchivo(int cliente_socket){
    char* nombreArchivo;
    uint32_t puntero;
    uint32_t tamanio;
    uint32_t pid;
    uint32_t direccion;
    recibirParamsParaEscrituraArchivo(archivoPendienteDeEscribir, punteroPendiente, tamanio, pid, direccion);
    t_list* listaInts = list_create();
    list_add(listaInts, direccion);
    list_add(listaInts, tamanio);
    list_add(listaInts, pid);
    enviarListaUint32_t(listaInts,cliente_socket,info_logger, ACCESO_PEDIDO_LECTURA);

}
void realizarEscrituraArchivo(void* datos, uint32_t tamanioDatos);
void finalizarEscrituraArchivo(int cliente_socket){
    uint32_t tamanioDatos;
    void* datos = recibirDatos(cliente_socket, tamanioDatos);
    realizarEscrituraArchivo(datos, tamanioDatos);

}