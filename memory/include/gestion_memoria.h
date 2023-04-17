//
// Created by utnso on 4/17/23.
//

#ifndef MEMORY_GESTION_MEMORIA_H
#define MEMORY_GESTION_MEMORIA_H
#include <stdint.h>
#include <stdbool.h>

void escribirEnPosicion(uint32_t direccion);
bool hayDisponibilidadDeEspacio();
bool elEspacioSeEncuentraEnDiferentesHuecos();
uint32_t realizarCreacionSegmento(uint32_t pid);
void realizarEliminacionSegmento(uint32_t direccion);
uint32_t realizarCompactacion();
void informarTablasActualizadas(uint32_t tablasActualizadas,int cliente_socket);

#endif //MEMORY_GESTION_MEMORIA_H
