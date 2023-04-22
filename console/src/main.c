#include <main.h>
bool controlarPuntoEntrada = true;


int main(int argc, char* argv[])
{

    path_config = argv[1];
    path_pseudo = argv[2];

    if(!argumentosInvalidos(argc,argv, 3) || !init_logs_configs(path_config) || !cargar_configuracion(path_config))
    {
        printf("Error al iniciar, cerrando consola....");
        return 0;
    }
    if (!generar_conexiones()){
        //cerrar_programa(logger_console,config);
        return EXIT_FAILURE;
    }
    if(controlarPuntoEntrada){
        int a;
        printf("Inserte cualquier numero para continuar: \n");
        scanf("%d", &a);
    }
    t_list* lista = crear_lista_de_instrucciones(path_pseudo);
    list_iterate(lista, (void*)closure_mostrarListaInstrucciones);
    enviarListaInstrucciones(lista, fd_kernel, info_logger);

    return 0;
}
