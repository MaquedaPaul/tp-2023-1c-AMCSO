//
// Created by utnso on 4/29/23.
//

#include <gestion_filesystem.h>
t_bitmapBloques* bitmapDeBloques;
t_bloques* archivoBloques;
t_bitarray* bitarrayBitmapDeBloques;
t_config_superbloque* cfg_superbloque;
t_list* lista_FCBs; //TODO DEFINIR
char* nombre_del_archivo_a_truncar;//TODO DEFINIR

bool existeFcbConNombre(char* nombreArchivo){
    //TODO
    return true;
}

int existe_archivoFCB(char *nombre_archivo) {

    int tamanio_lista_FCBs = list_size(lista_FCBs);

    for (int i = 0; i < tamanio_lista_FCBs; i++)  {

        t_config_fcb *aux_FCB = list_get(lista_FCBs, i);

        if (strcmp(aux_FCB->NOMBRE_ARCHIVO, nombre_archivo) == 0)  {

            log_info(info_logger, "Abrir archivo: <%s>", nombre_archivo);

            return 1;
        }
    }

    return 0;
}


t_config_fcb* buscarFCBporNombre(char* nombre){
    t_config_fcb* fcb;
    //TODO
    return fcb;
}


void realizarCreacionArchivo(char* nombreArchivo){
    char* punto_PATH_FCB = cfg_filesystem->PATH_FCB; //TODO DEFINIR
    char* nombre_de_archivo = nombreArchivo; //TODO DEFINIR
	char* path =string_new();
	string_append(&path, punto_PATH_FCB);
	string_append(&path, "/");
	string_append(&path, nombre_de_archivo);
	string_append(&path,".dat");

	FILE* f = fopen(path,"wb");
        fclose(f);

	t_config* aux_config = config_create(path);

	config_set_value(aux_config,"NOMBRE_ARCHIVO",nombre_de_archivo);
	config_set_value(aux_config,"TAMANIO_ARCHIVO","0");
	config_set_value(aux_config,"PUNTERO_DIRECTO", "0");
	config_set_value(aux_config,"PUNTERO_INDIRECTO", "0");

	config_save(aux_config);

            t_config_fcb* aux_FCB = malloc(sizeof(t_config_fcb));

            int tamanio_nombre_archivo = strlen(nombre_de_archivo);

            aux_FCB->NOMBRE_ARCHIVO = malloc( tamanio_nombre_archivo + 1  );
            strcpy( aux_FCB->NOMBRE_ARCHIVO, nombre_de_archivo );

            aux_FCB->TAMANIO_ARCHIVO = 0;
            aux_FCB->PUNTERO_DIRECTO = 0;
            aux_FCB->PUNTERO_INDIRECTO = 0;

            aux_FCB->fcb_config = aux_config;

	  list_add(lista_FCBs,aux_FCB);

	log_info(info_logger,"Se creo el archivo en: %s", path);
	free(path);




}
void realizarTruncacionArchivo(char* nombreArchivo, uint32_t nuevo_tamanio_del_archivo){

    uint32_t tamanio_del_archivo_a_truncar;
    uint32_t tamanio_de_lista_FCBs = list_size(lista_FCBs);
    for (int i = 0; i < tamanio_de_lista_FCBs; i++) {

        t_config_fcb *aux_FCB = list_get(lista_FCBs, i);
        if (strcmp(aux_FCB->NOMBRE_ARCHIVO, nombre_del_archivo_a_truncar) == 0) {

            tamanio_del_archivo_a_truncar = aux_FCB->TAMANIO_ARCHIVO;

            ampliar_o_reducir_tamanio(aux_FCB, nuevo_tamanio_del_archivo, tamanio_del_archivo_a_truncar); 
        
    }
         }
}


void ampliar_o_reducir_tamanio(t_config_fcb *aux_FCB, uint32_t nuevo_tamanio, uint32_t tamanio_archivo) { 
t_list* lista_bloques; //TODO DEFINIR
uint32_t tamanio_del_bloque; //TODO DEFINIR
t_config* archivo_config; //TODO DEFINIR
t_bitarray* auxBitArray; //TODO DEFINIR
uint32_t cantidad_de_bloques; //TODO DEFINIR
t_bitarray* bitmap; //TODO DEFINIR
void* superbloque = archivoBloques->archivo; //TODO DEFINIR ?

if(tamanio_archivo == 0){ 

    if(nuevo_tamanio > tamanio_del_bloque){

      lista_bloques = list_create();

      uint32_t cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / tamanio_del_bloque);

       for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libre(auxBitArray) ;
          list_add(lista_bloques, &bloque_libre);
       }
     uint32_t* enteroAuxPuntero = list_remove(lista_bloques,0);
     aux_FCB->PUNTERO_DIRECTO = *enteroAuxPuntero;
     uint32_t* enteroAuxIndPuntero = list_remove(lista_bloques,0);
     aux_FCB->PUNTERO_INDIRECTO = *enteroAuxIndPuntero;


     uint32_t  puntero_directo = aux_FCB->PUNTERO_DIRECTO ;


     uint32_t  puntero_indirecto = aux_FCB->PUNTERO_INDIRECTO ;


     uint32_t cantidad_de_punteros = list_size(lista_bloques);

     uint32_t  offset = 0;

       for (uint32_t i = 0; i < cantidad_de_punteros; i++) {
             uint32_t* enteroAuxPuntero = list_get(lista_bloques,i);
             uint32_t puntero_a_escribir = *enteroAuxPuntero;

             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(superbloque + posicion_del_puntero_indirecto, &puntero_a_escribir, sizeof(uint32_t));
             offset += sizeof(uint32_t);

       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
    config_set_value(archivo_config, "PUNTERO_DIRECTO", string_itoa((int)puntero_directo));
    config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_itoa((int)puntero_indirecto));
    config_save(archivo_config);

    // cambiaren el bitmap el bloque con 1 todos los bloques
    // agregar en el archivo el numero de bloque directo y el bloque con punteros 
    // agrego en el archivo de bloques los punteros 

    } else {    // nuevo_tamanio < tamanio_del_bloque


      uint32_t puntero_directo = obtener_bloque_libre(auxBitArray) ;

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
    config_set_value(archivo_config, "PUNTERO_DIRECTO", string_itoa((int)puntero_directo));
    config_save(archivo_config);

    // cambiaren en el bitmap el bloque que voy a ocupar  con 1
    // agregar en el archivo el numero de bloque directo
            
    }

} else {   // tamanio_archivo > 0

    if(tamanio_archivo <= tamanio_del_bloque){

        if(nuevo_tamanio <= tamanio_del_bloque ){    //  cantidad_de_bloques = 1 ;

        } else {    // nuevo_tamanio  > 64

           lista_bloques = list_create();

            cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / tamanio_del_bloque);

             cantidad_de_bloques = cantidad_de_bloques - 1 ;

          for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libre(auxBitArray) ;
          list_add(lista_bloques, &bloque_libre);

         } 

        uint32_t puntero_directo = config_get_int_value(archivo_config, "PUNTERO_DIRECTO");
        uint32_t* enteroAuxInd = list_remove(lista_bloques,0);
        uint32_t puntero_indirecto = *enteroAuxInd;

        uint32_t cantidad_de_punteros = list_size(lista_bloques);

         uint32_t  offset = sizeof(uint32_t);

         for (int i = 0; i < cantidad_de_punteros; i++) {
             uint32_t* enteroAuxPunteroTemp = list_get(lista_bloques,i);
             uint32_t puntero_a_escribir = *enteroAuxPunteroTemp;

             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(superbloque + posicion_del_puntero_indirecto, &puntero_a_escribir, sizeof(uint32_t));
             offset += sizeof(uint32_t);

       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
    config_set_value(archivo_config, "PUNTERO_INDIRECTO", string_itoa((int)puntero_indirecto));
    config_save(archivo_config);


    // cambiaren el bitmap el bloque con 1 todos los bloques menos el primero que ya esta ocupado
    // agregar en el archivo, el bloque con punteros , puede que el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 
        }

    } else { // si tamaño es mayor al bloque  tamanio_archivo > tamanio_del_bloque
              
              
       int cantidad_de_bloques_viejos = 1 + ((tamanio_archivo - 1) / tamanio_del_bloque);

            cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / tamanio_del_bloque);

      //   int tamanioSobrante = cantidad_de_bloques_viejos* tamanioDelBloque - tamanio_archivo ;
 
        if(cantidad_de_bloques > cantidad_de_bloques_viejos ){      //  agrandando
 
           // nuevo_tamanio = nuevo_tamanio - cantidad_de_bloques_viejos* tamanioDelBloque;

             cantidad_de_bloques = cantidad_de_bloques - cantidad_de_bloques_viejos ;

              lista_bloques = list_create();

          for (uint32_t i = 0; i < cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libre(auxBitArray) ;
          list_add(lista_bloques, &bloque_libre);

         } 

        uint32_t puntero_directo = config_get_int_value(archivo_config, "PUNTERO_DIRECTO");
        uint32_t puntero_indirecto = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");

        uint32_t cantidad_de_punteros = list_size(lista_bloques);

        uint32_t  offset = sizeof(uint32_t) * cantidad_de_bloques_viejos;


        for (int i = 0; i < cantidad_de_punteros; i++) {
             uint32_t* enteroAuxPuntero = list_get(lista_bloques,i);
             uint32_t puntero_a_escribir = *enteroAuxPuntero;

             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(superbloque + posicion_del_puntero_indirecto, &puntero_a_escribir, sizeof(uint32_t));
             offset += sizeof(uint32_t);

       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
    config_save(archivo_config);

   // cambiaren el bitmap el bloque con 1 todos los bloques menos los que ya estan ocupados
    // el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 
             

        } else {   // cantidad_de_bloques < cantidad_de_bloques_viejos


           uint32_t  cantidad_de_bloques_a_eliminar = cantidad_de_bloques_viejos - cantidad_de_bloques ;

          if (cantidad_de_bloques > 1){
               
          uint32_t puntero_indirecto = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");

        uint32_t  offset = sizeof(uint32_t) * (cantidad_de_bloques - 1);

        for (uint32_t i = 0; i < cantidad_de_bloques_a_eliminar; i++) {

             uint32_t bloque_ocupado ;
             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(&bloque_ocupado, superbloque + posicion_del_puntero_indirecto, sizeof(uint32_t));
             offset += sizeof(uint32_t);

            bitarray_clean_bit(bitmap,bloque_ocupado );
       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
    config_save(archivo_config);


    // agregar en el archivo, el bloque con punteros , puede que el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 

          }

          if (cantidad_de_bloques == 1){

                      uint32_t puntero_indirecto = config_get_int_value(archivo_config, "PUNTERO_INDIRECTO");

        uint32_t  offset = sizeof(uint32_t) * (cantidad_de_bloques - 1);

        for (uint32_t i = 0; i < cantidad_de_bloques_a_eliminar; i++) {

             uint32_t bloque_ocupado ;
             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(&bloque_ocupado, superbloque + posicion_del_puntero_indirecto, sizeof(uint32_t));
             offset += sizeof(uint32_t);

            bitarray_clean_bit(bitmap,bloque_ocupado );
       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", string_itoa((int)nuevo_tamanio));
    config_set_value(archivo_config, "PUNTERO_INDIRECTO", 0); 
    config_save(archivo_config);


             }

  } 
    }
}

}



uint32_t obtener_bloque_libre(t_bitarray* auxBitArray) {

    for (uint32_t i = 0; i < bitarray_get_max_bit(auxBitArray); i++) {

        if (!bitarray_test_bit(auxBitArray, i)) {

            bitarray_set_bit(auxBitArray, i);

            return i;
        }

    }

	log_info(info_logger, "No se obtuvo un bloque libre");

    return -1;

}


void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(fcb->TAMANIO_ARCHIVO, numeroBloque);

    escribirBloque(numeroBloque, posicionBloque, punteroArchivo, datos,tamanioDatos, fcb);

}

void escribirBloque(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* fcb){ //tamanioAEscrbir = 11

    uint32_t offset;
    void* datoAEscribirFaltante =NULL;

    uint32_t bytesQueSePuedenEscrbirEnUnBloque = cantidadDisponibleDelBloque(posicionBloque);
    uint32_t cantidadBytesNoEscrita = 0;
    uint32_t bytesEscritos = 0;

    uint32_t numeroBloqueArchivoGlobal = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

    if (bytesQueSePuedenEscrbirEnUnBloque >= tamanioAEscrbir) {

        memcpy(archivoBloques->archivo + (numeroBloqueArchivoGlobal - 1 * cfg_superbloque->BLOCK_SIZE), datos, tamanioAEscrbir);
        bytesEscritos = tamanioAEscrbir;

    }else{

        memcpy(archivoBloques->archivo + (numeroBloqueArchivoGlobal - 1 * cfg_superbloque->BLOCK_SIZE), datos, bytesQueSePuedenEscrbirEnUnBloque); // bytesQueSePuedenEscrbirEnUnBloque = 1

        bytesEscritos = bytesQueSePuedenEscrbirEnUnBloque; //se escribio 1
        cantidadBytesNoEscrita = tamanioAEscrbir - bytesQueSePuedenEscrbirEnUnBloque; //11 - 1 = 10

        offset = bytesQueSePuedenEscrbirEnUnBloque; //lo que se lleyo, offset = 1, me sirve para leer "datos"

        while(bytesEscritos != tamanioAEscrbir) { //datos = holacomoe st , 1º ya escrbir  ç

            uint32_t cantidadBytesQueFaltanEscrbir = cantidadBytesQueNoSePuedeLeerEnUnBloque(cantidadBytesNoEscrita); //2  "st"
            uint32_t nuevoTamanio = cantidadBytesNoEscrita - cantidadBytesQueFaltanEscrbir; //10 - 2 = 8,         2º: 2

            datoAEscribirFaltante = malloc(nuevoTamanio);
            memcpy(datoAEscribirFaltante, datos + offset, nuevoTamanio); //copio lo que falta escrbir  datos + offset = 8, 		2º= offeset = 9
            numeroBloque++;
            numeroBloqueArchivoGlobal = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

            memcpy(archivoBloques->archivo + ((numeroBloqueArchivoGlobal - 1) * cfg_superbloque->BLOCK_SIZE), datoAEscribirFaltante, nuevoTamanio);

            cantidadBytesNoEscrita = cantidadBytesQueFaltanEscrbir; //2  "st"
            bytesEscritos += nuevoTamanio;
            offset += nuevoTamanio; //offset = 1 + 8 = 9, osea lo que se leyo

            free(datoAEscribirFaltante); //libero lo que escribi en este ciclo
            datoAEscribirFaltante = NULL; //solo va a contener lo que necesito escrbir en cada ciclo
        }

    }
}

void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t tamanio){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo); //1
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(punteroArchivo, numeroBloque); //6

    void* datoLeido = leer_archivo(numeroBloque, posicionBloque, punteroArchivo, tamanio, fcb);

    return datoLeido;

}


void* leer_archivo(int numeroBloque, uint32_t posicionBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb){

    void* datoLeido = NULL;
    void* datoLeidoNuevo = NULL;
    uint32_t loQueSePuedeLeerEnUnBloque = cantidadDisponibleDelBloque(posicionBloque); //2
    uint32_t cantidadBytesNoLeida;
    uint32_t punteroBloqueDentroDelArchivo;

    if (loQueSePuedeLeerEnUnBloque >= tamanioALeer) {

        //Lo que se tiene q leer se puede leer en un bloque
        char bytesALeer[tamanioALeer];
        datoLeido = malloc(sizeof(bytesALeer));
        memcpy(datoLeido, archivoBloques->archivo + (posicionBloque-1 * cfg_superbloque->BLOCK_SIZE) + posicionBloque,  tamanioALeer);

    }else{

        datoLeido = malloc(sizeof (loQueSePuedeLeerEnUnBloque));
        memcpy(datoLeido, archivoBloques-> archivo + (posicionBloque-1 * cfg_superbloque->BLOCK_SIZE) + posicionBloque,  loQueSePuedeLeerEnUnBloque);

        cantidadBytesNoLeida = tamanioALeer - loQueSePuedeLeerEnUnBloque;
    }

    while(sizeof(datoLeido) != tamanioALeer) {

        uint32_t cantidadBytesQueFaltanLeer = cantidadBytesQueNoSePuedeLeerEnUnBloque(cantidadBytesNoLeida); //verifico si el nuevoTamaño a leer se puede leer en 1 bloque, si es asi da 0
        uint32_t nuevoTamanioALeer = cantidadBytesNoLeida - cantidadBytesQueFaltanLeer; //si alcanza en un bloque va a ser igual a cantidadBytesNoLeida
        numeroBloque++;
        punteroBloqueDentroDelArchivo = buscarNumeroDeBloqueDelArchivoDeBloque(numeroBloque, fcb);

        datoLeidoNuevo = malloc(nuevoTamanioALeer);
        memcpy(&datoLeidoNuevo, archivoBloques-> archivo + (punteroBloqueDentroDelArchivo - 1 * cfg_superbloque->BLOCK_SIZE),  nuevoTamanioALeer);

        uint32_t cantidadBytesLeidos = sizeof(datoLeido);
        void* datoLeido_temporal = realloc(datoLeido, cantidadBytesLeidos + nuevoTamanioALeer); //Cuando se redimensiona la memoria con realloc, si el nuevo tamaño es mayor que el anterior, se conservan todos los valores originales

        if(datoLeido_temporal == NULL){
            //ERROR
        }
        datoLeido = datoLeido_temporal;
        memcpy(datoLeido + nuevoTamanioALeer, datoLeidoNuevo, nuevoTamanioALeer); //escribo en el datoLeido el nuevo contenido, voy concatenando lo leido
        free(datoLeidoNuevo);
        datoLeidoNuevo = NULL; //solo va a contener lo que necesito leer en cada ciclo

        cantidadBytesNoLeida = cantidadBytesQueFaltanLeer;
    }

    return datoLeido;
}

int numeroDeBloque(uint32_t punteroArchivo) {

    return (punteroArchivo/cfg_superbloque->BLOCK_SIZE) + 1;
}

int buscarPosicionDentroDelBloque(uint32_t punteroArchivo, uint32_t numeroBloque){ //ej: size bloque: 16, 0 a 15 devuelve

    uint32_t offset = punteroArchivo % cfg_superbloque->BLOCK_SIZE; //me devuelve un puntero desde donde hay que escrbir o leer dentro del bloque

}

uint32_t buscarNumeroDeBloqueDelArchivoDeBloque(int numero_bloque, t_config_fcb* fcb) {
    if (numero_bloque == 1) {
        return fcb->PUNTERO_DIRECTO;

    } else {

        void* punteroAUnBloque;
        memcpy(&punteroAUnBloque, archivoBloques-> archivo + ((fcb -> PUNTERO_INDIRECTO -1) * cfg_superbloque->BLOCK_SIZE) + (numero_bloque-1), sizeof(uint32_t));
        (char*) punteroAUnBloque;

        return atoi(punteroAUnBloque);
    }
}

uint32_t cantidadBytesQueNoSePuedeLeerEnUnBloque(uint32_t nuevoTamanioALeer){

    if (nuevoTamanioALeer > cfg_superbloque->BLOCK_SIZE){ //2   .. 16| 10 .. 8
        return  nuevoTamanioALeer - cfg_superbloque->BLOCK_SIZE; //10-8 =2
    }else{
        return 0; //lo q hay q leer alcanza en un bloque
    }
}

uint32_t cantidadDisponibleDelBloque(uint32_t puntero){
    return cfg_superbloque->BLOCK_SIZE - puntero;
}

