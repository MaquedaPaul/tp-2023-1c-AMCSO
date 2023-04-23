#include <kernel_config.h>

KernelConfig KERNEL_CONFIG;

void rellenar_configuracion_kernel(Config *config)
{
  KERNEL_CONFIG.IP = config_get_string_value(config, "IP");
  KERNEL_CONFIG.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
  KERNEL_CONFIG.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
  KERNEL_CONFIG.IP_CPU = config_get_string_value(config, "IP_CPU");
  KERNEL_CONFIG.PUERTO_CPU = config_get_string_value(config, "PUERTO_CPU");
  KERNEL_CONFIG.IP_FILESYSTEM = config_get_string_value(config, "IP_FILESYSTEM");
  KERNEL_CONFIG.PUERTO_FILESYSTEM = config_get_string_value(config, "PUERTO_FILESYSTEM");
  KERNEL_CONFIG.PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
  KERNEL_CONFIG.ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
  KERNEL_CONFIG.GRADO_MAX_MULTIPROGRAMACION = config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION");
  KERNEL_CONFIG.ESTIMACION_INICIAL = config_get_string_value(config, "ESTIMACION_INICIAL");
  KERNEL_CONFIG.HRRN_ALFA = config_get_string_value(config, "HRRN_ALFA");
  KERNEL_CONFIG.RECURSOS = config_get_array_value(config, "RECURSOS");
  KERNEL_CONFIG.INSTANCIAS_RECURSOS = config_get_array_value(config, "INSTANCIAS_RECURSOS");
}
