#include <main.h>
bool controlarPuntoEntrada = true;


int main(int argc, char* argv[])
{

    path_config = argv[1];
    init_logs_configs(path_config);
    if(argumentosInvalidos(argc,argv)){
        //ERROR
        printf("error");
        return 0;
    }
    cargar_configuracion(path_config);
    if (!generar_conexiones()){
        //cerrar_programa(logger_console,config);
        return EXIT_FAILURE;
    }
    if(controlarPuntoEntrada){
        int a;
        printf("Inserte cualquier numero para continuar: \n");
        scanf("%d", &a);
    }

    return 0;
}
