#include <protocolo.h>
bool ALL_DETAILS = false;

//!! CUIDADO PORQUE RECIBE UINT32_T Y MUCHOS METODOS PASAN INT COMO SOCKET, Y AL REVES EL TAMAÑO
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////FUNCIONES GENERICAS//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void enviarOrden(op_code orden, int socket, t_log *logger) {
    t_paquete * paquete= crear_paquete(orden, logger);
    paquete->buffer->size+=sizeof (uint32_t);
    void* stream = malloc(paquete->buffer->size);
    uint32_t valor = 0;
    int offset= 0;
    memcpy(stream + offset, &valor, sizeof(uint32_t));
    paquete->buffer->stream = stream;

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete, logger);
    free(valor);
    free(offset);
}
void recibirOrden(int socket){
    int tamanio;
    void* stream = recibir_stream(&tamanio, socket);
    free (stream);
}

void enviarValor_uint32(uint32_t valor, int socket, op_code orden, t_log *logger)
{
    t_paquete * paquete= crear_paquete(orden, logger);
    paquete->buffer->size = sizeof(uint32_t);
    void* stream = malloc(paquete->buffer->size);
    int offset = 0;
    memcpy(stream + offset, &valor, sizeof(uint32_t));
    paquete->buffer->stream = stream;
    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete, logger);
}

uint32_t recibirValor_uint32(int socket, t_log *logger) {

    t_paquete *paquete = malloc(sizeof (t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    int tamanio;
    void* stream = recibir_stream(&tamanio, socket);
    uint32_t valor;
    memcpy(&valor, stream, sizeof(uint32_t));
    free(stream);
    eliminar_paquete(paquete, logger);

    return valor;
}


t_paquete* crear_paquete(op_code codigo, t_log* logger) //CREA BUFFER
{
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo;
    crear_buffer(paquete); // Le inyecta un buffer
    log_trace(logger, "Creo el paquete a enviar");
    return paquete;
}

void crear_buffer(t_paquete* paquete) //INYECTA BUFFER EN PAQUETE
{
    paquete->buffer = malloc(sizeof(t_buffer)); //reserva memoria para la struct t_buffer (devuelve un puntero)
    paquete->buffer->size = 0;			//inicializa el tamanio del payload del buffer en 0
    paquete->buffer->stream = NULL;		//el payload apunta a NULL de momento
}


void enviar_paquete(t_paquete* paquete, int socket_cliente){
    int bytes = paquete->buffer->size + sizeof(uint32_t) + sizeof(op_code);//Almacena en bytes, el tamanio del paquete: tamanio del payload(size) + tamanio de size de buffer + tamanio de op_code(ENUM) de paquete
    //POSIBLE FALLA
    void* enviar = serializar_paquete(paquete, bytes); //Dado un paquete y su tamanio lo serializa, devuelve un puntero al stream que se enviara finalmente.

    send(socket_cliente, enviar, bytes, 0); //Envia el stream

    free(enviar);			//Libera memoria dinámica
}

void* recibir_stream(int* size, uint32_t cliente_socket) { //En realidad devuelve el stream, no el t_buffer
    recv(cliente_socket, size, sizeof(int), MSG_WAITALL);
    void *buffer = malloc(*size);
    recv(cliente_socket, buffer, *size, MSG_WAITALL);
    return buffer;
}


void eliminar_paquete(t_paquete* paquete, t_log* logger) {
    if(ALL_DETAILS){
        log_info(logger, "SE LIBERA STREAM");
        free(paquete->buffer->stream);
        log_info(logger, "SE LIBERA BUFFER");
        free(paquete->buffer);
        log_info(logger, "SE LIBERA PAQUETE");
        free(paquete);
    }else{
        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);
    }


}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio) {
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
    memcpy(paquete->buffer->stream + paquete->buffer->size, valor, tamanio);
    paquete->buffer->size += tamanio;
}



int recibir_operacion(int socket_cliente)
{

    int cod_op;
    /*
    int flags = fcntl(socket_cliente, F_GETFL, 0);
    flags &= ~O_NONBLOCK;  // Desactivar el modo no bloqueante
    fcntl(socket_cliente, F_SETFL, flags);

    */
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}





bool enviarListaUint32_t(t_list* listaInts, int socket_cliente, t_log* logger, op_code codigo)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarUint32_tsAPaquete(listaInts, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarUint32_tsAPaquete(t_list* listaInts, t_paquete* paquete)
{

    paquete->buffer->size+= sizeof(uint32_t)*list_size(listaInts);

    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento

    //Sumo la cantidad de instrucciones al buffer
    paquete->buffer->size += sizeof(uint8_t);

    void copiarElementos(uint32_t unEntero){
        memcpy(stream + offset, &unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    int cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    list_iterate(listaInts,copiarElementos);
    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;

}





t_list* recibirListaUint32_t(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* listaInts = list_create();
    int cantidad_ints = 0;
    memcpy(&cantidad_ints, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_ints; ++i) {
        uint32_t nuevoEntero;
        memcpy(&nuevoEntero, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
    }
    free(buffer);
    return listaInts;

}


bool enviarDatos(void* datos, uint32_t tamanioDatos,op_code codigo, int socket_cliente, t_log* logger)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarDatosAPaquete(datos,tamanioDatos, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarDatosAPaquete(void* datos, uint32_t tamanioDatos, t_paquete* paquete)
{

    paquete->buffer->size+= tamanioDatos;

    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento

    //Sumo el numero que me dice el tamanio de los datos al buffer
    paquete->buffer->size += sizeof(uint32_t);

    memcpy(stream + offset, &tamanioDatos, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, datos, tamanioDatos);

    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;

}





void* recibirDatos(int socket_cliente, uint32_t tamanioDatos){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    void* datos;
    memcpy(&tamanioDatos, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    datos= malloc(tamanioDatos);
    memcpy(datos, buffer + desplazamiento, tamanioDatos);
    free(buffer);
    return datos;

}




/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////FUNCIONES PARTICULARES (NUEVAS)/////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

bool enviarListaInstrucciones(t_list* listaInstrucciones, int socket_cliente, t_log* logger)

{
    t_paquete* paquete = crear_paquete(GESTIONAR_CONSOLA_NUEVA, logger);
    if(!agregarInstruccionesAPaquete(listaInstrucciones, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarInstruccionesAPaquete(t_list* listaInstrucciones, t_paquete* paquete)
{


    void sumarTamaniosInstrucciones(t_instr* unaInstruccion){
        int id = sizeof (uint8_t);
        int idLength = unaInstruccion->idLength +1;
        int params = (sizeof (uint8_t) * 3);
        int param1Length = unaInstruccion->param1Length +1;
        int param2Length = unaInstruccion->param2Length +1;
        int param3Length = unaInstruccion->param3Length +1;
        paquete->buffer->size+= id + idLength + params + param1Length + param2Length + param3Length;
    }
    list_iterate(listaInstrucciones,sumarTamaniosInstrucciones);
    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento

    //Sumo la cantidad de instrucciones al buffer
    paquete->buffer->size += sizeof(uint8_t);

    void copiarElementos(t_instr *instruccion){ //Copia
        if(esInstruccionSinParametros(instruccion)){
            memcpy(stream + offset, &instruccion->idLength, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->id, instruccion->idLength + 1);
            offset += instruccion->idLength +1;
        }
        else if (esInstruccionConUnParametro(instruccion)){
            memcpy(stream + offset, &instruccion->idLength, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->id, instruccion->idLength + 1);
            offset += instruccion->idLength +1;
            memcpy(stream + offset, &instruccion->param1Length, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->param1, instruccion->param1Length + 1);
            offset += instruccion->param1Length +1;
        }
        else if(esInstruccionConDosParametros(instruccion)){
            memcpy(stream + offset, &instruccion->idLength, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->id, instruccion->idLength + 1);
            offset += instruccion->idLength +1;
            memcpy(stream + offset, &instruccion->param1Length, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->param1, instruccion->param1Length + 1);
            offset += instruccion->param1Length +1;
            memcpy(stream + offset, &instruccion->param2Length, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->param2, instruccion->param2Length + 1);
            offset += instruccion->param2Length +1;
        }
        else if(esInstruccionConTresParametros(instruccion)){
            memcpy(stream + offset, &instruccion->idLength, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->id, instruccion->idLength + 1);
            offset += instruccion->idLength +1;
            memcpy(stream + offset, &instruccion->param1Length, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->param1, instruccion->param1Length + 1);
            offset += instruccion->param1Length +1;
            memcpy(stream + offset, &instruccion->param2Length, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->param2, instruccion->param2Length + 1);
            offset += instruccion->param2Length +1;
            memcpy(stream + offset, &instruccion->param3Length, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            memcpy(stream + offset, instruccion->param3, instruccion->param3Length + 1);
            offset += instruccion->param3Length +1;
        }
        else {
            return false;
            printf("Es una instruccion invalida\n");
        }

    }
    int cantidad_instrucciones = list_size(listaInstrucciones);
    memcpy(stream + offset, &cantidad_instrucciones, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    list_iterate(listaInstrucciones,copiarElementos);
    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;

}





t_list* recibirListaInstrucciones(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* instrucciones = list_create();
    int cantidad_instrucciones = 0;
    memcpy(&cantidad_instrucciones, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_instrucciones; ++i) {
        t_instr *instruccion= malloc(sizeof(t_instr));

        memcpy(&instruccion->idLength, buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento+=sizeof(uint8_t);

        instruccion->id= malloc(instruccion->idLength +1);
        memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength +1);
        desplazamiento+= instruccion->idLength +1;

        if(esInstruccionConTresParametros(instruccion)){
            memcpy(&instruccion->param1Length, buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento+=sizeof(uint8_t);

            instruccion->param1= malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length +1);
            desplazamiento+= instruccion->param1Length + 1;

            memcpy(&instruccion->param2Length, buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento+=sizeof(uint8_t);

            instruccion->param2= malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length +1);
            desplazamiento+= instruccion->param2Length + 1;

            memcpy(&instruccion->param3Length, buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento+=sizeof(uint8_t);

            instruccion->param3= malloc(instruccion->param3Length + 1);
            memcpy(instruccion->param3, buffer + desplazamiento, instruccion->param3Length +1);
            desplazamiento+= instruccion->param3Length + 1;

        }
        if(esInstruccionConDosParametros(instruccion)){
            memcpy(&instruccion->param1Length, buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento+=sizeof(uint8_t);

            instruccion->param1= malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length +1);
            desplazamiento+= instruccion->param1Length + 1;

            memcpy(&instruccion->param2Length, buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento+=sizeof(uint8_t);

            instruccion->param2= malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length +1);
            desplazamiento+= instruccion->param2Length + 1;
        }
        if(esInstruccionConUnParametro(instruccion)){
            memcpy(&instruccion->param1Length, buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento+=sizeof(uint8_t);
            instruccion->param1= malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length +1);
            desplazamiento+= instruccion->param1Length + 1;
        }



        list_add(instrucciones, instruccion);
    }
    free(buffer);
    return instrucciones;

}


bool enviarString(char* string, int socket_cliente, op_code codigoOperacion, t_log* logger){
    t_paquete* paquete = crear_paquete(codigoOperacion, logger);
    if(!agregarStringAPaquete(string, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarStringAPaquete(char* string, t_paquete* paquete){

    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento
    uint8_t tamanio = strlen(string);
    memcpy(stream + offset, &tamanio, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, string, tamanio+1);
    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;
}
char* recibirString(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    uint8_t tamanioString = 0;
    char* string;
    memcpy(&tamanioString, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);
    string= malloc(tamanioString);
    memcpy(string, buffer + desplazamiento, tamanioString);
    free(buffer);
    return string;
}




uint32_t enviar_int_array(uint32_t *array, int conexion, op_code codigo, t_log* logger){
    //el array tiene que decir la cantidad de elementos en el slot 0 y desp los elementos
    t_paquete* paquete= crear_paquete(codigo, logger);
    agregarIntArrayAPaquete(paquete, array);
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete, logger);
}

void agregarIntArrayAPaquete(t_paquete* paquete, uint32_t *array){

    //calculamos tamaño buffer
    paquete->buffer->size += (sizeof(uint32_t) * (array[0] + 1));

    //armamos el stream de datos
    void* stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    //Array de ints, primero va en el stream el TAMAÑO, luego la lista en si
    uint32_t tamanioLista = array[0];
    memcpy(stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for(int i = 0; i < tamanioLista; i++){
        memcpy(stream + desplazamiento, &array[i+1], sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    paquete->buffer->stream= stream;
}

uint32_t * recibir_int_array(int conexion){
    //Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer = 0;
    uint32_t desplazamiento = 0;
    void* buffer = recibir_stream(&tamanioBuffer, conexion);
    //inicializo lo que necesito
    uint32_t arraySize =-1;

    memcpy(&arraySize, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    uint32_t* array = calloc(arraySize+1,sizeof (uint32_t));
    array[0] = arraySize;

    for(int i = 0; i < arraySize; i++){
        memcpy(&array[i+1], buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    free(buffer);


    return array;


}



void enviar_mensaje(char* mensaje, int socket_cliente, op_code codigoOperacion, t_log* logger)
{
    t_paquete* paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = codigoOperacion;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2*sizeof(int);

    void* a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete, logger);
}

char* recibir_mensaje(int socket_cliente)
{
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);

    return buffer;

}

void* recibir_buffer(int* size, int socket_cliente)
{
    void * buffer;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);

    return buffer;
}


////////////////////////////
bool enviarTablasSegmentos(t_list* tablasSegmentos, int socket_cliente, t_log* logger,op_code codigo)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarTablasAPaquete(tablasSegmentos, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las tablas de segmentos al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarTablasAPaquete(t_list* tablasSegmentos, t_paquete* paquete)
{
    void sumarTamaniosTablas(t_tablaSegmentos* tablaSegmentos){
        int pid = sizeof(uint32_t);
        int cantidadSegmentos = list_size(tablaSegmentos->segmentos) * 3*sizeof(uint32_t);
        paquete->buffer->size+= pid + cantidadSegmentos;
    }
    list_iterate(tablasSegmentos,sumarTamaniosTablas);
    //Sumo la cantidad de tablas al buffer
    paquete->buffer->size += sizeof(uint8_t);
    //Sumo la cantidad de segmentos al buffer
    paquete->buffer->size += sizeof(uint8_t);
    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento




    void copiarElementos(t_tablaSegmentos* unaTablaSegmentos){

            memcpy(stream + offset, &unaTablaSegmentos->pid, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            void copiarSegmentos(t_segmento* unSegmento){
                memcpy(stream + offset, &unSegmento->base, sizeof(uint32_t));
                offset += sizeof(uint32_t);
                memcpy(stream + offset, &unSegmento->limite, sizeof(uint32_t));
                offset += sizeof(uint32_t);
                memcpy(stream + offset, &unSegmento->id, sizeof(uint32_t));
                offset += sizeof(uint32_t);

            }
            int cantidad_segmentos = list_size(unaTablaSegmentos->segmentos);
            memcpy(stream + offset, &cantidad_segmentos, sizeof(uint8_t));
            offset += sizeof(uint8_t);
            list_iterate(unaTablaSegmentos->segmentos, copiarSegmentos);

    }
    int cantidad_tablas = list_size(tablasSegmentos);
    memcpy(stream + offset, &cantidad_tablas, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    list_iterate(tablasSegmentos,copiarElementos);
    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;

}





t_list* recibirTablasSegmentosInstrucciones(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* tablasSegmentos = list_create();
    int cantidad_tablas = 0;
    memcpy(&cantidad_tablas, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);
    for (int i = 0; i < cantidad_tablas; ++i) {
        t_tablaSegmentos* nuevaTabla= malloc(sizeof(t_tablaSegmentos));
        uint8_t cantidadSegmentos = 0;
        t_list* segmentos = list_create();

        memcpy(&nuevaTabla->pid, buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento+=sizeof(uint32_t);

        memcpy(&cantidadSegmentos, buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento+=sizeof(uint8_t);

        for (int j = 0; j <cantidadSegmentos ; ++j) {
            t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
            memcpy(&nuevoSegmento->base, buffer + desplazamiento, sizeof(uint32_t));
            desplazamiento+=sizeof(uint32_t);
            memcpy(&nuevoSegmento->limite, buffer + desplazamiento, sizeof(uint32_t));
            desplazamiento+=sizeof(uint32_t);
            memcpy(&nuevoSegmento->id, buffer + desplazamiento, sizeof(uint32_t));
            desplazamiento+=sizeof(uint32_t);
            list_add(segmentos, nuevoSegmento);
        }
        nuevaTabla->segmentos = segmentos;
        list_add(tablasSegmentos, nuevaTabla);
    }
    free(buffer);
    return tablasSegmentos;

}





bool enviarListaIntsYDatos(t_list* listaInts,t_datos* datos, int socket_cliente, t_log* logger, op_code codigo)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarIntsYDatosAPaquete(listaInts, datos, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarIntsYDatosAPaquete(t_list* listaInts, t_datos* datos, t_paquete* paquete)
{

    paquete->buffer->size+= sizeof(uint32_t)*list_size(listaInts);
    paquete->buffer->size+= datos->tamanio + sizeof(uint32_t);

    //Sumo la cantidad de enteros al buffer
    paquete->buffer->size += sizeof(uint8_t);
    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento

    void copiarElementos(uint32_t* unEntero){
        memcpy(stream + offset, unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    uint8_t cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    list_iterate(listaInts,copiarElementos);
    memcpy(stream + offset, &datos->tamanio, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(stream + offset, datos->datos, datos->tamanio);
    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;

}





t_list* recibirListaIntsYDatos(int socket_cliente,t_datos* datos)
{
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* listaInts = list_create();
    int cantidad_ints = 0;
    memcpy(&cantidad_ints, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_ints; ++i) {
        uint32_t* nuevoEntero = malloc(sizeof(uint32_t));
        memcpy(nuevoEntero, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
    }
    uint32_t* posicion = list_get(listaInts,0);
    uint32_t* pid = list_get(listaInts,1);
    memcpy(&datos->tamanio, buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento+=sizeof(uint32_t);
    datos->datos = malloc(datos->tamanio);
    memcpy(datos->datos, buffer + desplazamiento, datos->tamanio);

    free(buffer);
    return listaInts;

}





bool enviarParamsParaLecturaEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid, op_code codigoOperacion, t_log* logger, int socket_cliente){
    t_paquete* paquete = crear_paquete(codigoOperacion, logger);
    if(!agregarParamsParaLecturaEscrituraArchivoAPaquete(nombreArchivo, puntero, tamanio, direccionFisica, pid, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete, logger);
    return true;
}

bool agregarParamsParaLecturaEscrituraArchivoAPaquete(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid, t_paquete* paquete){


    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento
    uint8_t tamanioNombreArchivo = strlen(nombreArchivo) +1;
    memcpy(stream + offset, &tamanioNombreArchivo, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, nombreArchivo, tamanio);
    offset += tamanio;
    memcpy(stream + offset, &puntero, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &tamanio, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &direccionFisica, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);



    paquete->buffer->stream = stream;
    printf("SE AGREGO EL PAQUETE\n");
    return true;

}

void recibirParamsParaLecturaEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, uint32_t pid, int socket_cliente){
    int tamanioBuffer;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanioBuffer, socket_cliente);
    uint8_t tamanioNombreArchivo = 0;
    memcpy(&tamanioNombreArchivo, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);
    nombreArchivo= malloc(tamanioNombreArchivo);
    memcpy(nombreArchivo, buffer + desplazamiento, tamanioNombreArchivo);
    desplazamiento+= tamanioNombreArchivo;
    memcpy(&puntero, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    memcpy(&tamanio, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    memcpy(&direccionFisica, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    memcpy(&pid, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);
    free(buffer);

}




void agregar_instrucciones_a_paquete(t_paquete *paquete, t_list *instrucciones) {

    uint32_t cantidad_instrucciones = list_size(instrucciones);

    agregar_a_paquete(paquete, &cantidad_instrucciones, sizeof(uint32_t));

    for (int i = 0; i < cantidad_instrucciones; i++) {
        t_instr *instruccion = list_get(instrucciones, i);

        agregar_a_paquete(paquete, &(instruccion->cantidad_parametros), sizeof(uint8_t));
        agregar_a_paquete(paquete, &(instruccion->idLength), sizeof(uint8_t));
        agregar_a_paquete(paquete, instruccion->id, instruccion->idLength + 1);


        if(instruccion->cantidad_parametros == 1){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length+1);          }

        if(instruccion->cantidad_parametros == 2){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete(paquete, instruccion->param2, instruccion->param2Length+1);        }

        if(instruccion->cantidad_parametros == 3){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete(paquete, instruccion->param2, instruccion->param2Length+1);
            agregar_a_paquete(paquete,  &(instruccion->param3Length), sizeof(uint8_t));
            agregar_a_paquete(paquete, instruccion->param3, instruccion->param3Length+1);
            
            }

        

    }
}
//pcb->tablaSegmentos

void agregar_tablaSegmentos_a_paquete(t_paquete* paquete, t_tablaSegmentos* tablaSegmentos){
    uint32_t pid = tablaSegmentos->pid;
    agregar_a_paquete(paquete, &pid, sizeof(uint32_t));
    uint32_t cantidad_segmentos = list_size(tablaSegmentos->segmentos);

    agregar_a_paquete(paquete, &cantidad_segmentos, sizeof(uint32_t));

    for (int i = 0; i < cantidad_segmentos; i++) {
        t_segmento *segmen = list_get(tablaSegmentos->segmentos, i);
        agregar_a_paquete(paquete, &(segmen->id), sizeof(uint32_t));
        agregar_a_paquete(paquete, &(segmen->limite), sizeof(uint32_t));
        agregar_a_paquete(paquete, &(segmen->base), sizeof(uint32_t));
    }
}



void agregar_registros_a_paquete(t_paquete *paquete, registros_cpu *registro) {

    //Los valores a almacenar en los registros siempre tendrán la misma longitud que el registro,
    // es decir que si el registro es de 16 bytes siempre se escribirán 16 caracteres.

    uint32_t tamanioAx = 5;
    uint32_t tamanioEax = 9;
    uint32_t tamanioRax = 17;


    agregar_a_paquete(paquete, &(tamanioAx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(tamanioEax), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(tamanioRax), sizeof(uint32_t));

agregar_a_paquete(paquete, (registro->registro_AX), tamanioAx);
agregar_a_paquete(paquete, (registro->registro_BX), tamanioAx);
agregar_a_paquete(paquete, (registro->registro_CX), tamanioAx);
agregar_a_paquete(paquete, (registro->registro_DX), tamanioAx);

agregar_a_paquete(paquete, (registro->registro_EAX), tamanioEax);
agregar_a_paquete(paquete, (registro->registro_EBX), tamanioEax);
agregar_a_paquete(paquete, (registro->registro_ECX), tamanioEax);
agregar_a_paquete(paquete, (registro->registro_EDX), tamanioEax);


agregar_a_paquete(paquete, (registro->registro_RAX), tamanioRax);
agregar_a_paquete(paquete, (registro->registro_RBX), tamanioRax);
agregar_a_paquete(paquete, (registro->registro_RCX), tamanioRax);
agregar_a_paquete(paquete, (registro->registro_RDX), tamanioRax);

}


void agregar_PCB_a_paquete(t_paquete *paquete, t_pcb* pcb) {
    //PCB(UINTS_32T): ID, PROGRAMAM COUNTER, ESTIMACION RAFAGA, RAFAGA ANTERIOR, TIEMPO LLEGADA READY, TIEMPO ENVIO EXEC
    agregar_a_paquete(paquete, &(pcb->id), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->programCounter), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->estimacionRafaga), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->rafagaAnterior), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->tiempoLlegadaReady), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->tiempoEnvioExec), sizeof(uint32_t));
    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU, INSTRUCCIONES, TABLA SEGMENTOS
    agregar_registros_a_paquete(paquete, pcb->registrosCpu);
    agregar_instrucciones_a_paquete(paquete, pcb->instr);
    agregar_tablaSegmentos_a_paquete(paquete, pcb->tablaSegmentos);
}






/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////FUNCIONES PARTICULARES (ANTIGUO)///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void* serializar_paquete(t_paquete* paquete, int bytes)
{
    void * retornar = malloc(bytes); //Reserva espacio para el stream (void*)
    int desplazamiento = 0;

    //OP_CODE DE PAQUETE
    memcpy(retornar + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
    desplazamiento+= sizeof(op_code);

    //SIZE DE BUFFER
    memcpy(retornar + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
    desplazamiento+= sizeof(uint32_t);

    //BUFFER
    memcpy(retornar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    //desplazamiento+= paquete->buffer->size; CREO QUE NO VA

    return retornar; //retorna el puntero al stream
}

void enviar_paquete_pcb(t_pcb* pcbDelProceso, int conexion, op_code codigo, t_log* logger){ //USADA POR KERNEL PARA MANDAR PCB A CPU
    t_paquete* paquete= crear_paquete(codigo, logger);
    agregar_PCB_a_paquete(paquete,pcbDelProceso);
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete, logger);
}

t_pcb * recibir_pcb(int conexion) {
    //Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_stream(&tamanioBuffer, conexion);

    //Inicializo todas las estructuras que necesito
    t_pcb *unPcb = malloc(sizeof(t_pcb));
    registros_cpu *registros = malloc(sizeof(registros_cpu));
    t_list *instrucciones = list_create();
    t_tablaSegmentos* tablaSegmentos = malloc(sizeof (t_tablaSegmentos));
    t_list *segmentos = list_create();

    //Comienzo a consumir el buffer (EN ORDEN, MUY IMPORTANTE)
    //PCB(UINTS_32T): ID, PROGRAMAM COUNTER, ESTIMACION RAFAGA, RAFAGA ANTERIOR, TIEMPO LLEGADA READY, TIEMPO ENVIO EXEC

    //ID
    memcpy(&(unPcb->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //PROGRAM COUNTER
    memcpy(&(unPcb->programCounter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //ESTIMACION RAFAGA
    memcpy(&(unPcb->estimacionRafaga), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //RAFAGA ANTERIOR
    memcpy(&(unPcb->rafagaAnterior), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //TIEMPO LLEGADA READY
    memcpy(&(unPcb->tiempoLlegadaReady), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //TIEMPO ENVIO EXEC
    memcpy(&(unPcb->tiempoEnvioExec), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU, INSTRUCCIONES, TABLA SEGMENTOS

    //REGISTROS CPU (primero tiene los 3 tamaños fijos y luego los datos)

    uint32_t tamanioAx = 0;
    uint32_t tamanioEax = 0;
    uint32_t tamanioRax = 0;

    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&tamanioEax, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&tamanioRax, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;

    memcpy(&(registros->registro_EAX), buffer + desplazamiento, tamanioEax);
    desplazamiento += tamanioEax;
    memcpy(&(registros->registro_EBX), buffer + desplazamiento, tamanioEax);
    desplazamiento += tamanioEax;
    memcpy(&(registros->registro_ECX), buffer + desplazamiento, tamanioEax);
    desplazamiento += tamanioEax;
    memcpy(&(registros->registro_EDX), buffer + desplazamiento, tamanioEax);
    desplazamiento += tamanioEax;

    memcpy(&(registros->registro_RAX), buffer + desplazamiento, tamanioRax);
    desplazamiento += tamanioRax;
    memcpy(&(registros->registro_RBX), buffer + desplazamiento, tamanioRax);
    desplazamiento += tamanioRax;
    memcpy(&(registros->registro_RCX), buffer + desplazamiento, tamanioRax);
    desplazamiento += tamanioRax;
    memcpy(&(registros->registro_RDX), buffer + desplazamiento, tamanioRax);
    desplazamiento += tamanioRax;

    unPcb->registrosCpu = registros;

    //Instrucciones
    //Al ser una lista primero recibimos el tamanio y luego la lista
    uint32_t tamanioListaInstrucciones = 0;

    memcpy(&tamanioListaInstrucciones, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanioListaInstrucciones; i++) {
        t_instr *instruccion = malloc(sizeof(t_instr));

        //INSTRUCCION: ID LENGTH, ID, CANTIDAD_PARAMETROS, PARAM 1 LENGTH, PARAM 1, ...

        //CANTIDAD_PARAMETROS
        memcpy(&(instruccion->cantidad_parametros), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);

        //ID LENGTH
        memcpy(&(instruccion->idLength), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);

        //ID
        instruccion->id = malloc(instruccion->idLength + 1);
        memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength + 1);
        desplazamiento += instruccion->idLength + 1;



        if (instruccion->cantidad_parametros == 1) {
            //PARAM 1 LENGTH
            memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 1
            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length + 1;
        }

        if (instruccion->cantidad_parametros == 2) {
            //PARAM 1 LENGTH
            memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 1
            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length + 1;

            //PARAM 2 LENGTH
            memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 2
            instruccion->param2 = malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
            desplazamiento += instruccion->param2Length + 1;
        }

        if (instruccion->cantidad_parametros == 3) {
            //PARAM 1 LENGTH
            memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 1
            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length + 1;

            //PARAM 2 LENGTH
            memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 2
            instruccion->param2 = malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
            desplazamiento += instruccion->param2Length + 1;

            //PARAM 3 LENGTH
            memcpy(&(instruccion->param3Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 3
            instruccion->param3 = malloc(instruccion->param3Length + 1);
            memcpy(instruccion->param3, buffer + desplazamiento, instruccion->param3Length + 1);
            desplazamiento += instruccion->param3Length + 1;
        }
        list_add(instrucciones,instruccion);
    }
    unPcb->instr = instrucciones;

    //TABLA SEGMENTOS
    //PID DEL PROCESO LIGADO A LA TDS
    uint32_t pid = 0;

    memcpy(&pid, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Al ser una lista primero recibimos el tamanio

    uint32_t tamanioListaSegmentos = 0;

    memcpy(&tamanioListaSegmentos, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for(int i = 0 ; i<tamanioListaSegmentos; i++){
        t_segmento* segmento = malloc(sizeof(t_segmento));

        //El segmento esta compuesto por: Id,limite,base
        //ID
        memcpy(&(segmento->id), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //Limite
        memcpy(&(segmento->limite), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //Base
        memcpy(&(segmento->base), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(segmentos,segmento);
    }
    tablaSegmentos->segmentos = segmentos;
    tablaSegmentos->pid = unPcb->id;
    unPcb->tablaSegmentos = tablaSegmentos;

    return unPcb;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



t_pcb * recibir_pcb2(int socket_cliente) {

    int size;
	void * buffer = recibir_buffer(&size, socket_cliente);

    uint32_t desplazamiento = 0;
    t_pcb *unPcb = recibir_paquete_con_PCB(&desplazamiento, buffer);

	free(buffer);

 return unPcb;
}


t_pcb * recibir_paquete_con_PCB(uint32_t* desplazamiento, char* buffer) {

    t_pcb *unPcb = malloc(sizeof(t_pcb));
    t_list *instrucciones = list_create();
    t_list *segmentos = list_create();

    unPcb->id                 = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
    unPcb->programCounter     = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
    unPcb->estimacionRafaga   = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
    unPcb->rafagaAnterior     = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
    unPcb->tiempoLlegadaReady = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
    unPcb->tiempoEnvioExec    = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
  
    registros_cpu* registro = malloc(sizeof(registros_cpu));

    strcpy(registro->registro_AX, sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_BX, sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_CX,   sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_DX,   sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_EAX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_EBX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_ECX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_EDX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_RAX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_RBX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_RCX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));
    strcpy(registro->registro_RDX,  sacar_cadena_de_paquete(desplazamiento, buffer + *desplazamiento));

	unPcb->registrosCpu = registro;

    uint32_t  tamanio_de_lista = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
    for (int i = 0; i < tamanio_de_lista; i++) {

        t_instr *instruccion = malloc(sizeof(t_instr));

        instruccion->idLength = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

        instruccion->id = malloc(instruccion->idLength + 1);
        memcpy(instruccion->id, buffer + *desplazamiento, instruccion->idLength + 1);
        *desplazamiento = *desplazamiento + instruccion->idLength + 1;

        instruccion->cantidad_parametros= sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

        if (instruccion->cantidad_parametros == 1) {

        instruccion->param1Length = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + *desplazamiento, instruccion->param1Length + 1);
            *desplazamiento = *desplazamiento + instruccion->param1Length + 1;
        }

        if (instruccion->cantidad_parametros == 2) {

            instruccion->param1Length = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + *desplazamiento, instruccion->param1Length + 1);
            *desplazamiento = *desplazamiento +instruccion->param1Length + 1;

            instruccion->param2Length = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

            instruccion->param2 = malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + *desplazamiento, instruccion->param2Length + 1);
            *desplazamiento = *desplazamiento + instruccion->param2Length + 1;
        }

        if (instruccion->cantidad_parametros == 3) {

            instruccion->param1Length = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + *desplazamiento, instruccion->param1Length + 1);
            *desplazamiento = *desplazamiento + instruccion->param1Length + 1;

            instruccion->param2Length = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

            instruccion->param2 = malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + *desplazamiento, instruccion->param2Length + 1);
            *desplazamiento = *desplazamiento + instruccion->param2Length + 1;

            instruccion->param3Length = sacar_uint8_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

            instruccion->param3 = malloc(instruccion->param3Length + 1);
            memcpy(instruccion->param3, buffer + *desplazamiento, instruccion->param3Length + 1);
            *desplazamiento = *desplazamiento + instruccion->param3Length + 1;
        }

		    list_add(instrucciones, instruccion);

    }

	unPcb->instr =instrucciones;

        tamanio_de_lista = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
        for(int i = 0 ; i<tamanio_de_lista; i++){
            t_segmento* segmento = malloc(sizeof(t_segmento));

            segmento->id     = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
            segmento->base   = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;
            segmento->limite = sacar_uint32_t_de_paquete(desplazamiento, buffer + *desplazamiento) ;

          	list_add(segmentos, segmento);
        }

        	unPcb->tablaSegmentos =segmentos;

 return unPcb;
	
}

char* sacar_cadena_de_paquete( uint32_t*desplazamiento, char* buffer) {

    uint32_t tamanio;
    memcpy(&(tamanio), buffer , sizeof(uint32_t));
    *desplazamiento = *desplazamiento + sizeof(uint32_t);

	char* cadena = malloc(tamanio);
	memcpy(cadena, (buffer + sizeof(uint32_t) ), tamanio);
	*desplazamiento = *desplazamiento + tamanio;

	return cadena;
}

uint32_t  sacar_uint32_t_de_paquete( uint32_t*desplazamiento, char* buffer) {

    uint32_t entero;
    memcpy(&(entero), buffer , sizeof(uint32_t ));
    *desplazamiento = *desplazamiento + sizeof(uint32_t );

	return entero;
}

uint8_t  sacar_uint8_t_de_paquete( uint32_t*desplazamiento, char* buffer) {

    uint8_t entero;
    memcpy(&(entero), buffer , sizeof(uint8_t ));
    *desplazamiento = *desplazamiento + sizeof(uint8_t );

	return entero;
}



void agregar_PCB_a_paquete2(t_paquete *paquete,t_pcb* pcb) {  

    agregar_a_paquete(paquete, &(pcb->id),sizeof(uint32_t)); 
    agregar_a_paquete(paquete, &(pcb->programCounter),sizeof(uint32_t)); 
    agregar_a_paquete(paquete, &(pcb->estimacionRafaga),sizeof(uint32_t)); 
    agregar_a_paquete(paquete, &(pcb->rafagaAnterior),sizeof(uint32_t)); 
    agregar_a_paquete(paquete, &(pcb->tiempoLlegadaReady),sizeof(uint32_t)); 
    agregar_a_paquete(paquete, &(pcb->tiempoEnvioExec),sizeof(uint32_t)); 
    agregar_registros_a_paquete2(paquete,pcb->registrosCpu);
    agregar_instrucciones_a_paquete2(paquete, pcb->instr);
    agregar_segmentos_a_paquete2(paquete, pcb->tablaSegmentos);
}

void agregar_registros_a_paquete2(t_paquete* paquete,registros_cpu* registro) {

   agregar_registroPCB(registro->registro_AX,paquete);
   agregar_registroPCB(registro->registro_BX,paquete);
   agregar_registroPCB(registro->registro_CX,paquete);
   agregar_registroPCB(registro->registro_DX,paquete);
   agregar_registroPCB(registro->registro_EAX,paquete);
   agregar_registroPCB(registro->registro_EBX,paquete);
   agregar_registroPCB(registro->registro_ECX,paquete);
   agregar_registroPCB(registro->registro_EDX,paquete);
   agregar_registroPCB(registro->registro_RAX,paquete);
   agregar_registroPCB(registro->registro_RBX,paquete);
   agregar_registroPCB(registro->registro_RCX,paquete);
   agregar_registroPCB(registro->registro_RDX,paquete);
}

void agregar_registroPCB(char* nombre_registro,t_paquete* paquete){

            uint32_t tamanio = strlen(nombre_registro)+1;
            agregar_a_paquete(paquete, &(tamanio),sizeof(uint32_t)); 
            agregar_a_paquete(paquete,nombre_registro,tamanio); 
}

void agregar_instrucciones_a_paquete2(t_paquete* paquete, t_list* instrucciones ) {

       uint32_t tamanio11 = list_size(instrucciones);
       agregar_a_paquete(paquete, &(tamanio11),sizeof(uint32_t));

	for (int k = 0; k < tamanio11; k++) {

            t_instr* instruc = list_get(instrucciones,k);
            agregar_a_paquete(paquete, &(instruc->idLength),sizeof(uint8_t)); 
            agregar_a_paquete(paquete,instruc->id,instruc->idLength+1); 

            agregar_a_paquete(paquete, &(instruc->cantidad_parametros),sizeof(uint8_t)); 

        if(instruc->cantidad_parametros == 1){
        agregar_a_paquete(paquete, &(instruc->param1Length),sizeof(uint8_t)); 
        agregar_a_paquete(paquete,instruc->param1,instruc->param1Length + 1); 
        }
        if(instruc->cantidad_parametros == 2){
        agregar_a_paquete(paquete, &(instruc->param1Length),sizeof(uint8_t)); 
        agregar_a_paquete(paquete,instruc->param1,instruc->param1Length + 1); 

        agregar_a_paquete(paquete, &(instruc->param2Length),sizeof(uint8_t)); 
        agregar_a_paquete(paquete,instruc->param2,instruc->param2Length + 1); 
        }
        if(instruc->cantidad_parametros == 3){
        agregar_a_paquete(paquete, &(instruc->param1Length),sizeof(uint8_t)); 
        agregar_a_paquete(paquete,instruc->param1,instruc->param1Length + 1); 

        agregar_a_paquete(paquete, &(instruc->param2Length),sizeof(uint8_t)); 
        agregar_a_paquete(paquete,instruc->param2,instruc->param2Length + 1); 

        agregar_a_paquete(paquete, &(instruc->param3Length),sizeof(uint8_t)); 
        agregar_a_paquete(paquete,instruc->param3,instruc->param3Length + 1); 
        }
    }
}

void agregar_segmentos_a_paquete2(t_paquete* paquete, t_list* segmentos){

            uint32_t tamanio_lista_segmentos=list_size(segmentos);
            agregar_a_paquete(paquete, &(tamanio_lista_segmentos),sizeof(uint32_t));

            for (int i = 0; i < tamanio_lista_segmentos; i++) {

            t_segmento* seg = list_get(segmentos,i);
            agregar_a_paquete(paquete, &(seg->id),sizeof(uint32_t));            
            agregar_a_paquete(paquete, &(seg->base),sizeof(uint32_t));            // ver el orden de los demas 
            agregar_a_paquete(paquete, &(seg->limite),sizeof(uint32_t)); 
           }
}

void enviar_paquete_pcb2(t_pcb* pcb, int conexion, op_code codigo, t_log* info_logger){
    t_paquete* paquete= crear_paquete(codigo, info_logger);
    agregar_PCB_a_paquete2(paquete,pcb);
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete, info_logger);
}
