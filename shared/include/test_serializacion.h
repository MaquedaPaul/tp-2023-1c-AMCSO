#ifndef SHARED_TEST_SERIALIZACION_H
#define SHARED_TEST_SERIALIZACION_H
#include <utils/protocolo.h>
typedef struct t_Package {
    char* username;
    uint32_t username_long;
    char* message;
    uint32_t message_long;
} t_package;

t_package* crear_a_john();
t_buffer* crearBuffer_para_john(t_package* john);
void* crearStream_para_john(t_buffer* buffer);
void serializar_a_john(t_package* john, t_buffer* buffer);
t_paquete* crearUnPaqueteConCodOp(op_code cod, t_buffer* buffer);
void* crearStreamAEnviar_para_john(t_paquete* paqueteJohn);
void enviarAJohnA(int cliente_socket,t_paquete* paqueteJohn, void* a_enviar);
void enviarUnJohn(op_code cod, int cliente_socket, t_log* logger);
int liberarAJohn(t_package* john,t_paquete* paqueteJohn, t_log* logger);



t_paquete* johnHaArribadoCaballeros(int cliente_socket);
t_package* deserializar_a_john(t_paquete* paquete,t_log* logger);
t_package* recibirAJohn(int cliente_socket, t_log*logger);
void loguearAJohn(t_log* logger, t_package* john);

#endif //SHARED_TEST_SERIALIZACION_H
