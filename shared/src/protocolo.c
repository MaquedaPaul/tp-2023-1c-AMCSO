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


    void sumarTamaniosInstrucciones(instr_t *unaInstruccion){
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

    void copiarElementos(instr_t *instruccion){ //Copia
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
        instr_t *instruccion= malloc(sizeof(instr_t));

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
bool enviarTablasSegmentos(t_list* tablasSegmentos, int socket_cliente, t_log* logger)

{
    t_paquete* paquete = crear_paquete(GESTIONAR_CONSOLA_NUEVA, logger);
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
        int cantidadSegmentos = list_size(tablaSegmentos->segmentos) * 2*sizeof(uint32_t);
        paquete->buffer->size+= pid + cantidadSegmentos;
    }
    list_iterate(tablasSegmentos,sumarTamaniosTablas);
    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento

    //Sumo la cantidad de tablas al buffer
    paquete->buffer->size += sizeof(uint8_t);

    void copiarElementos(t_tablaSegmentos* unaTablaSegmentos){

            memcpy(stream + offset, &unaTablaSegmentos->pid, sizeof(uint32_t));
            offset += sizeof(uint32_t);

            void copiarSegmentos(t_segmento* unSegmento){
                memcpy(stream + offset, &unSegmento->base, sizeof(uint32_t));
                offset += sizeof(uint32_t);
                memcpy(stream + offset, &unSegmento->limite, sizeof(uint32_t));
                offset += sizeof(uint32_t);

            }
            int cantidad_segmentos = list_size(unaTablaSegmentos->segmentos);
            memcpy(stream + offset, &cantidad_segmentos, sizeof(uint8_t));
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
            list_add(segmentos, nuevoSegmento);
        }

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


    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento

    //Sumo la cantidad de enteros al buffer
    paquete->buffer->size += sizeof(uint8_t);

    void copiarElementos(uint32_t unEntero){
        memcpy(stream + offset, &unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    int cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    list_iterate(listaInts,copiarElementos);
    memcpy(stream + offset, &datos->tamanio, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(stream + offset, &datos->datos, datos->tamanio);
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
        uint32_t nuevoEntero;
        memcpy(&nuevoEntero, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
    }

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

    int cantidad_instrucciones = list_size(instrucciones);

    agregar_a_paquete(paquete, &cantidad_instrucciones, sizeof(int));

    for (int i = 0; i < cantidad_instrucciones; i++) {
        t_instr *instruccion = list_get(instrucciones, i);

        agregar_a_paquete(paquete, &(instruccion->idLength), sizeof(int));
        agregar_a_paquete(paquete, instruccion->id, instruccion->idLength);

        agregar_a_paquete(paquete, &(instruccion->cantidad_parametros), sizeof(int));

        if(instruccion->cantidad_parametros == 1){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(int));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length);          }

        if(instruccion->cantidad_parametros == 2){

            agregar_a_paquete(paquete, &(instruccion->param1Length), sizeof(int));
            agregar_a_paquete(paquete, instruccion->param1, instruccion->param1Length);
            agregar_a_paquete(paquete,  &(instruccion->param2Length), sizeof(int));
            agregar_a_paquete(paquete, instruccion->param2, instruccion->param2Length);        }

    }
}


void agregar_segmentos_a_paquete(t_paquete *paquete, t_list *segmentos) {

    int cantidad_segmentos = list_size(segmentos);

    agregar_a_paquete(paquete, &cantidad_segmentos, sizeof(int));

    for (int i = 0; i < cantidad_segmentos; i++) {
        t_segmento *segmen = list_get(segmentos, i);
        agregar_a_paquete(paquete, &(segmen->id), sizeof(u_int32_t));
        agregar_a_paquete(paquete, &(segmen->limite), sizeof(u_int32_t));
        agregar_a_paquete(paquete, &(segmen->base), sizeof(u_int32_t));
    }
}

void agregar_registros_a_paquete(t_paquete *paquete, registros_cpu *registro) {

int tamanioAX = strlen(registro->registroAX) + 1;
agregar_a_paquete(paquete, &tamanioAX, sizeof(int));
agregar_a_paquete(paquete, (registro->registroAX), tamanioAX);

int tamanioBX = strlen(registro->registroBX) + 1;
agregar_a_paquete(paquete, &(tamanioBX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroBX), tamanioBX);

int tamanioCX = strlen(registro->registroCX) + 1;
agregar_a_paquete(paquete, &(tamanioCX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroCX), tamanioCX);

int tamanioDX = strlen(registro->registroDX) + 1;
agregar_a_paquete(paquete, &(tamanioDX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroDX), tamanioDX);

int tamanioEAX = strlen(registro->registroEAX) + 1;
agregar_a_paquete(paquete, &tamanioEAX, sizeof(int));
agregar_a_paquete(paquete, (registro->registroEAX), tamanioEAX);

int tamanioEBX = strlen(registro->registroEBX) + 1;
agregar_a_paquete(paquete, &(tamanioEBX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroEBX), tamanioEBX);

int tamanioECX = strlen(registro->registroECX) + 1;
agregar_a_paquete(paquete, &(tamanioECX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroECX), tamanioECX);

int tamanioEDX = strlen(registro->registroEDX) + 1;
agregar_a_paquete(paquete, &(tamanioEDX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroEDX), tamanioEDX);

int tamanioRAX = strlen(registro->registroRAX) + 1;
agregar_a_paquete(paquete, &tamanioRAX, sizeof(int));
agregar_a_paquete(paquete, (registro->registroRAX), tamanioRAX);

int tamanioRBX = strlen(registro->registroRBX) + 1;
agregar_a_paquete(paquete, &(tamanioRBX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroRBX), tamanioRBX);

int tamanioRCX = strlen(registro->registroRCX) + 1;
agregar_a_paquete(paquete, &(tamanioRCX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroRCX), tamanioRCX);

int tamanioRDX = strlen(registro->registroRDX) + 1;
agregar_a_paquete(paquete, &(tamanioRDX), sizeof(int));
agregar_a_paquete(paquete, (registro->registroRDX), tamanioRDX);

}


void agregar_PCB_a_paquete(t_paquete *paquete, t_pcb* pcb) {
    agregar_a_paquete(paquete, &(pcb->id), sizeof(u_int32_t));
    agregar_a_paquete(paquete, &(pcb->programCounter), sizeof(u_int32_t));
    agregar_instrucciones_a_paquete(paquete, pcb->instr);
    agregar_segmentos_a_paquete(paquete, pcb->tablaSegmentos);
    agregar_registros_a_paquete(paquete, pcb->registrosCpu);
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





