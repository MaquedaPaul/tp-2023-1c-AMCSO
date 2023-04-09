//
// Created by utnso on 4/7/23.
//

#include <init.h>

char* path_pseudo;



int argumentosInvalidos(int argc, char *argv[])
{
    // hay 2 argumentos, el path de el psudocodigo y el path de la config
    if (argc < 2)
    {
        log_error(error_logger, "Se esperaba: [CONFIG_PATH]\n");
        return 1;
    }
    path_pseudo = argv[2];

    // config valida
    t_config *config = config_create(path_config);
    if (config == NULL)
    {
        log_error(error_logger, "Ocurrió un error al intentar abrir el archivo config");
        return 1;
    }

    char *properties[] = {
            "IP_KERNEL",
            "PUERTO_KERNEL",
            NULL};
    // Falta alguna propiedad
    if (!config_has_all_properties(config, properties))
    {
        log_error(error_logger, "Propiedades faltantes en el archivo de configuracion");
        return 1;
    }
    log_info(trace_logger,"Liberando config");
    config_destroy(config);

    return 0;
}

void cargar_configuracion(char *path)
{
    file_cfg_console = config_create(path);
    cfg_console->IP_KERNEL = strdup(config_get_string_value(file_cfg_console, "IP_KERNEL"));
    log_info(trace_logger, "IP Cargada Correctamente: %s", cfg_console->IP_KERNEL);
    cfg_console->PUERTO_KERNEL = strdup(config_get_string_value(file_cfg_console, "PUERTO_KERNEL"));
    log_info(trace_logger, "Puerto Cargado Correctamente: %s", cfg_console->PUERTO_KERNEL);
    log_info(trace_logger, "Archivo de configuracion cargado correctamente");
    config_destroy(file_cfg_console);
}
