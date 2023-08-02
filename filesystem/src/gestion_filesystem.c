//
// Created by utnso on 4/29/23.
//

#include <gestion_filesystem.h>
#include <sys/mman.h>

pthread_mutex_t mutex_cliente_socket;



int existe_archivoFCB(char *nombre_archivo) {

    int tamanio_lista_FCBs = list_size(lista_FCBs);
    for (int i = 0; i < tamanio_lista_FCBs; i++)  {

        t_config_fcb *aux_FCB = list_get(lista_FCBs, i);

        if (strcmp(aux_FCB->NOMBRE_ARCHIVO, nombre_archivo) == 0)  {


            aperturaArchivo(nombre_archivo);
            return 1;
        }
    }
    return 0;
}


t_config_fcb* buscarFCBporNombre(char* nombre){

    int tamanio_lista_FCBs = list_size(lista_FCBs);
    for (int i = 0; i < tamanio_lista_FCBs; i++)  {

        t_config_fcb *aux_FCB = list_get(lista_FCBs, i);

        if (strcmp(aux_FCB->NOMBRE_ARCHIVO, nombre) == 0)  {

            return aux_FCB;
        }
    }
}


void realizarCreacionArchivo(char* nombreArchivo){

    char* punto_PATH_FCB = cfg_filesystem->PATH_FCB; 
	char* path =string_new();
	string_append(&path, punto_PATH_FCB);
	string_append(&path, "/");
	string_append(&path, nombreArchivo);
	string_append(&path,".dat");

	FILE* f = fopen(path,"wb");
    fclose(f);

	t_config* aux_config = config_create(path);

	config_set_value(aux_config,"NOMBRE_ARCHIVO",nombreArchivo);
	config_set_value(aux_config,"TAMANIO_ARCHIVO","0");
	config_set_value(aux_config,"PUNTERO_DIRECTO", "0");
	config_set_value(aux_config,"PUNTERO_INDIRECTO", "0");

	config_save(aux_config);

    t_config_fcb* aux_FCB = malloc(sizeof(t_config_fcb));

    int tamanio_nombre_archivo = strlen(nombreArchivo);

    aux_FCB->NOMBRE_ARCHIVO = malloc( tamanio_nombre_archivo + 1);
    strcpy( aux_FCB->NOMBRE_ARCHIVO,nombreArchivo);

    //TODO asignar los primeros bloques libres
    aux_FCB->TAMANIO_ARCHIVO = 0;
    aux_FCB->PUNTERO_DIRECTO = 0;
    aux_FCB->PUNTERO_INDIRECTO = 0;
    aux_FCB->fcb_config = aux_config;

    list_add(lista_FCBs,aux_FCB);

	creacionArchivo(nombreArchivo);
	free(path);


}
void realizarTruncacionArchivo(char* nombreArchivo, uint32_t nuevo_tamanio_del_archivo){

    uint32_t tamanio_del_archivo_a_truncar;
    uint32_t tamanio_de_lista_FCBs = list_size(lista_FCBs);
    for (int i = 0; i < tamanio_de_lista_FCBs; i++) {

        t_config_fcb *aux_FCB = list_get(lista_FCBs, i);
        if (strcmp(aux_FCB->NOMBRE_ARCHIVO, nombreArchivo) == 0) {

            tamanio_del_archivo_a_truncar = aux_FCB->TAMANIO_ARCHIVO;

            if(tamanio_del_archivo_a_truncar != nuevo_tamanio_del_archivo ) {

                ampliar_o_reducir_tamanio(aux_FCB, nuevo_tamanio_del_archivo, tamanio_del_archivo_a_truncar);
                truncacionArchivo(nombreArchivo, nuevo_tamanio_del_archivo);
            }

        break;
        }
    }
}


void ampliar_o_reducir_tamanio(t_config_fcb *aux_FCB, uint32_t nuevo_tamanio, uint32_t tamanio_archivo) {

    if(tamanio_archivo == 0){ 

    if(nuevo_tamanio > cfg_superbloque->BLOCK_SIZE){

       uint32_t cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / cfg_superbloque->BLOCK_SIZE);

       for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libre(bitmap) ;
          uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
          *nuevo_bloque_libre = bloque_libre;
          list_add(lista_bloques, nuevo_bloque_libre);
       }

       aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;

       uint32_t* enteroAuxPuntero = list_remove(lista_bloques,0);
       aux_FCB->PUNTERO_DIRECTO = *enteroAuxPuntero;

       accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, 0, aux_FCB->PUNTERO_DIRECTO);

       uint32_t* enteroAuxIndPuntero = list_remove(lista_bloques,0);
       aux_FCB->PUNTERO_INDIRECTO = *enteroAuxIndPuntero;

       log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, aux_FCB->PUNTERO_INDIRECTO);

       uint32_t  puntero_directo = *enteroAuxPuntero;
       uint32_t  puntero_indirecto = *enteroAuxIndPuntero;
       uint32_t cantidad_de_punteros = list_size(lista_bloques);
       uint32_t  offset = 0;

       for (uint32_t i = 0; i < cantidad_de_punteros; i++) {

           uint32_t* puntero_a_escribir = list_get(lista_bloques,i);
           uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;

           memcpy(archivoBloques->archivo + posicion_del_puntero_indirecto, puntero_a_escribir, sizeof(uint32_t));
           offset += sizeof(uint32_t);

       } 

        t_config* archivo_config = aux_FCB->fcb_config;

        config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
        config_set_value(archivo_config, "PUNTERO_DIRECTO", string_itoa((int)puntero_directo));
        config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_itoa((int)puntero_indirecto));
        config_save(archivo_config);

    // cambiaren el bitmap el bloque con 1 todos los bloques
    // agregar en el archivo el numero de bloque directo y el bloque con punteros 
    // agrego en el archivo de bloques los punteros 

    }else {

        uint32_t puntero_directo = obtener_bloque_libre(bitmap) ;
        aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
        aux_FCB->PUNTERO_DIRECTO = puntero_directo;

         accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, 0, aux_FCB->PUNTERO_DIRECTO);

        t_config* archivo_config = aux_FCB->fcb_config;


        config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
        config_set_value(archivo_config, "PUNTERO_DIRECTO", string_itoa((int)puntero_directo));
        config_save(archivo_config);

    // cambiaren en el bitmap el bloque que voy a ocupar  con 1
    // agregar en el archivo el numero de bloque directo
            
    }

} else {   // tamanio_archivo > 0

    if(tamanio_archivo <= cfg_superbloque->BLOCK_SIZE){

        if(nuevo_tamanio <= cfg_superbloque->BLOCK_SIZE ){    //  cantidad_de_bloques = 1 ;

          if(nuevo_tamanio == 0 ){ 

            t_config* archivo_config = aux_FCB->fcb_config;

            uint32_t puntero_directo  = config_get_int_value(archivo_config, "PUNTERO_DIRECTO");

            accesoABitmap(puntero_directo, bitarray_test_bit(bitmap, puntero_directo));
            bitarray_clean_bit(bitmap,puntero_directo );
            accesoABitmap(puntero_directo, bitarray_test_bit(bitmap, puntero_directo));
            accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, 0, puntero_directo  );

            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            aux_FCB->PUNTERO_DIRECTO = 0;


            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
            config_set_value(archivo_config, "PUNTERO_DIRECTO", string_itoa(0));
            config_save(archivo_config);

         } else {

            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            t_config* archivo_config = aux_FCB->fcb_config;
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
            config_save(archivo_config);

            }
        } else {    // nuevo_tamanio  > 64

            uint32_t  cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / cfg_superbloque->BLOCK_SIZE);
            cantidad_de_bloques = cantidad_de_bloques - 1 ;

          for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

              uint32_t bloque_libre = obtener_bloque_libre(bitmap) ;
              uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
              *nuevo_bloque_libre = bloque_libre;
              list_add(lista_bloques, nuevo_bloque_libre);

             }

            uint32_t* enteroAuxInd = list_remove(lista_bloques,0);
            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            aux_FCB->PUNTERO_INDIRECTO = *enteroAuxInd;
            uint32_t puntero_indirecto = *enteroAuxInd;

            log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, aux_FCB->PUNTERO_INDIRECTO);

            uint32_t cantidad_de_punteros = list_size(lista_bloques);
            uint32_t  offset = 0;

            for (int i = 0; i < cantidad_de_punteros; i++) {

                uint32_t* puntero_a_escribir = list_get(lista_bloques,i);
                accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+1, *puntero_a_escribir);
                uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;

                memcpy(archivoBloques->archivo + posicion_del_puntero_indirecto, puntero_a_escribir, sizeof(uint32_t));
                offset += sizeof(uint32_t);
            }

            t_config* archivo_config = aux_FCB->fcb_config;

            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
            config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_itoa((int)puntero_indirecto));
            config_save(archivo_config);

    // cambiaren el bitmap el bloque con 1 todos los bloques menos el primero que ya esta ocupado
    // agregar en el archivo, el bloque con punteros , puede que el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 
        }

    } else { // si tamaño es mayor al bloque  tamanio_archivo > cfg_superbloque->BLOCK_SIZE

         if(nuevo_tamanio <=  cfg_superbloque->BLOCK_SIZE){  

             if(nuevo_tamanio == 0 ){

                uint32_t  cantidad_de_bloques_a_eliminar =   1 + ((tamanio_archivo - 1) / cfg_superbloque->BLOCK_SIZE);
                t_config* archivo_config = aux_FCB->fcb_config;

                uint32_t puntero_directo  = config_get_int_value(archivo_config, "PUNTERO_DIRECTO");
                uint32_t puntero_indirecto  = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");
                accesoABitmap(puntero_directo, bitarray_test_bit(bitmap, puntero_directo));
                bitarray_clean_bit(bitmap,puntero_directo);
                accesoABitmap(puntero_directo, bitarray_test_bit(bitmap, puntero_directo));
                accesoABitmap(puntero_indirecto, bitarray_test_bit(bitmap, puntero_indirecto));
                bitarray_clean_bit(bitmap,puntero_indirecto);
                accesoABitmap(puntero_indirecto, bitarray_test_bit(bitmap, puntero_indirecto));
                log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, puntero_indirecto);

                aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
                aux_FCB->PUNTERO_DIRECTO = 0;
                aux_FCB->PUNTERO_INDIRECTO = 0;

                uint32_t  offset = 0;

                for (uint32_t i = 0; i < (cantidad_de_bloques_a_eliminar-1); i++) {

                    uint32_t bloque_ocupado ;
                    uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;

                    memcpy(&bloque_ocupado, archivoBloques->archivo + posicion_del_puntero_indirecto, sizeof(uint32_t));
                    offset += sizeof(uint32_t);
                    accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                    bitarray_clean_bit(bitmap,bloque_ocupado );
                    accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                    accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+1, bloque_ocupado);
                }

                config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio) );
                config_set_value(archivo_config, "PUNTERO_DIRECTO", string_itoa(0));
                config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_itoa(0) );
                config_save(archivo_config);

              } else {

                uint32_t  cantidad_de_bloques_a_eliminar =   1 + ((tamanio_archivo - 1) / cfg_superbloque->BLOCK_SIZE);
                t_config* archivo_config = aux_FCB->fcb_config;

                uint32_t puntero_indirecto  = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");
                accesoABitmap(puntero_indirecto, bitarray_test_bit(bitmap, puntero_indirecto));
                bitarray_clean_bit(bitmap,puntero_indirecto );
                accesoABitmap(puntero_indirecto, bitarray_test_bit(bitmap, puntero_indirecto));
                log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, puntero_indirecto);

                aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
                aux_FCB->PUNTERO_INDIRECTO = 0;

                uint32_t  offset = 0;

                for (uint32_t i = 0; i < (cantidad_de_bloques_a_eliminar-1); i++) {

                    uint32_t bloque_ocupado ;
                    uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;

                    memcpy(&bloque_ocupado, archivoBloques->archivo + posicion_del_puntero_indirecto, sizeof(uint32_t));
                    offset += sizeof(uint32_t);
                    accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));

                    bitarray_clean_bit(bitmap,bloque_ocupado );
                    accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                    accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+1, bloque_ocupado);
                }

                config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio) );
                config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_itoa(0) );
                config_save(archivo_config);
            } 
         } 

    else {

        uint32_t  cantidad_de_bloques_viejos = 1 + ((tamanio_archivo - 1) / cfg_superbloque->BLOCK_SIZE);
        uint32_t  cantidad_de_bloques        = 1 + ((nuevo_tamanio - 1) / cfg_superbloque->BLOCK_SIZE);

        if(cantidad_de_bloques > cantidad_de_bloques_viejos ){      //  agrandando

          cantidad_de_bloques = cantidad_de_bloques - cantidad_de_bloques_viejos ;

          for (uint32_t i = 0; i < cantidad_de_bloques; i++) {

              uint32_t bloque_libre = obtener_bloque_libre(bitmap) ;
              uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
              *nuevo_bloque_libre = bloque_libre;
              list_add(lista_bloques, nuevo_bloque_libre);

          }

          t_config* archivo_config = aux_FCB->fcb_config;
          uint32_t puntero_indirecto = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");
          //log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, puntero_indirecto);

          uint32_t cantidad_de_punteros = list_size(lista_bloques);
          uint32_t  offset = sizeof(uint32_t) * (cantidad_de_bloques_viejos-1);

          for (int i = 0; i < cantidad_de_punteros; i++) {

              uint32_t* puntero_a_escribir = list_get(lista_bloques,i);
              uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;
              accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+cantidad_de_bloques_viejos, *puntero_a_escribir);

              memcpy(archivoBloques->archivo + posicion_del_puntero_indirecto, puntero_a_escribir, sizeof(uint32_t));
              offset += sizeof(uint32_t);
          }

         aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
         config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
         config_save(archivo_config);

   // cambiaren el bitmap el bloque con 1 todos los bloques menos los que ya estan ocupados
    // el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 

          }

        if(cantidad_de_bloques < cantidad_de_bloques_viejos ) {   // cantidad_de_bloques = cantidad_de_bloques_viejos  NO HACER NADA

             uint32_t  cantidad_de_bloques_a_eliminar = cantidad_de_bloques_viejos - cantidad_de_bloques ;

             t_config* archivo_config = aux_FCB->fcb_config;
             uint32_t puntero_indirecto = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");
             log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, puntero_indirecto);

             uint32_t  offset = sizeof(uint32_t) * (cantidad_de_bloques - 1);

            for (uint32_t i = 0; i < cantidad_de_bloques_a_eliminar; i++) {

                uint32_t bloque_ocupado ;
                uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;

                memcpy(&bloque_ocupado, archivoBloques->archivo + posicion_del_puntero_indirecto, sizeof(uint32_t));
                offset += sizeof(uint32_t);
                accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                bitarray_clean_bit(bitmap,bloque_ocupado );
                accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+cantidad_de_bloques, bloque_ocupado);
            } 

            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
            config_save(archivo_config);


    // agregar en el archivo, el bloque con punteros , puede que el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 
         } 

          if(cantidad_de_bloques == cantidad_de_bloques_viejos ) { 

            t_config* archivo_config = aux_FCB->fcb_config;
            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
            config_save(archivo_config);

        }
    }
}
}
}


uint32_t obtener_bloque_libre(t_bitarray* bitmap) {

    for (uint32_t i = 0; i < bitarray_get_max_bit(bitmap); i++) {
            accesoABitmap(i, bitarray_test_bit(bitmap, i));

        if (!bitarray_test_bit(bitmap, i)) {

            bitarray_set_bit(bitmap, i);
            accesoABitmap(i, bitarray_test_bit(bitmap, i));

            return i;
        }
    }

	log_info(info_logger, "No se obtuvo un bloque libre");
    return -1;
}


void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(punteroArchivo, numeroBloque);

    escribirBloque(numeroBloque, posicionBloque, punteroArchivo, datos,tamanioDatos, fcb);

}

void escribirBloque(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* fcb){

    void* datoAEscribirFaltante =NULL;

    uint32_t bytesQueSePuedenEscrbirEnUnBloque = cantidadDisponibleDelBloque(posicionBloque);
    uint32_t cantidadBytesNoEscrita = 0;
    uint32_t bytesEscritos = 0;
    uint32_t numeroBloqueDelFS = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

    uint32_t bytesYaEscritos = 0;
    log_debug(debug_logger,"puntero: %d ", punteroArchivo);
    log_debug(debug_logger,"bytesQueSePuedenEscrbirEnUnBloque: %d", bytesQueSePuedenEscrbirEnUnBloque);

    if (bytesQueSePuedenEscrbirEnUnBloque >= tamanioAEscrbir) {
        log_debug(debug_logger,"SE ESCRIBE EN UN BLOQUE NADA MAS");
        char* datoLeidoDeMemoria = malloc(tamanioAEscrbir +1);
        datoLeidoDeMemoria = (char*) datos;
        log_debug(debug_logger,"dato a escrbir en el archivo: %s", datos);
        memcpy(archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque, datos, tamanioAEscrbir);

        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

    }else{
        log_debug(debug_logger,"SE VA A ESCRIBIR EN MAS DE UN BLOQUE");
        memcpy(archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque, datos, bytesQueSePuedenEscrbirEnUnBloque);
        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

        //para testear: leo lo que escribi
        void* datoEscritoEnElArchivo = malloc(bytesQueSePuedenEscrbirEnUnBloque);
        memcpy(datoEscritoEnElArchivo, archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque, bytesQueSePuedenEscrbirEnUnBloque);

        bytesEscritos = bytesQueSePuedenEscrbirEnUnBloque;
        cantidadBytesNoEscrita = tamanioAEscrbir - bytesQueSePuedenEscrbirEnUnBloque;

        while(bytesEscritos != tamanioAEscrbir) {
            log_debug(debug_logger,"SE ESCRIBE EN EL SIGUIENTE BLOQUE");

            uint32_t cantidadBytesQueFaltanEscrbir = cantidadBytesQueNoSePuedeLeerEnUnBloque(cantidadBytesNoEscrita);
            uint32_t nuevoTamanio = cantidadBytesNoEscrita - cantidadBytesQueFaltanEscrbir;
            numeroBloque++;
            numeroBloqueDelFS = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

            datoAEscribirFaltante = malloc(nuevoTamanio);
            memcpy(datoAEscribirFaltante, datos + bytesEscritos, nuevoTamanio); //porque voy a escrbir en el archivo de a poco, separo "datos"

            memcpy(archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE), datoAEscribirFaltante, nuevoTamanio);
            accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

            cantidadBytesNoEscrita = cantidadBytesQueFaltanEscrbir;
            bytesEscritos += nuevoTamanio;
            free(datoAEscribirFaltante); //libero lo que escribi en este ciclo
            datoAEscribirFaltante = NULL; //solo va a contener lo que necesito escrbir en cada ciclo
        }
    }

    msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
}

void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t tamanioDatos){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(punteroArchivo, numeroBloque);

    void* datoLeido = leer_archivo(numeroBloque, posicionBloque, punteroArchivo, tamanioDatos, fcb);

    return datoLeido;

}


void* leer_archivo(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb){

    void* datoLeido = NULL;
    void* datoLeidoNuevo = NULL;
    uint32_t loQueSePuedeLeerEnUnBloque = cantidadDisponibleDelBloque(posicionBloque);
    uint32_t cantidadBytesNoLeida = 0;
    uint32_t bytesLeidos = 0;

    uint32_t numeroBloqueDelFS = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

    if (loQueSePuedeLeerEnUnBloque >= tamanioALeer) {
        datoLeido = malloc(tamanioALeer + 1);
        memcpy(datoLeido, archivoBloques->archivo + (posicionBloque * cfg_superbloque->BLOCK_SIZE) + posicionBloque,  tamanioALeer);

        char* dato = (char*) datoLeido;
        log_debug(debug_logger, "Dato leido: %s", dato);
        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

    }else{

        datoLeido = malloc(loQueSePuedeLeerEnUnBloque);
        memcpy(datoLeido, archivoBloques-> archivo + (posicionBloque * cfg_superbloque->BLOCK_SIZE) + posicionBloque,  loQueSePuedeLeerEnUnBloque);
        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

        cantidadBytesNoLeida = tamanioALeer - loQueSePuedeLeerEnUnBloque;
        bytesLeidos = loQueSePuedeLeerEnUnBloque;

        while(bytesLeidos != tamanioALeer) {

            uint32_t cantidadBytesQueFaltanLeer = cantidadBytesQueNoSePuedeLeerEnUnBloque(cantidadBytesNoLeida);
            uint32_t nuevoTamanioALeer = cantidadBytesNoLeida - cantidadBytesQueFaltanLeer;
            numeroBloque++;
            numeroBloqueDelFS = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

            datoLeidoNuevo = malloc(nuevoTamanioALeer);
            memcpy(datoLeidoNuevo, archivoBloques-> archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE), nuevoTamanioALeer);
            accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

            void* datoLeido_temporal = realloc(datoLeido, bytesLeidos + nuevoTamanioALeer); //Cuando se redimensiona la memoria con realloc, si el nuevo tamaño es mayor que el anterior, se conservan todos los valores originales
            if(datoLeido_temporal == NULL){
                log_error(error_logger,"No se pudo agrandar el tamanio de datoLeido");
            }
            datoLeido = datoLeido_temporal;

            memcpy(datoLeido + bytesLeidos, datoLeidoNuevo, nuevoTamanioALeer); //escribo en el datoLeido el nuevo contenido a partir de lo ultimo leido, voy concatenando lo leido
            accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

            bytesLeidos += nuevoTamanioALeer;
            cantidadBytesNoLeida = cantidadBytesQueFaltanLeer;
            free(datoLeidoNuevo);
            datoLeidoNuevo = NULL; //solo va a contener lo que necesito leer en cada ciclo
        }
    }

    log_debug(debug_logger, "el dato leido: %s", datoLeido);
    return datoLeido;
}

int numeroDeBloque(uint32_t punteroArchivo) {
    uint32_t numero = (punteroArchivo/cfg_superbloque->BLOCK_SIZE) + 1;
    log_debug(debug_logger,"numero bloques del archivo: %d", numero);
    return numero;
}

int buscarPosicionDentroDelBloque(uint32_t punteroArchivo, uint32_t numeroBloque){ //ej: size bloque: 16, 0 a 15 devuelve

    uint32_t offset = punteroArchivo % cfg_superbloque->BLOCK_SIZE; //me devuelve un puntero desde donde hay que escrbir o leer dentro del bloque

    log_debug(debug_logger,"posicion dentro del bloque: %d ",  offset);
    return offset;
}

uint32_t buscarNumeroDeBloqueDelArchivoDeBloque(int numero_bloque, t_config_fcb* fcb) {

    if (numero_bloque == 1) {
        log_debug(debug_logger,"puntero directo: %d ",  fcb->PUNTERO_DIRECTO);
        return fcb->PUNTERO_DIRECTO;

    }else{

        uint32_t offset = sizeof(uint32_t) * (numero_bloque-1);
        uint32_t numeroBloqueDelPunteroDeBloques;
        memcpy(&numeroBloqueDelPunteroDeBloques, archivoBloques-> archivo + (fcb -> PUNTERO_INDIRECTO * cfg_superbloque->BLOCK_SIZE) + offset, sizeof(uint32_t));

        log_debug(debug_logger,"puntero indirecto: %d ",  fcb->PUNTERO_INDIRECTO);
        log_debug(debug_logger,"puntero apuntado por el indirecto: %d ",  numeroBloqueDelPunteroDeBloques);
        return numeroBloqueDelPunteroDeBloques;
    }
}

uint32_t cantidadBytesQueNoSePuedeLeerEnUnBloque(uint32_t nuevoTamanioALeer){

    if (nuevoTamanioALeer > cfg_superbloque->BLOCK_SIZE){
        uint32_t cant = nuevoTamanioALeer - cfg_superbloque->BLOCK_SIZE;
        log_debug(debug_logger, "bytes que faltan leer: %d", cant);
        return cant;
    }else{
        return 0; //lo q hay q leer alcanza en un bloque
    }
}

uint32_t cantidadDisponibleDelBloque(uint32_t puntero){
    uint32_t cantidadDisponible = cfg_superbloque->BLOCK_SIZE - puntero;
    log_debug(debug_logger,"bytes para escrbir/leer en el bloque: %d", cantidadDisponible);
    return cantidadDisponible;
}

