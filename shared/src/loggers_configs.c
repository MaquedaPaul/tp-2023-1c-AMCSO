#include "utils/loggers_configs.h"


t_log *logger_kernel;
t_config *file_cfg_kernel;
int enteroCualquiera;

t_log *logger_cpu;
t_config *file_cfg_cpu;

t_log *logger_memory;
t_config *file_cfg_memory;


t_config_kernel *cfg_kernel;
t_config_cpu *cfg_cpu;
t_config_memory *cfg_memory;
bool mostrarConfig = true;

bool config_has_all_properties(t_config *cfg, char **properties)
{
    for (uint8_t i = 0; properties[i] != NULL; i++)
    {
        if (!config_has_property(cfg, properties[i]))
            return false;
    }
    return true;
}
t_config* iniciar_config(char* path_config)
{
    t_config* nuevo_config;
    if((nuevo_config = config_create(path_config)) == NULL){
        printf("No pude leer la config");
        exit(2);
    }
    return nuevo_config;
}



