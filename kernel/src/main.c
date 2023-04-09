#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    path_config = argv[1];
    ip_kernel = argv[2];

    if (!init(path_config) || !cargar_configuracion(path_config)) {
        cerrar_programa();
        printf("No se pudo inicializar kernel\n");
        return EXIT_FAILURE;

    }


    return 0;
}
