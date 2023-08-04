//
// Created by utnso on 4/29/23.
//

#include <gestion_filesystem.h>
#include <sys/mman.h>

pthread_mutex_t mutex_cliente_socket;

t_list* lista_bloques;
void* bitarraycontent;
t_bitarray* bitmap;
t_config_superbloque* cfg_superbloque;
t_bloques* archivoBloques;
t_list* lista_FCBs;
t_list* archivosUsados;



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

       uint32_t bloque = 0;
       for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libreConBloque(bitmap, bloque) ;
          uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
          *nuevo_bloque_libre = bloque_libre;
          list_add(lista_bloques, nuevo_bloque_libre);
          bloque= bloque_libre+1;
       }

       /*
       
            uint32_t bloque = 0;
            for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

            uint32_t bloque_libre = obtener_bloque_libre(bitmap,bloque) ;
            uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
            *nuevo_bloque_libre = bloque_libre;
            list_add(lista_bloques, nuevo_bloque_libre);
            bloque= bloque_libre+1;
            }
       
       
       */

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
           accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+1, *puntero_a_escribir );
           uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;
           memcpy(archivoBloques->archivo + posicion_del_puntero_indirecto, puntero_a_escribir, sizeof(uint32_t));
           offset += sizeof(uint32_t);

       } 

        t_config* archivo_config = aux_FCB->fcb_config;
        char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
        char* string_puntero_directo = string_itoa(puntero_directo);
        char* string_puntero_indirecto = string_itoa(puntero_indirecto);

        config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
        config_set_value(archivo_config, "PUNTERO_DIRECTO", string_puntero_directo);
        config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_puntero_indirecto);
        config_save(archivo_config);
        free(string_puntero_directo);
        free(string_puntero_indirecto);
        free(string_nuevo_tamanio);
        free(enteroAuxPuntero);
        free(enteroAuxIndPuntero);
    // cambiaren el bitmap el bloque con 1 todos los bloques
    // agregar en el archivo el numero de bloque directo y el bloque con punteros 
    // agrego en el archivo de bloques los punteros 

    }else {

        uint32_t bloque = 0;
        //   uint32_t puntero_directo = obtener_bloque_libre(bitmap, bloque) ;

        uint32_t puntero_directo = obtener_bloque_libreConBloque(bitmap, bloque) ;
        aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
        aux_FCB->PUNTERO_DIRECTO = puntero_directo;

        accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, 0, aux_FCB->PUNTERO_DIRECTO);

        t_config* archivo_config = aux_FCB->fcb_config;
        char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
        char* string_puntero_directo = string_itoa(puntero_directo);
        config_set_value(archivo_config, "TAMANIO_ARCHIVO",string_nuevo_tamanio);
        config_set_value(archivo_config, "PUNTERO_DIRECTO", string_puntero_directo);
        config_save(archivo_config);
        free(string_nuevo_tamanio);
        free(string_puntero_directo);

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
            char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
            char* string_puntero_directo = string_itoa(0);
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
            config_set_value(archivo_config, "PUNTERO_DIRECTO", string_puntero_directo);

            config_save(archivo_config);
            free(string_nuevo_tamanio);
            free(string_puntero_directo);

         } else {

            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            t_config* archivo_config = aux_FCB->fcb_config;
            char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
            config_save(archivo_config);
            free(string_nuevo_tamanio);

            }
        } else {    // nuevo_tamanio  > 64

            uint32_t  cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / cfg_superbloque->BLOCK_SIZE);
            cantidad_de_bloques = cantidad_de_bloques - 1 ;

            uint32_t bloque = 0;
          for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

              uint32_t bloque_libre = obtener_bloque_libreConBloque(bitmap, bloque) ;
              uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
              *nuevo_bloque_libre = bloque_libre;
              list_add(lista_bloques, nuevo_bloque_libre);

             }
       /*
       
            uint32_t bloque = 0;
            for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

            uint32_t bloque_libre = obtener_bloque_libre(bitmap,bloque) ;
            uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
            *nuevo_bloque_libre = bloque_libre;
            list_add(lista_bloques, nuevo_bloque_libre);
            bloque= bloque_libre+1;
            }
       
       
       */
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
            char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
            char* string_puntero_indirecto = string_itoa(puntero_indirecto);
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
            config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_puntero_indirecto);
            config_save(archivo_config);
            free(string_nuevo_tamanio);
            free(string_puntero_indirecto);
            free(enteroAuxInd);
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
                accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, 0, puntero_directo);
                log_info(info_logger,"Acceso Bloque - Archivo: <%s> - Puntero Indirecto    - Bloque File System <%d>", aux_FCB->NOMBRE_ARCHIVO, puntero_indirecto);

                aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
                aux_FCB->PUNTERO_DIRECTO = 0;
                aux_FCB->PUNTERO_INDIRECTO = 0;

                uint32_t  offset = 0;

                for (uint32_t i = 0; i < (cantidad_de_bloques_a_eliminar-1); i++) { //elimina 1, si son2 bloques

                    uint32_t bloque_ocupado ;
                    uint32_t posicion_del_puntero_indirecto = puntero_indirecto * cfg_superbloque->BLOCK_SIZE + offset;

                    memcpy(&bloque_ocupado, archivoBloques->archivo + posicion_del_puntero_indirecto, sizeof(uint32_t));
                    offset += sizeof(uint32_t);
                    accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                    bitarray_clean_bit(bitmap,bloque_ocupado);
                    log_debug(debug_logger, "posicion del bloque ocupado que se va a borrar del bitmap: %d", bloque_ocupado);
                    accesoABitmap(bloque_ocupado, bitarray_test_bit(bitmap, bloque_ocupado));
                    accesoABloqueArchivo(aux_FCB->NOMBRE_ARCHIVO, i+1, bloque_ocupado);
                }
                char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
                char* string_puntero_directo = string_itoa(0);
                char* string_puntero_indirecto = string_itoa(0);
                config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio );
                config_set_value(archivo_config, "PUNTERO_DIRECTO", string_puntero_directo);
                config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_puntero_indirecto );
                config_save(archivo_config);
                free(string_puntero_indirecto);
                free(string_puntero_directo);
                free(string_nuevo_tamanio);
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
                char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
                char* string_puntero_indirecto = string_itoa(0);
                config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio );
                config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_puntero_indirecto );
                config_save(archivo_config);
                free(string_nuevo_tamanio);
                free(string_puntero_indirecto);
            } 
         } 

    else {

        uint32_t  cantidad_de_bloques_viejos = 1 + ((tamanio_archivo - 1) / cfg_superbloque->BLOCK_SIZE);
        uint32_t  cantidad_de_bloques        = 1 + ((nuevo_tamanio - 1) / cfg_superbloque->BLOCK_SIZE);

        if(cantidad_de_bloques > cantidad_de_bloques_viejos ){      //  agrandando

          cantidad_de_bloques = cantidad_de_bloques - cantidad_de_bloques_viejos ;
/*
          for (uint32_t i = 0; i < cantidad_de_bloques; i++) {

              uint32_t bloque_libre = obtener_bloque_libre(bitmap) ;
              uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
              *nuevo_bloque_libre = bloque_libre;
              list_add(lista_bloques, nuevo_bloque_libre);

          }*/

       
            uint32_t bloque = 0;
            for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

            uint32_t bloque_libre = obtener_bloque_libreConBloque(bitmap,bloque) ;
            uint32_t *nuevo_bloque_libre = malloc(sizeof(uint32_t));
            *nuevo_bloque_libre = bloque_libre;
            list_add(lista_bloques, nuevo_bloque_libre);
            bloque= bloque_libre+1;
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
         char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
         config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
         config_save(archivo_config);
         free(string_nuevo_tamanio);

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
            char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
            config_save(archivo_config);
            free(string_nuevo_tamanio);


    // agregar en el archivo, el bloque con punteros , puede que el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 
         } 

          if(cantidad_de_bloques == cantidad_de_bloques_viejos ) { 

            t_config* archivo_config = aux_FCB->fcb_config;
            aux_FCB->TAMANIO_ARCHIVO = nuevo_tamanio;
            char* string_nuevo_tamanio = string_itoa(nuevo_tamanio);
            config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_nuevo_tamanio);
            config_save(archivo_config);
            free(string_nuevo_tamanio);

        }
    }
}
}
    msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
    int tamanio = obtener_tamanio_bitmap();
    msync(bitmap, archivoBloques->tamanio, MS_SYNC);
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

    int tamanio = obtener_tamanio_bitmap();
    msync(bitmap, archivoBloques->tamanio, MS_SYNC);
	log_info(info_logger, "No se obtuvo un bloque libre");
    return -1;
}



uint32_t obtener_bloque_libreConBloque(t_bitarray* bitmap, uint32_t ultimo_bloque) {

    for (uint32_t i = ultimo_bloque ; i < bitarray_get_max_bit(bitmap); i++) {
            accesoABitmap(i, bitarray_test_bit(bitmap, i));

        if (!bitarray_test_bit(bitmap, i)) {

            bitarray_set_bit(bitmap, i);
            accesoABitmap(i, bitarray_test_bit(bitmap, i));

            return i;
        }
    }

    int tamanio = obtener_tamanio_bitmap();
    msync(bitmap, tamanio, MS_SYNC);
	log_info(info_logger, "No se obtuvo un bloque libre");
    return -1;
}





int obtener_tamanio_bitmap(){

    if(cfg_superbloque->BLOCK_COUNT%8 == 0){
        return cfg_superbloque->BLOCK_COUNT/8;
    }else {
        return cfg_superbloque->BLOCK_COUNT/8 + 1;
    }
}

void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(punteroArchivo, numeroBloque);

    log_debug(debug_logger,"tamanio archivo: %d", fcb->TAMANIO_ARCHIVO);
    escribirBloque(numeroBloque, posicionBloque, punteroArchivo, datos,tamanioDatos, fcb);
}

void escribirBloque(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* fcb){

    void* datoAEscribirFaltante =NULL;

    uint32_t bytesQueSePuedenEscrbirEnUnBloque = cantidadDisponibleDelBloque(posicionBloque);
    uint32_t cantidadBytesNoEscrita = 0;
    uint32_t bytesEscritos = 0;
    uint32_t numeroBloqueDelFS = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

    uint32_t bytesYaEscritos = 0;

    log_debug(debug_logger,"bytesQueSePuedenEscrbirEnUnBloque: %d", bytesQueSePuedenEscrbirEnUnBloque);

    //char* datoLeidoDeMemoria = malloc(tamanioAEscrbir +1);
    //memcpy(datoLeidoDeMemoria, datos, tamanioAEscrbir);
    //log_debug(debug_logger,"dato a escrbir en el archivo: %s", datoLeidoDeMemoria);
    //free(datoLeidoDeMemoria);
    if (bytesQueSePuedenEscrbirEnUnBloque >= tamanioAEscrbir) {
        log_debug(debug_logger,"SE ESCRIBE EN UN BLOQUE NADA MAS");
        //char* unDatoLeidoDeMemoria = malloc(tamanioAEscrbir +1);
        //memcpy(unDatoLeidoDeMemoria, datos, tamanioAEscrbir +1);
        //log_debug(debug_logger,"dato a escrbir en el archivo: %s", unDatoLeidoDeMemoria);
        memcpy(archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque, datos, tamanioAEscrbir);
        //free(unDatoLeidoDeMemoria);
        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

    }else{
        log_debug(debug_logger,"SE VA A ESCRIBIR EN MAS DE UN BLOQUE");
        memcpy(archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque, datos, bytesQueSePuedenEscrbirEnUnBloque);
        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

        bytesEscritos = bytesQueSePuedenEscrbirEnUnBloque;
        cantidadBytesNoEscrita = tamanioAEscrbir - bytesQueSePuedenEscrbirEnUnBloque;

        while(bytesEscritos != tamanioAEscrbir) {
            log_debug(debug_logger,"SE ESCRIBE EN EL SIGUIENTE BLOQUE");

            uint32_t cantidadBytesQueFaltanEscrbir = cantidadBytesQueNoSePuedeLeerEnUnBloque(cantidadBytesNoEscrita);
            uint32_t nuevoTamanio = cantidadBytesNoEscrita - cantidadBytesQueFaltanEscrbir;
            numeroBloque +=1;
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
        log_debug(debug_logger, "SE VA A LEER EN UN BLOQUE");
        datoLeido = malloc(tamanioALeer);
        memcpy(datoLeido, archivoBloques->archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque,  tamanioALeer);

        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

    }else{
        log_debug(debug_logger, "SE VA A LEER EN MAS DE UN BLOQUE");
        datoLeido = malloc(loQueSePuedeLeerEnUnBloque);
        memcpy(datoLeido, archivoBloques-> archivo + (numeroBloqueDelFS * cfg_superbloque->BLOCK_SIZE) + posicionBloque,  loQueSePuedeLeerEnUnBloque);
        accesoABloqueArchivo(fcb->NOMBRE_ARCHIVO, numeroBloque, numeroBloqueDelFS);

        cantidadBytesNoLeida = tamanioALeer - loQueSePuedeLeerEnUnBloque;
        bytesLeidos = loQueSePuedeLeerEnUnBloque;

        log_debug(debug_logger, "Primera lectur, bytes leidos: %d", bytesLeidos);

        while(bytesLeidos != tamanioALeer) {
            log_debug(debug_logger, "SE LEE EN EL SIGUIENTE BLOQUE");

            uint32_t cantidadBytesQueFaltanLeer = cantidadBytesQueNoSePuedeLeerEnUnBloque(cantidadBytesNoLeida);
            log_debug(debug_logger, "Cantidad de bytes que no se van a leer en este ciclo: %d", cantidadBytesQueFaltanLeer);
            uint32_t nuevoTamanioALeer = cantidadBytesNoLeida - cantidadBytesQueFaltanLeer;
            log_debug(debug_logger, "tamanio nuevo a leer: %d", nuevoTamanioALeer);
            numeroBloque += 1;
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

            bytesLeidos += nuevoTamanioALeer;
            cantidadBytesNoLeida = cantidadBytesQueFaltanLeer;
            //free(datoLeidoNuevo);
            //datoLeidoNuevo = NULL; //solo va a contener lo que necesito leer en cada ciclo
        }
    }

    char* datoLeidoDeMemoria = malloc(tamanioALeer);
    memcpy(datoLeidoDeMemoria,datoLeido, tamanioALeer);
    log_debug(debug_logger,"dato leido del archivo: %s", datoLeidoDeMemoria);
    free(datoLeidoDeMemoria);
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

    uint32_t offset = sizeof(uint32_t) * (numero_bloque- 2);
    if (numero_bloque == 1) {
        log_debug(debug_logger,"puntero directo: %d ",  fcb->PUNTERO_DIRECTO);
        return fcb->PUNTERO_DIRECTO;

    }else {

        uint32_t offset = sizeof(uint32_t) * (numero_bloque - 2);
        log_debug(debug_logger, "offset para el bloque de punteros: %d", offset);
        uint32_t numeroBloqueDelPunteroDeBloques;
        memcpy(&numeroBloqueDelPunteroDeBloques,
               archivoBloques->archivo + (fcb->PUNTERO_INDIRECTO * cfg_superbloque->BLOCK_SIZE) + offset,
               sizeof(uint32_t));

        log_debug(debug_logger, "puntero indirecto: %d ", fcb->PUNTERO_INDIRECTO);
        log_debug(debug_logger, "puntero apuntado por el indirecto: %d ", numeroBloqueDelPunteroDeBloques);
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

