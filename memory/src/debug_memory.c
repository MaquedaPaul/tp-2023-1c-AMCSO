//
// Created by utnso on 7/27/23.
//

#include <debug_memory.h>
#include <commons/string.h>


void mostrarSegmentoLibre(t_segmento* unSegmento){
    log_debug(debug_logger,"SEG:L, ID:%d, BASE:%d, LIMITE:%d", unSegmento->id,unSegmento->base,unSegmento->limite);
}
void mostrarSegmentoUsado(t_segmento* unSegmento){
    log_debug(debug_logger,"SEG:U, ID:%d, BASE:%d, LIMITE:%d", unSegmento->id,unSegmento->base,unSegmento->limite);
}
void mostrarSegmento(t_segmento* unSegmento){
    log_debug(debug_logger,"SEG, ID:%d, BASE:%d, LIMITE:%d", unSegmento->id,unSegmento->base,unSegmento->limite);
}
char* obtenerStringSegmento(t_segmento* unSegmento){
    char* nuevoString = string_new();
    string_append(&nuevoString, "ID:");
    string_append(&nuevoString, string_itoa(unSegmento->id));
    string_append(&nuevoString, ":");
    string_append(&nuevoString, string_itoa(unSegmento->base));
    string_append(&nuevoString, ":");
    string_append(&nuevoString, string_itoa(unSegmento->limite));
    return nuevoString;
}
void mostrarListaSegmentos(t_list* segmentos){
    char* nuevoString = string_new();
    int conteo =0;
    void concatenarStrings(t_segmento* unSegmento){
        char* unString = obtenerStringSegmento(unSegmento);
        if(conteo == 5){
            conteo = 0;
            string_append(&nuevoString, "\n ");
        }
        string_append(&nuevoString, unString);
        string_append(&nuevoString, " | ");
        conteo++;
    }
    string_append(&nuevoString,"[");
    list_iterate(segmentos,concatenarStrings);
    string_append(&nuevoString,"]");
    log_debug(debug_logger,"Lista de Segmentos:\n %s",nuevoString);
    free(nuevoString);
}

char* mem_decimalstring(const void* data, size_t size) {
    const unsigned char* bytes = (const unsigned char*)data;
    char* decstr = (char*)malloc(size * 3 + 1); // Tres dígitos decimales por byte + 1 para el terminador nulo
    if (decstr == NULL) {
        perror("Error en malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++) {
        sprintf(&decstr[i * 3], "%03d", bytes[i]);
    }
    decstr[size * 3] = '\0'; // Agregamos el terminador nulo al final de la cadena

    return decstr;
}

void imprimir_void(void* data, size_t size) {
    log_debug(debug_logger,"%.*s", (int)size, (char*)data);
}

void print_void_data(const void* data, size_t size) {
    const char* bytes = (const char*)data;
    size_t i = 0;
    while (i < size) {
        if (bytes[i] != 0) {
            log_debug(debug_logger,"C:%zu %.*s ", size - i, (int)(size - i), &bytes[i]);
            i += strlen(&bytes[i]) + 1; // Avanza el índice hasta el siguiente dato
        } else {
            size_t count_zeros = 0;
            while (i < size && bytes[i] == 0) {
                count_zeros++;
                i++;
            }
            log_debug(debug_logger,"C:%zu ", count_zeros);
        }
    }
}

void imprimir_espacio_contiguo(void* espacio_contiguo, size_t tamanio) {
    char* datos = (char*)espacio_contiguo;
    size_t i = 0;

    while (i < tamanio) {
        if (datos[i] == 0) {
            size_t bytes_contiguos = 0;
            while (i + bytes_contiguos < tamanio && datos[i + bytes_contiguos] == 0) {
                bytes_contiguos++;
            }
            log_debug(debug_logger, "CB:%zu ", bytes_contiguos);
            i += bytes_contiguos;
        } else {
            char dato_actual[5] = {0}; // Máximo 4 caracteres + terminador nulo
            size_t j = 0;
            while (i < tamanio && datos[i] != 0) {
                dato_actual[j] = datos[i];
                i++;
                j++;
            }
            log_debug(debug_logger, "C:%s ", dato_actual);
        }
    }
}

void mostrarMemoria(){
    log_debug(debug_logger,"Mostrando MEMORIA FISICA");
    imprimir_espacio_contiguo(espacio_contiguo, cfg_memory->TAM_MEMORIA);

}
void mostrarListaLibres(){
    log_debug(debug_logger,"Mostrando lista de segmentos libres");
    mostrarListaSegmentos(huecosLibres);
}
void mostrarListaUsados(){
    log_debug(debug_logger,"Mostrando lista de segmentos usados");
    mostrarListaSegmentos(huecosUsados);
}
void mostrarPosicionMemoria(uint32_t posicion, int cantidad){
    char* dato = espacio_contiguo + posicion;
    memcpy(dato, espacio_contiguo+posicion, cantidad);
    log_debug(debug_logger, "Dato concreto: %s",dato);

}




