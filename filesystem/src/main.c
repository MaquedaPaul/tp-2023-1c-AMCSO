#include <main.h>

void handle_sigint(int sig){
    cerrar_programa();
    exit(0);
}

int main(int argc, char* argv[]) {
    path_config= argv[1];
    ip_filesystem = argv[2];
    signal(SIGINT, handle_sigint);
    if (!argumentosInvalidos(argc,argv,3) || !init_logs_configs(path_config) || !cargar_configuracion(path_config)) {
        //cerrar_programa();
        printf("No se pudo inicializar kernel\n");
        return EXIT_FAILURE;

    }
    if (!iniciarFilesystem()){
        //cerrar_programa(logger_console,config);
        return EXIT_FAILURE;
    }


    return 0;
}
