#include <utils/test_serializacion.h>


t_package* crear_a_john(){
    t_package *p1 = malloc(sizeof (t_package));
    p1->username= "John Doe";
    p1->message ="Hola, soy John Doe.";
    p1->message_long= strlen(p1->message)+1;
    p1->username_long = strlen(p1->username)+1;
    return p1;
}


t_buffer* crearBuffer_para_john(t_package* john){
    t_buffer* buffer = malloc(sizeof(t_buffer));

    buffer->size =  john->username_long  +john->message_long + 2*sizeof(uint32_t);
    // La longitud del string nombre. Le sumamos 1
    // para enviar tambien el caracter centinela '\0'.
    // Esto se podría obviar, pero entonces
    // deberíamos agregar el centinela en el receptor.
    return buffer;
}

void* crearStream_para_john(t_buffer* buffer){
    void* stream = malloc(buffer->size);
    return stream;
}

void serializar_a_john(t_package* john, t_buffer* buffer){
    void*stream = buffer->stream;
    int offset = 0; // Desplazamiento
// Para el nombre primero mandamos el tamaño y luego el texto en sí:
    memcpy(stream + offset, &john->username_long, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, john->username, john->username_long);
    offset += john->username_long;

    memcpy(stream + offset, &john->message_long, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, john->message, john->message_long);

// No tiene sentido seguir calculando el desplazamiento, ya ocupamos el buffer completo
    buffer->stream = stream;
}



t_paquete* crearUnPaqueteConCodOp(op_code cod, t_buffer* buffer){
    t_paquete* paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = cod; // Podemos usar una constante por operación
    paquete->buffer = buffer; // Nuestro buffer de antes.
    return paquete;
}

void* crearStreamAEnviar_para_john(t_paquete* paqueteJohn){
    t_buffer* buffer = paqueteJohn->buffer;
    void* a_enviar = malloc(buffer->size + sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paqueteJohn->codigo_operacion), sizeof(op_code));

    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(paqueteJohn->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paqueteJohn->buffer->stream, paqueteJohn->buffer->size);
    return a_enviar;
}

void enviarAJohnA(int cliente_socket,t_paquete* paqueteJohn, void* a_enviar){
    t_buffer* buffer = paqueteJohn->buffer;
    send(cliente_socket, a_enviar, buffer->size + sizeof(op_code) + sizeof(uint32_t), 0);
}
void enviarUnJohn(op_code cod, int cliente_socket,t_log* logger){
    t_package *john = crear_a_john();
    t_buffer* buffer = crearBuffer_para_john(john);
    void* unStream = crearStream_para_john(buffer);
    buffer->stream = unStream;
    serializar_a_john(john, buffer);
    t_paquete* paqueteJohn = crearUnPaqueteConCodOp(cod, buffer);
    void* a_enviar = crearStreamAEnviar_para_john(paqueteJohn);
    enviarAJohnA(cliente_socket, paqueteJohn,a_enviar);
    liberarAJohn(john, paqueteJohn, logger);
    free(a_enviar);
}
int liberarAJohn(t_package* john,t_paquete* paqueteJohn, t_log* logger){
    //free(john->username);
    //free(john->message);
    free(john);
    eliminar_paquete(paqueteJohn, logger);
    return true;
}




t_paquete* johnHaArribadoCaballeros(int cliente_socket){
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    recv(cliente_socket, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(cliente_socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);
    return paquete;
}
t_package* deserializar_a_john(t_paquete* paquete, t_log* logger){
    t_package *p1 = malloc(sizeof(t_package));
    void *stream = paquete->buffer->stream;
    memcpy(&(p1->username_long), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    p1->username = malloc(p1->username_long);
    memcpy(p1->username, stream, p1->username_long);
    stream += p1->username_long;

    memcpy(&(p1->message_long), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    p1->message = malloc(p1->message_long);
    memcpy(p1->message, stream, p1->message_long);
    eliminar_paquete(paquete, logger);
    //free(stream);
    return p1;
}

t_package* recibirAJohn(int cliente_socket,t_log* logger){
    t_paquete* paquete = johnHaArribadoCaballeros(cliente_socket);
    return deserializar_a_john(paquete, logger);
}

void loguearAJohn(t_log* logger, t_package* john){
    log_info(logger,"%s",john->username);
    log_info(logger,"%d",john->username_long);
    log_info(logger,"%s",john->message);
    log_info(logger,"%d",john->message_long);
}
