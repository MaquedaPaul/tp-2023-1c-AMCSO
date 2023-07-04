//
// Created by utnso on 4/29/23.
//

#include <gestion_filesystem.h>
t_bitmapBloques* bitmapDeBloques;
t_bloques* archivoBloques;
t_bitarray* bitarrayBitmapDeBloques;
t_config_superbloque* cfg_superbloque;

bool existeFcbConNombre(char* nombreArchivo){
    //TODO
    return true;
}

t_config_fcb* buscarFCBporNombre(char* nombre){
    t_config_fcb* fcb;
    //TODO
    return fcb;
}


void realizarCreacionArchivo(char* nombreArchivo){

}
void realizarTruncacionArchivo(char* nombreArchivo){

}

void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(fcb->TAMANIO_ARCHIVO, punteroArchivo);
    uint32_t posicionBloque = buscar_posicion_dentro_del_bloque(fcb->TAMANIO_ARCHIVO, numeroBloque);
    int posicionInicial = buscarPunteroInicioDelBloque(numeroBloque, fcb);

    escribirBloque(numeroBloque, posicionBloque, punteroArchivo, datos,tamanioDatos, fcb);

}

void escribirBloque(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* fcb){

    void* datoEscrito = NULL;
    uint32_t cantidad = cantidadDisponibleDelBloque(punteroBloque);
    uint32_t nuevoTamanioAEscribir = 0;

    if (cantidad >= nuevoTamanioAEscribir) { //1 >= 6
        //LO QUE SE TIENE Q ESCRIBIR ESTA EN UN BLOQUE
        //escribir archivo de bloques a partir de la posicionInicialBloque con el tamanioALeer de una
    }else{

        int cantidadOcupada = cantidadOcupadaDentroDelBloque(cantidad);
        uint32_t tamanio = cfg_superbloque->BLOCK_SIZE - cantidadOcupada;

        //leer archivo de bloques con el tamaño que se pueda desde la "punteroBloque", y buscar el SIGUIENTE bloque para terminar de leer los bytes
        //lo que escribo en el archivo, lo copio en datoEscrito

        nuevoTamanioAEscribir = nuevoTamanioAEscribir - tamanio;
    }

    while(sizeof(datoEscrito) != tamanioAEscrbir) {

        uint32_t cantidadFaltante = cantidadFaltanteDelDato(nuevoTamanioAEscribir);
        uint32_t nuevoTamanio = nuevoTamanioAEscribir - cantidadFaltante;
        numeroBloque++;
        uint32_t posicionNuevoBloque = buscarPunteroInicioDelBloque(numeroBloque, fcb);

        //escrbir archivo de bloques con el nuevoTamaño a partir del posicionNuevoBloque
        //lo que escribo en el archivo, lo copio en datoEscrito

        nuevoTamanioAEscribir = cantidadFaltante;
    }

}

void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t  tamanio){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(fcb->TAMANIO_ARCHIVO, punteroArchivo); //punteroArchivo lo uso para buscar el numero de bloque
    uint32_t posicionBloque = buscar_posicion_dentro_del_bloque(fcb->TAMANIO_ARCHIVO, numeroBloque);
    int posicionInicial = buscarPunteroInicioDelBloque(numeroBloque, fcb);  //buscar puntero de inicio del bloque correspondiende al numero_bloque

    void* datoLeido = leer_archivo(numeroBloque, posicionBloque, punteroArchivo, tamanio, fcb);

    return datoLeido;
}


int numeroDeBloque(uint32_t tamanioArchivo, uint32_t puntero) {
    int i;
    int numero_bloque;
    int cantidad_bloques = tamanioArchivo / cfg_superbloque->BLOCK_SIZE + 1;

    for (i = 0; i < cantidad_bloques * cfg_superbloque->BLOCK_SIZE; i++) {

        int tamanio_bloques = cfg_superbloque->BLOCK_SIZE; //16

        if (puntero < tamanio_bloques) {
            numero_bloque = i;
            return numero_bloque;
        }

        tamanio_bloques += cfg_superbloque->BLOCK_SIZE;
    }
}

int buscar_posicion_dentro_del_bloque(uint32_t tamanioArchivo, uint32_t puntero){ //ej: size bloque: 16, 0 a 15 devuelve

    int cantidad_bloques = tamanioArchivo / cfg_superbloque->BLOCK_SIZE + 1;
    int tamanio_logico = cantidad_bloques*cfg_superbloque->BLOCK_SIZE;
    int posicion = tamanio_logico - puntero;
    int posicion_bloque_real = (cfg_superbloque->BLOCK_SIZE - posicion) + 1;
    return posicion_bloque_real;

}

uint32_t buscarPunteroInicioDelBloque(int numero_bloque, t_config_fcb* fcb){
    if(numero_bloque ==1){
        return fcb -> PUNTERO_DIRECTO;
    }else{
        //buscar en el archivo de bloque desde la posicion del punteroIndirecto, el puntero correspondiendo numeroBloque
        fcb -> PUNTERO_INDIRECTO;
        return 1;
    }
}

void* leer_archivo(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb){

    void* datoLeido = NULL;
    uint32_t cantidad = cantidadDisponibleDelBloque(punteroBloque);
    uint32_t nuevoTamanioALeer;

    if (cantidad >= tamanioALeer) { //1 >=  6
        //LO QUE SE TIENE Q LEER ESTA EN UN BLOQUE
        //leer archivo de bloques a partir de la posicionInicialBloque con el tamanioALeer de una
    }else{
        //se va a leer el bloque pero lo que se pueda

        int cantidadOcupada = cantidadOcupadaDentroDelBloque(cantidad); //2
        uint32_t tamanio = cfg_superbloque->BLOCK_SIZE - cantidadOcupada;// 3-2 =1

        //leer archivo de bloques con el tamaño que se pueda desde la "punteroBloque", y buscar el SIGUIENTE bloque para terminar de leer los bytes

        nuevoTamanioALeer = tamanioALeer - tamanio; // 6-1 = 5
    }


    while(sizeof(datoLeido) != tamanioALeer) {

        uint32_t cantidadFaltante = cantidadFaltanteDelDato(nuevoTamanioALeer);  // 5- 3 = 2
        uint32_t nuevoTamanio = nuevoTamanioALeer - cantidadFaltante; //5 - 2 = 3
        numeroBloque++;
        uint32_t posicionNuevoBloque = buscarPunteroInicioDelBloque(numeroBloque, fcb);

        //leer archivo de bloques con el nuevoTamaño a partir del posicionNuevoBloque
        nuevoTamanioALeer = cantidadFaltante;
    }

    return datoLeido;
}

uint32_t cantidadFaltanteDelDato(uint32_t nuevoTamanioALeer){

    if (nuevoTamanioALeer > cfg_superbloque->BLOCK_SIZE){
        return  nuevoTamanioALeer - cfg_superbloque->BLOCK_SIZE;
    }else{
        return 0; //lo q hay q leer alcanza en un bloque
    }
}

uint32_t cantidadDisponibleDelBloque(uint32_t puntero){
    return cfg_superbloque->BLOCK_SIZE - puntero;
}

uint32_t cantidadOcupadaDentroDelBloque(uint32_t cantidadDisponibles){
    return cfg_superbloque->BLOCK_SIZE - cantidadDisponibles;
}