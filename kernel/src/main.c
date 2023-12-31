#include <main.h>

void handle_sigint(int sig){
    cerrar_programa();
    exit(0);
}

//char* path_config;

int main(int argc, char *argv[]) {
    path_config = argv[1];
    ip_kernel = argv[2];

    signal(SIGINT, handle_sigint); // Esto lo uso para salir de la consola con CTRL + C

    if (!argumentosInvalidos(argc,argv, 3) || !init_logs_configs(path_config) || !cargar_configuracion(path_config)) {
        cerrar_programa();
        printf("No se pudo inicializar kernel\n");
        return EXIT_FAILURE;
    }

    inicializar_kernel();
    cerrar_programa();

    return EXIT_SUCCESS;
}