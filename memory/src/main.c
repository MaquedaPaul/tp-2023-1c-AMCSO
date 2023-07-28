#include <main.h>

void handle_sigint(int sig){
    //log_debug(debug_logger,"Mostrando estado actual antes de cerrar");
    //mostrarTablasDeSegmentos();
    //mostrarMemoria();
    //mostrarListaLibres();
    //mostrarListaUsados();
    cerrar_programa();
    exit(0);
}

int main(int argc, char* argv[]) {

    path_config = argv[1];
    ip_memory = argv[2];

    signal(SIGINT, handle_sigint);
    if (!argumentosInvalidos(argc,argv, 3) || !init_logs_configs(path_config) || !cargar_configuracion(path_config)) {
        cerrar_programa();
        printf("No se pudo inicializar memoria\n");
        return EXIT_FAILURE;

    }

    if (!iniciarMemoria()){
        cerrar_programa();
        return false;
    }


    return 0;
}
