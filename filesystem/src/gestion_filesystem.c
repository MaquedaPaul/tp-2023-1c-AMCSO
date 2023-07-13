//
// Created by utnso on 4/29/23.
//

#include <gestion_filesystem.h>
t_bitmapBloques* bitmapDeBloques;
t_bloques* archivoBloques;
t_bitarray* bitarrayBitmapDeBloques;
t_config_superbloque* cfg_superbloque;

bool existeFcbConNombre(char* nombreArchivo){
    //TODO
    return true;
}

t_config_fcb* buscarFCBporNombre(char* nombre){
    t_config_fcb* fcb;
    //TODO
    return fcb;
}


void realizarCreacionArchivo(char* nombreArchivo){

}
void realizarTruncacionArchivo(char* nombreArchivo){

    uint32_t tamanio_del_archivo_a_truncar;
    uint32_t tamanio_de_lista_FCBs = list_size(lista_FCBs);
    for (int i = 0; i < tamanio_de_lista_FCBs; i++) {

        t_config_fcb *aux_FCB = list_get(lista_FCBs, i);
        if (strcmp(aux_FCB->nombre_archivo, nombre_del_archivo_a_truncar) == 0) {

            tamanio_del_archivo_a_truncar = aux_FCB->tamanio_archivo;

            ampliar_o_reducir_tamanio(aux_FCB, nuevo_tamanio_del_archivo, tamanio_del_archivo_a_truncar); 
        
    }
         }
}


void ampliar_o_reducir_tamanio(t_config_fcb *aux_FCB, uint32_t nuevo_tamanio, uint32_t tamanio_archivo) { 

if(tamanio_archivo == 0){ 

    if(nuevo_tamanio > tamanio_del_bloque){

      lista_bloques = list_create();

      uint32_t cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / tamanio_del_bloque);

       for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libre(auxBitArray) ;
          list_add(lista_bloques, bloque_libre);
       } 

     aux_FCB->puntero_directo = list_remove(lista_bloques,0);
     aux_FCB->puntero_indirecto = list_remove(lista_bloques,0);

     uint32_t  puntero_directo = aux_FCB->puntero_directo ;
     uint32_t  puntero_indirecto = aux_FCB->puntero_indirecto ;


     uint32_t cantidad_de_punteros = list_size(lista_bloques);

     uint32_t  offset = 0;

       for (uint32_t i = 0; i < cantidad_de_punteros; i++) {

             uint32_t puntero_a_escribir = list_get(lista_bloques,i);

             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(superbloque + posicion_del_puntero_indirecto, &puntero_a_escribir, sizeof(uint32_t));
             offset += sizeof(uint32_t);

       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", nuevo_tamanio);
    config_set_value(archivo_config, "PUNTERO_DIRECTO", puntero_directo);     
    config_set_value(archivo_config, "PUNTERO_INDIRECTO", puntero_indirecto); 
    config_save(archivo_config);

    // cambiaren el bitmap el bloque con 1 todos los bloques
    // agregar en el archivo el numero de bloque directo y el bloque con punteros 
    // agrego en el archivo de bloques los punteros 

    } else {    // nuevo_tamanio < tamanio_del_bloque


      uint32_t puntero_directo = obtener_bloque_libre(auxBitArray) ;

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", nuevo_tamanio);
    config_set_value(archivo_config, "PUNTERO_DIRECTO", puntero_directo);     
    config_save(archivo_config);

    // cambiaren en el bitmap el bloque que voy a ocupar  con 1
    // agregar en el archivo el numero de bloque directo
            
    }

} else {   // tamanio_archivo > 0

    if(tamanio_archivo =< tamanio_del_bloque){

        if(nuevo_tamanio=< tamanio_del_bloque ){    //  cantidad_de_bloques = 1 ;

        } else {    // nuevo_tamanio  > 64

           lista_bloques = list_create();

            cantidad_de_bloques = 1 + ((nuevo_tamanio - 1) / tamanio_del_bloque);

             cantidad_de_bloques = cantidad_de_bloques - 1 ;

          for (uint32_t i = 0; i <= cantidad_de_bloques; i++) {

          uint32_t bloque_libre = obtener_bloque_libre(auxBitArray) ;
          list_add(lista_bloques, bloque_libre);

         } 

        uint32_t puntero_directo = config_get_value(archivo_config, "PUNTERO_DIRECTO"); 

        uint32_t puntero_indirecto = list_remove(lista_bloques,0);

        uint32_t cantidad_de_punteros = list_size(lista_bloques);

         uint32_t  offset = sizeof(uint32_t);

         for (uint32_t i = 0; i < cantidad_de_punteros; i++) {

             uint32_t puntero_a_escribir = list_get(lista_bloques,i);

             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(superbloque + posicion_del_puntero_indirecto, &puntero_a_escribir, sizeof(uint32_t));
             offset += sizeof(uint32_t);

       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", nuevo_tamanio);
    config_set_value(archivo_config, "PUNTERO_INDIRECTO", puntero_indirecto); 
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
          list_add(lista_bloques, bloque_libre);

         } 

        uint32_t puntero_directo = config_get_value(archivo_config, "PUNTERO_DIRECTO"); 
        uint32_t puntero_indirecto = config_get_value(archivo_config, "PUNTERO_INDIRECTO"); 

        uint32_t cantidad_de_punteros = list_size(lista_bloques);

        uint32_t  offset = sizeof(uint32_t) * cantidad_de_bloques_viejos;


        for (uint32_t i = 0; i < cantidad_de_punteros; i++) {

             uint32_t puntero_a_escribir = list_get(lista_bloques,i);

             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(superbloque + posicion_del_puntero_indirecto, &puntero_a_escribir, sizeof(uint32_t));
             offset += sizeof(uint32_t);

       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", nuevo_tamanio);
    config_save(archivo_config);

   // cambiaren el bitmap el bloque con 1 todos los bloques menos los que ya estan ocupados
    // el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 
             

        } else {   // cantidad_de_bloques < cantidad_de_bloques_viejos


           uint32_t  cantidad_de_bloques_a_eliminar = cantidad_de_bloques_viejos - cantidad_de_bloques ;

          if (cantidad_de_bloques > 1){
               
          uint32_t puntero_indirecto = config_get_value(archivo_config, "PUNTERO_INDIRECTO"); 

        uint32_t  offset = sizeof(uint32_t) * (cantidad_de_bloques - 1);

        for (uint32_t i = 0; i < cantidad_de_bloques_a_eliminar; i++) {

             uint32_t bloque_ocupado ;
             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(&bloque_ocupado, superbloque + posicion_del_puntero_indirecto, sizeof(uint32_t));
             offset += sizeof(uint32_t);

            bitarray_clean_bit(bitmap,bloque_ocupado );
       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", nuevo_tamanio);
    config_save(archivo_config);


    // agregar en el archivo, el bloque con punteros , puede que el bloque con punteros ya exista
    // agrego en el archivo de bloques los punteros 

          }

          if (cantidad_de_bloques == 1){

                      uint32_t puntero_indirecto = config_get_value(archivo_config, "PUNTERO_INDIRECTO"); 

        uint32_t  offset = sizeof(uint32_t) * (cantidad_de_bloques - 1);

        for (uint32_t i = 0; i < cantidad_de_bloques_a_eliminar; i++) {

             uint32_t bloque_ocupado ;
             uint32_t posicion_del_puntero_indirecto = puntero_indirecto * tamanio_del_bloque + offset;
 
             memcpy(&bloque_ocupado, superbloque + posicion_del_puntero_indirecto, sizeof(uint32_t));
             offset += sizeof(uint32_t);

            bitarray_clean_bit(bitmap,bloque_ocupado );
       } 

    config_set_value(archivo_config, "TAMANIO_ARCHIVO", nuevo_tamanio);
    config_set_value(archivo_config, "PUNTERO_INDIRECTO", 0); 
    config_save(archivo_config);


             }

  } 
    }
}

}








void realizarEscrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, void* datos, uint32_t tamanioDatos){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(fcb->TAMANIO_ARCHIVO, numeroBloque);
    int posicionInicial = buscarPunteroInicioDelBloque(numeroBloque, fcb);

    escribirBloque(numeroBloque, posicionBloque, punteroArchivo, datos,tamanioDatos, fcb);

}

void escribirBloque(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, void* datos, uint32_t tamanioAEscrbir, t_config_fcb* fcb){

    void* datoEscrito = NULL;
    uint32_t bytesQueSePuedenEscrbirEnUnBloque = cantidadDisponibleDelBloque(punteroBloque);
    uint32_t cantidadBytesNoEscrita = 0;

    if (bytesQueSePuedenEscrbirEnUnBloque >= tamanioAEscrbir) { //1 >= 6
        //LO QUE SE TIENE Q ESCRIBIR ESTA EN UN BLOQUE
        //escribir archivo de bloques a partir de la posicionInicialBloque con el tamanioALeer de una
    }else{

        //leer archivo de bloques con el tamaño = bytesQueSePuedenEscrbirEnUnBloque, desde la posicion "punteroBloque", y buscar el SIGUIENTE bloque para terminar de leer los bytes
        //lo que escribo en el archivo, lo copio en datoEscrito

        cantidadBytesNoEscrita = tamanioAEscrbir - bytesQueSePuedenEscrbirEnUnBloque;
    }

    while(sizeof(datoEscrito) != tamanioAEscrbir) {

        uint32_t cantidadBytesQueFaltanEscrbir = cantidadBytesQueFaltaOperar(cantidadBytesNoEscrita);
        uint32_t nuevoTamanio = cantidadBytesNoEscrita - cantidadBytesQueFaltanEscrbir;
        numeroBloque++;
        uint32_t posicionNuevoBloque = buscarPunteroInicioDelBloque(numeroBloque, fcb);

        //escrbir archivo de bloques con el nuevoTamaño a partir del posicionNuevoBloque
        //lo que escribo en el archivo, lo copio en datoEscrito

        cantidadBytesNoEscrita = cantidadBytesQueFaltanEscrbir;

    }

}

void* realizarLecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t  tamanio){

    t_config_fcb* fcb = buscarFCBporNombre(nombreArchivo);
    uint32_t numeroBloque = numeroDeBloque(punteroArchivo);
    uint32_t posicionBloque = buscarPosicionDentroDelBloque(punteroArchivo,numeroBloque);
    int punteroAlBloqueDeDatos = buscarPunteroInicioDelBloque(numeroBloque, fcb);  //buscar puntero de inicio del bloque correspondiende al numero_bloque

    void* datoLeido = leer_archivo(numeroBloque, posicionBloque, punteroArchivo, tamanio, fcb);

    return datoLeido;

}


void* leer_archivo(int numeroBloque, uint32_t punteroBloque, uint32_t punteroArchivo, uint32_t tamanioALeer, t_config_fcb* fcb){

    void* datoLeido = NULL;
    uint32_t loQueSePuedeLeerEnUnBloque = cantidadDisponibleDelBloque(punteroBloque); //2
    uint32_t cantidadBytesNoLeida;

    if (loQueSePuedeLeerEnUnBloque >= tamanioALeer) { //8>=10 ,,

        //LO QUE SE TIENE Q LEER ESTA EN UN BLOQUE
        //leer archivo de bloques a partir del numeroBloque, y una vez encontrado el bloque usar punteroBloque con el tamanioALeer de una

    }else{

        //se va a leer el bloque pero lo que se pueda
        //leer archivo de bloques a partir del numeroBloque, y una vez encontrado el bloque usar punteroBloque con el tamanioALeer= loQueSePuedeLeerEnUnBloque.

        //y buscar el SIGUIENTE bloque para terminar de leer los bytes

        cantidadBytesNoLeida = tamanioALeer - loQueSePuedeLeerEnUnBloque; // 10-8 = 5,,, 12-2=10
    }


    while(sizeof(datoLeido) != tamanioALeer) { //8 != 10,, 2!=10

        uint32_t cantidadBytesQueFaltanLeer = cantidadBytesQueFaltaOperar(cantidadBytesNoLeida);  // 5- 3 = 2
        uint32_t nuevoTamanio = cantidadBytesNoLeida - cantidadBytesQueFaltanLeer; //10-2= 8
        numeroBloque++;
        uint32_t posicionNuevoBloque = buscarPunteroInicioDelBloque(numeroBloque, fcb);

        //leer archivo de bloques con el nuevoTamaño a partir del posicionNuevoBloque
        cantidadBytesNoLeida = cantidadBytesQueFaltanLeer; //2
    }

    return datoLeido;
}

int numeroDeBloque(uint32_t punteroArchivo) {

    return punteroArchivo/cfg_superbloque->BLOCK_SIZE;
}

int buscarPosicionDentroDelBloque(uint32_t puntero, uint32_t numeroBloque){ //ej: size bloque: 16, 0 a 15 devuelve

    uint32_t offset = puntero % cfg_superbloque->BLOCK_SIZE; //me devuelve un puntero desde donde hay que escrbir o leer dentro del bloque

}

uint32_t buscarPunteroInicioDelBloque(int numero_bloque, t_config_fcb* fcb) {
    if (numero_bloque == 1) {
        return fcb->PUNTERO_DIRECTO;
    } else {
        //buscar en el archivo de bloque desde la posicion del punteroIndirecto, el puntero correspondiendo numeroBloque
        fcb->PUNTERO_INDIRECTO;
        return 1;

    }
}

uint32_t cantidadBytesQueFaltaOperar(uint32_t nuevoTamanioALeer){

    if (nuevoTamanioALeer > cfg_superbloque->BLOCK_SIZE){ //2   .. 16| 10 .. 8
        return  nuevoTamanioALeer - cfg_superbloque->BLOCK_SIZE; //10-8 =2
    }else{
        return 0; //lo q hay q leer alcanza en un bloque
    }
}

uint32_t cantidadDisponibleDelBloque(uint32_t puntero){
    return cfg_superbloque->BLOCK_SIZE - puntero;
}

