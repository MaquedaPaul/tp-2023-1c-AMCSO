//
// Created by utnso on 4/17/23.
//

#ifndef MEMORY_GESTION_MEMORIA_H
#define MEMORY_GESTION_MEMORIA_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <estructuras.h>
#include <loggers_configs.h>
#include <pthread.h>

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_memory;
extern t_config_memory *cfg_memory;





void escribirEnPosicion(uint32_t direccion, void* datos, uint32_t tamanio, uint32_t pid, bool esCpu);
void* buscarDatosEnPosicion(uint32_t pid, uint32_t posicion, uint32_t tamanio, bool esCpu);
bool hayDisponibilidadDeEspacio(uint32_t tamanioSegmento);
bool elEspacioSeEncuentraEnDiferentesHuecos();
uint32_t realizarCreacionSegmento(uint32_t pid, t_segmento* huecoLibre, uint32_t tamanio);





bool agregarAHuecosLibres(t_segmento* huecoLibre);
bool removerDeHuecosLibres(t_segmento* huecoLibre);
bool agregarAHuecosUsados(t_segmento* huecoLibre);
bool removerDeHuecosUsados(t_segmento* huecoUsado);


t_segmento* dividirEnDosYObtenerUtilizado(t_segmento* huecoLibre,uint32_t tamanio);
t_tablaSegmentos* crearTablaSegmentos(uint32_t pid);
t_tablaSegmentos* buscarTablaConPid(uint32_t pid);
t_segmento* buscarSegmentoEnBaseADireccion(uint32_t direccion);
void realizarEliminacionSegmento(t_segmento* segmento, uint32_t pid);
void realizarEliminacionSegmentoSinPid(t_segmento* segmento);
uint32_t realizarCompactacion();
void informarTablasActualizadas(int cliente_socket);

#endif //MEMORY_GESTION_MEMORIA_H
