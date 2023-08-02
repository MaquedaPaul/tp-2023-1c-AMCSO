#include <main.h>
bool controlarPuntoEntrada = true;

void handle_sigint(int sig){
    cerrar_programa();
    exit(0);
}


int main(int argc, char* argv[])
{

    path_config = argv[1];
    path_pseudo = argv[2];
    signal(SIGINT, handle_sigint);

    if(!argumentosInvalidos(argc,argv, 3) || !init_logs_configs(path_config) || !cargar_configuracion(path_config))
    {
        printf("Error al iniciar, cerrando consola....");
        return 0;
    }
    if (!generar_conexiones()){
        cerrar_programa();
        return EXIT_FAILURE;
    }
    if(controlarPuntoEntrada){
        int a;
        printf("Inserte cualquier numero para continuar: \n");
        scanf("%d", &a);
    }
    t_list* lista = crear_lista_de_instrucciones(path_pseudo);
    if(lista == NULL){
        cerrar_programa();
        return EXIT_FAILURE;
    }
    list_iterate(lista, (void*)closure_mostrarListaInstrucciones);
    enviarListaInstrucciones(lista, fd_kernel, info_logger);
    list_clean_and_destroy_elements(lista, (void*)liberarInstruccion);
    list_destroy(lista);
    atenderKernel();
    cerrar_programa();
    return 0;
}
