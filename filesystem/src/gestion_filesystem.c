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
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(fcb->TAMANIO_ARCHIVO, numeroBloque);
    int posicionInicial = buscarPunteroInicioDelBloque(numeroBloque, fcb);

    escribirBloque(numeroBloque, posicionBloque, punteroArchivo, datos,tamanioDatos, fcb);

}

void escribirBloque(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* fcb){

    void* datoEscrito = NULL;
    uint32_t bytesQueSePuedenEscrbirEnUnBloque = cantidadDisponibleDelBloque(punteroBloque);
    uint32_t cantidadBytesNoEscrita = 0;

    if (bytesQueSePuedenEscrbirEnUnBloque >= tamanioAEscrbir) { //1 >= 6
        //LO QUE SE TIENE Q ESCRIBIR ESTA EN UN BLOQUE
        //escribir archivo de bloques a partir de la posicionInicialBloque con el tamanioALeer de una
    }else{

        //leer archivo de bloques con el tamaño = bytesQueSePuedenEscrbirEnUnBloque, desde la posicion "punteroBloque", y buscar el SIGUIENTE bloque para terminar de leer los bytes
        //lo que escribo en el archivo, lo copio en datoEscrito

        cantidadBytesNoEscrita = tamanioAEscrbir - bytesQueSePuedenEscrbirEnUnBloque;
    }

    while(sizeof(datoEscrito) != tamanioAEscrbir) {

        uint32_t cantidadBytesQueFaltanEscrbir = cantidadBytesQueFaltaOperar(cantidadBytesNoEscrita);
        uint32_t nuevoTamanio = cantidadBytesNoEscrita - cantidadBytesQueFaltanEscrbir;
        numeroBloque++;
        uint32_t posicionNuevoBloque = buscarPunteroInicioDelBloque(numeroBloque, fcb);

        //escrbir archivo de bloques con el nuevoTamaño a partir del posicionNuevoBloque
        //lo que escribo en el archivo, lo copio en datoEscrito

        cantidadBytesNoEscrita = cantidadBytesQueFaltanEscrbir;

    }

}

void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t  tamanio){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(punteroArchivo,numeroBloque);
    int punteroAlBloqueDeDatos = buscarPunteroInicioDelBloque(numeroBloque, fcb);  //buscar puntero de inicio del bloque correspondiende al numero_bloque

    void* datoLeido = leer_archivo(numeroBloque, posicionBloque, punteroArchivo, tamanio, fcb);

    return datoLeido;

}


void* leer_archivo(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb){

    void* datoLeido = NULL;
    uint32_t loQueSePuedeLeerEnUnBloque = cantidadDisponibleDelBloque(punteroBloque); //2
    uint32_t cantidadBytesNoLeida;

    if (loQueSePuedeLeerEnUnBloque >= tamanioALeer) { //8>=10 ,,

        //LO QUE SE TIENE Q LEER ESTA EN UN BLOQUE
        //leer archivo de bloques a partir del numeroBloque, y una vez encontrado el bloque usar punteroBloque con el tamanioALeer de una

    }else{

        //se va a leer el bloque pero lo que se pueda
        //leer archivo de bloques a partir del numeroBloque, y una vez encontrado el bloque usar punteroBloque con el tamanioALeer= loQueSePuedeLeerEnUnBloque.

        //y buscar el SIGUIENTE bloque para terminar de leer los bytes

        cantidadBytesNoLeida = tamanioALeer - loQueSePuedeLeerEnUnBloque; // 10-8 = 5,,, 12-2=10
    }


    while(sizeof(datoLeido) != tamanioALeer) { //8 != 10,, 2!=10

        uint32_t cantidadBytesQueFaltanLeer = cantidadBytesQueFaltaOperar(cantidadBytesNoLeida);  // 5- 3 = 2
        uint32_t nuevoTamanio = cantidadBytesNoLeida - cantidadBytesQueFaltanLeer; //10-2= 8
        numeroBloque++;
        uint32_t posicionNuevoBloque = buscarPunteroInicioDelBloque(numeroBloque, fcb);

        //leer archivo de bloques con el nuevoTamaño a partir del posicionNuevoBloque
        cantidadBytesNoLeida = cantidadBytesQueFaltanLeer; //2
    }

    return datoLeido;
}

int numeroDeBloque(uint32_t punteroArchivo) {

    return punteroArchivo/cfg_superbloque->BLOCK_SIZE;
}

int buscarPosicionDentroDelBloque(uint32_t puntero, uint32_t numeroBloque){ //ej: size bloque: 16, 0 a 15 devuelve

    uint32_t offset = puntero % cfg_superbloque->BLOCK_SIZE; //me devuelve un puntero desde donde hay que escrbir o leer dentro del bloque

}

uint32_t buscarPunteroInicioDelBloque(int numero_bloque, t_config_fcb* fcb) {
    if (numero_bloque == 1) {
        return fcb->PUNTERO_DIRECTO;
    } else {
        //buscar en el archivo de bloque desde la posicion del punteroIndirecto, el puntero correspondiendo numeroBloque
        fcb->PUNTERO_INDIRECTO;
        return 1;

    }
}

uint32_t cantidadBytesQueFaltaOperar(uint32_t nuevoTamanioALeer){

    if (nuevoTamanioALeer > cfg_superbloque->BLOCK_SIZE){ //2   .. 16| 10 .. 8
        return  nuevoTamanioALeer - cfg_superbloque->BLOCK_SIZE; //10-8 =2
    }else{
        return 0; //lo q hay q leer alcanza en un bloque
    }
}

uint32_t cantidadDisponibleDelBloque(uint32_t puntero){
    return cfg_superbloque->BLOCK_SIZE - puntero;
}

