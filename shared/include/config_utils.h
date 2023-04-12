#ifndef LOGGERS_CONFIGS_H_
#define LOGGERS_CONFIGS_H_
#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <stdint.h> //Distintos tipos de INT
#include <stdio.h>  //Para incluir NULL
#include <errno.h>





bool config_has_all_properties(t_config *cfg, char **properties);
t_config* iniciar_config(char* path_config);
bool argumentosInvalidos(int argc, char *argv[], int limitante);
#endif