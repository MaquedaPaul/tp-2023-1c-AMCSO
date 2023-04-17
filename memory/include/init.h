//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_INIT_H
#define TPSO_INIT_H
#include <loggers_configs.h>
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_memory;
extern t_config_memory *cfg_memory;
extern char* path_config;

int cargar_configuracion(char *path);
void crearEstructurasAdministrativas();

bool crearEspacioContiguoDeMemoria();
bool crearTablasSegmentos();
bool crearListaHuecosLibres();
bool crearSegmento0();

#endif //TEMPLATE_INIT_H
