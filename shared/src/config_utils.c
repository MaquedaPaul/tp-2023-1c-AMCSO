#include <config_utils.h>



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

bool argumentosInvalidos(int argc, char *argv[], int limitante)
{
    // hay 2 argumentos, el path de el psudocodigo y el path de la config
    if (argc < limitante)
    {
        printf("Argumentos insuficientes\n");
        return false;
    }
    if(argc >= limitante+1){
        printf("Se esperaba: [2] argumentos, se obtuvo: [%d], %s;%s;%s....\n", argc,argv[1],argv[2],argv[3]);
        return false;
    }
    return true;
}



