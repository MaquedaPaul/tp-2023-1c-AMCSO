//
// Created by utnso on 5/4/23.
//

#include <estructuras_utils.h>


uint32_t obtenerBlockSize(char* superbloque){
    char* key = obtenerKey(superbloque, "BLOCK_SIZE");
    return atoi(key);
}
uint32_t obtenerBlockCount(char* superbloque){
    char* key = obtenerKey(superbloque, "BLOCK_COUNT");
    return atoi(key);
}


char* obtenerNombreFcb(char* unFcb){
    return obtenerKey(unFcb, "NOMBRE_ARCHIVO");;
}
uint32_t obtenerTamanioArchvio(char* unFcb){
    char* key = obtenerKey(unFcb, "TAMANIO_ARCHIVO");
    return atoi(key);
}
uint32_t obtenerPunteroDirecto(char* unFcb){
    char* key = obtenerKey(unFcb, "PUNTERO_DIRECTO");
    return atoi(key);
}
uint32_t obtenerPunteroIndirecto(char* unFcb){
    char* key = obtenerKey(unFcb, "PUNTERO_INDIRECTO");
    return atoi(key);
}
char* obtenerKey(char* path, char* key){
    t_config* config = config_create(path);
    char* keyObtenida = config_get_string_value(config, key);
    config_destroy(config);
    return keyObtenida;
}


void cambiarBitPor0(uint32_t bloque){

}
void cambiarBitPor1(uint32_t bloque){

}



