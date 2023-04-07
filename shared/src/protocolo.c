#include "utils/protocolo.h"
bool ALL_DETAILS = false;

//!! CUIDADO PORQUE RECIBE UINT32_T Y MUCHOS METODOS PASAN INT COMO SOCKET, Y AL REVES EL TAMAÑO
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////FUNCIONES GENERICAS//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void enviarOrden(op_code orden, int socket, t_log *logger) {
    t_paquete * paquete= crear_paquete(orden);
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
    t_paquete * paquete= crear_paquete(orden);
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


t_paquete* crear_paquete(op_code codigo) //CREA BUFFER
{
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo;
    crear_buffer(paquete); // Le inyecta un buffer
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
    void *buffer;
    recv(cliente_socket, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
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

/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////FUNCIONES PARTICULARES///////////////////////////////////
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



t_proceso* recibir_paquete(int socket_cliente)
{
	int tamanio;
	int desplazamiento = 0;
	void *buffer = recibir_stream(&tamanio, socket_cliente);
	t_proceso *proceso= malloc(sizeof(t_proceso));
	t_list* valores = list_create();
    t_list* tamanios_segmentos = list_create();

    //PRIMERO RECIBO CANTIDAD DE INSTRUCCIONES (INT)
    memcpy(&proceso->cantidad_instrucciones, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    //SEGUNDO RECIBO INSTRUCCIONES
    for (int i = 0; i < &proceso->cantidad_instrucciones; ++i) {
        instr_t *instruccion= malloc(sizeof(instr_t));

        memcpy(&instruccion->idLength, buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento+=sizeof(uint8_t);

        instruccion->id= malloc(instruccion->idLength +1);
        memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength +1);
        desplazamiento+= instruccion->idLength +1;

        if(strcmp(instruccion->id, "EXIT") == 0){
            list_add(valores, instruccion);
            break;
        }
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

        list_add(valores, instruccion);
    }
	proceso->instrucciones= valores;

    //TERCERO RECIBO CANTIDAD DE SEGMENTOS (INT)
    memcpy(&proceso->cantidad_segmentos, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    //CUARTO RECIBO TAMANIOS DE SEGMENTOS
    for (int i = 0; i < proceso->cantidad_segmentos; ++i) {
        uint16_t* tamanio_segmento = malloc(sizeof(uint16_t));
        memcpy(tamanio_segmento, buffer + desplazamiento, sizeof(uint16_t));
        desplazamiento+=sizeof(uint16_t);
        list_add(tamanios_segmentos, tamanio_segmento);
    }
     proceso->tam_segmentos = tamanios_segmentos;

	free(buffer);
	return proceso;
}

//{SANTI}
//AGREGO METODOS DE SERIALIZACION PARA CPU, HAY ALGUNOS REPETIDOS EN COMUNICACION.H DE CONSOLA, CONVIENE PASARLOS ACA

void enviar_paquete_pcb(pcb* pcbDelProceso, int conexion, op_code codigo, t_log* logger){ //USADA POR KERNEL PARA MANDAR PCB A CPU
	t_paquete* paquete= crear_paquete(codigo);
	agregarPcbAPaquete(paquete, pcbDelProceso);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete, logger);
}

void agregarPcbAPaquete(t_paquete* paquete, pcb* pcb){
	//Memoria para variables simples
    paquete->buffer->size += (sizeof(uint32_t) * 4) + (sizeof(registros_cpu)); //+ sizeof (tabla_segmento);
	
	//Memoria para Instrucciones
	void calcularTamanioDeInstruccion(instr_t * instruccion){
        if(strcmp(instruccion->id, "EXIT") == 0){
            paquete->buffer->size += sizeof(uint8_t)
                    + instruccion->idLength +1;
        }else{
            paquete->buffer->size += (sizeof(uint8_t) *3 )
                    + instruccion->idLength +1
            + instruccion->param1Length +1
            + instruccion->param2Length +1;
        }
	}

    void calcularTamanioDeTablaSegmento(segmento* seg){
        paquete->buffer->size += sizeof(segmento);
        //paquete->buffer->size += sizeof(uint16_t) + sizeof(uint32_t);
    }


	list_iterate(pcb->instr,calcularTamanioDeInstruccion);//Calcula el de todas las instrucciones
    list_iterate(pcb->tablaSegmentos,calcularTamanioDeTablaSegmento);//Calcula el de todos los segmentos
	
	//Memoria para Registros
	//paquete->buffer->size += sizeof(uint32_t)*4;

	//Creo stream para el buffer, el size ya lo tengo actualizado
	
    void* stream = malloc(paquete->buffer->size); 
	int desplazamiento = 0; 

	//PCB: ID, LIST_INSTRUCCIONES, REGISTROS_CPU, PROGRAM_COUNTER, TABLA_SEGMENTO
	//ID	
	memcpy(stream + desplazamiento, &(pcb->id), sizeof(uint32_t)); 
	desplazamiento += sizeof(uint32_t);
	
	//LIST INSTRUCCIONES, primero va en el stream el TAMAÑO, luego la lista en si
	uint32_t tamanioLista = list_size(pcb->instr);
	memcpy(stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    void copiarElementos(instr_t *instruccion){ //Copia
        if(strcmp(instruccion->id, "EXIT") == 0){
            memcpy(stream + desplazamiento, &instruccion->idLength, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);
            memcpy(stream + desplazamiento, instruccion->id, instruccion->idLength + 1);
            desplazamiento += instruccion->idLength +1;
        }
        else{
            memcpy(stream + desplazamiento, &instruccion->idLength, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);
            memcpy(stream + desplazamiento, instruccion->id, instruccion->idLength + 1);
            desplazamiento += instruccion->idLength +1; // se va a 12 y tendria que tener 8
            memcpy(stream + desplazamiento, &instruccion->param1Length, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t); //suma 4 bien 16
            memcpy(stream + desplazamiento, instruccion->param1, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length +1; //suma 3 bien 19
            memcpy(stream + desplazamiento, &instruccion->param2Length, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t); //suma 4 bien 23
            memcpy(stream + desplazamiento, instruccion->param2, instruccion->param2Length + 1);
            desplazamiento += instruccion->param2Length +1;
        }
    }

    //LIST SEGMENTO, primero va en el stream el TAMAÑO, luego la lista en si
    uint32_t tamanioListaSegmento = list_size(pcb->tablaSegmentos);
    memcpy(stream + desplazamiento, &tamanioListaSegmento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    void closure_copiarTablaSegmento(segmento* seg){
        memcpy(stream + desplazamiento, &seg->tamanioSegmento, sizeof(uint16_t));
        desplazamiento += sizeof(uint16_t);
        memcpy(stream + desplazamiento, &seg->indiceTablaPaginas, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }
	
	list_iterate(pcb->instr,copiarElementos);
    list_iterate(pcb->tablaSegmentos,closure_copiarTablaSegmento);

	//REGISTROS_CPU
    //La serializacion viene va: ID-INST1-INST2-INST3...-
    //Ahora para los registros y tabla de segmento, para no serializar otro struct, los cargo a los valores indepenedientes, y cuando recibo la pcb, al deserializar los recreo
    //ID-#LIST-(#ID1-ID1-#P11-P11-#P21-P21-INST1)-...-(#IDn-IDn-#P1n-P1n-#P2n-P2n-INSTn)-{AX-BX-CX-DX}-PC-{NS-TS-ITP}
    
    memcpy(stream + desplazamiento, &(pcb->registrosCpu->AX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(stream + desplazamiento, &(pcb->registrosCpu->BX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(stream + desplazamiento, &(pcb->registrosCpu->CX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(stream + desplazamiento, &(pcb->registrosCpu->DX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

	//PROGRAM_COUNTER
	memcpy(stream + desplazamiento, &(pcb->programCounter), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	
	paquete->buffer->stream= stream;
    }

pcb* recibir_pcb(int conexion){
	//Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void* buffer = recibir_stream(&tamanioBuffer, conexion);
	
    //Inicializo todas las estructuras que necesito
    pcb* unPcb = malloc(sizeof(pcb)); //+sizeof(registros)+sizeof(tabla) pcb o hay que hacer la suma de todos incluyen structs?
	t_list* instrucciones = list_create();
    t_list* segmentos = list_create();
    unPcb->tablaSegmentos= segmentos;
    registros_cpu* registros = malloc(sizeof(registros_cpu));
    uint32_t tamanioLista;
    uint32_t tamanioListaSegmento;

    //En el buffer: ID-#LIST-(#ID1-ID1-#P11-P11-#P21-P21-INST1)-...-(#IDn-IDn-#P1n-P1n-#P2n-P2n-INSTn)-{AX-BX-CX-DX}-PC-{NS-TS-ITP}
    //ID
	memcpy(&(unPcb->id), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	
    //TAMAÑO_LISTA
    memcpy(&tamanioLista, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);


    //TAMAÑO_LISTA SEGMENTO
    memcpy(&tamanioListaSegmento, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	
    //LISTA_INSTRUCCIONES
	for(int i = 0; i < tamanioLista; i++){
		
        instr_t* instruccion = malloc(sizeof(instr_t));
		//Tamaño id 
        memcpy(&(instruccion->idLength), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);
		//Id
        instruccion->id = malloc(instruccion->idLength +1);
		memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength +1);
		desplazamiento += instruccion->idLength +1;
        if(strcmp(instruccion->id, "EXIT") == 0){
            list_add(instrucciones, instruccion);
            break;
        }
		//Tamaño parametro 1 + parametro 1
        memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);
        instruccion->param1 = malloc(instruccion->param1Length +1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length +1);
		desplazamiento += instruccion->param1Length +1;
        //Tamaño parametro 2 + parametro 2
		memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);
        instruccion->param2 = malloc(instruccion->param2Length +1);
		memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length +1);
		desplazamiento += instruccion->param2Length +1;
        
        //Cargo en la lista cada una
		list_add(instrucciones, instruccion);
	}
	unPcb->instr = instrucciones;



    //LISTA DE SEGMENTOS
    for(int i = 0; i < tamanioListaSegmento; i++){
        segmento* seg = malloc(sizeof (segmento));
        //Tamanio de Segmento
        memcpy(&(seg->tamanioSegmento), buffer + desplazamiento, sizeof(uint16_t));
        desplazamiento += sizeof(uint16_t);
        //Indice tabla de paginas
        memcpy(&(seg->indiceTablaPaginas), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        list_add(unPcb->tablaSegmentos, seg);
    }

    //REGISTROS CPU
    memcpy(&(registros->AX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->BX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->CX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->DX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //PROGRAM COUNTER
	memcpy(&(unPcb->programCounter), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	
    

    //Agrego los registro y tabla de segmentos a la pcb
    unPcb->registrosCpu = registros;

    //unPcb->tablaSegmento = tabla;
	//free(buffer);
    //free(registros);
    //free(tabla);
	return unPcb;
    }


void enviar_paquete_pcbPf(pcb_page_fault* pcbPfDelProceso, int conexion, op_code codigo, t_log* logger){
    t_paquete* paquete= crear_paquete(codigo);
    agregarPcbPfAPaquete(paquete, pcbPfDelProceso);
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete, logger);
}

void agregarPcbPfAPaquete(t_paquete* paquete, pcb_page_fault* pcbPfDelProceso) {
    //Memoria para variables simples
    paquete->buffer->size += (sizeof(uint32_t) * 4) + (sizeof(registros_cpu));

    //Memoria para Instrucciones
    void calcularTamanioDeInstruccion(instr_t *instruccion) {
        if (strcmp(instruccion->id, "EXIT") == 0) {
            paquete->buffer->size += sizeof(uint8_t)
                                     + instruccion->idLength + 1;
        } else {
            paquete->buffer->size += (sizeof(uint8_t) * 3)
                                     + instruccion->idLength + 1
                                     + instruccion->param1Length + 1
                                     + instruccion->param2Length + 1;
        }
    }

    void calcularTamanioDeTablaSegmento(segmento *seg) {
        paquete->buffer->size += sizeof(segmento);
        //paquete->buffer->size += sizeof(uint16_t) + sizeof(uint32_t);
    }


    list_iterate(pcbPfDelProceso->pcb->instr, calcularTamanioDeInstruccion);//Calcula el de todas las instrucciones
    list_iterate(pcbPfDelProceso->pcb->tablaSegmentos,
                 calcularTamanioDeTablaSegmento);//Calcula el de todos los segmentos

    //Memoria para Registros
    //paquete->buffer->size += sizeof(uint32_t)*4;

    //Creo stream para el buffer, el size ya lo tengo actualizado

    void *stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    //PCB: ID, LIST_INSTRUCCIONES, REGISTROS_CPU, PROGRAM_COUNTER, TABLA_SEGMENTO
    //ID
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pcb-> id), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //LIST INSTRUCCIONES, primero va en el stream el TAMAÑO, luego la lista en si
    uint32_t tamanioLista = list_size(pcbPfDelProceso->pcb-> instr);
    memcpy(stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    void copiarElementos(instr_t *instruccion) { //Copia
        if (strcmp(instruccion->id, "EXIT") == 0) {
            memcpy(stream + desplazamiento, &instruccion->idLength, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);
            memcpy(stream + desplazamiento, instruccion->id, instruccion->idLength + 1);
            desplazamiento += instruccion->idLength + 1;
        } else {
            memcpy(stream + desplazamiento, &instruccion->idLength, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);
            memcpy(stream + desplazamiento, instruccion->id, instruccion->idLength + 1);
            desplazamiento += instruccion->idLength + 1; // se va a 12 y tendria que tener 8
            memcpy(stream + desplazamiento, &instruccion->param1Length, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t); //suma 4 bien 16
            memcpy(stream + desplazamiento, instruccion->param1, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length + 1; //suma 3 bien 19
            memcpy(stream + desplazamiento, &instruccion->param2Length, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t); //suma 4 bien 23
            memcpy(stream + desplazamiento, instruccion->param2, instruccion->param2Length + 1);
            desplazamiento += instruccion->param2Length + 1;
        }
    }

    //LIST SEGMENTO, primero va en el stream el TAMAÑO, luego la lista en si
    uint32_t tamanioListaSegmento = list_size(pcbPfDelProceso->pcb-> tablaSegmentos);
    memcpy(stream + desplazamiento, &tamanioListaSegmento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    void closure_copiarTablaSegmento(segmento *seg) {
        memcpy(stream + desplazamiento, &seg->tamanioSegmento, sizeof(uint16_t));
        desplazamiento += sizeof(uint16_t);
        memcpy(stream + desplazamiento, &seg->indiceTablaPaginas, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    list_iterate(pcbPfDelProceso->pcb-> instr, copiarElementos);
    list_iterate(pcbPfDelProceso->pcb-> tablaSegmentos, closure_copiarTablaSegmento);

    //REGISTROS_CPU
    //La serializacion viene va: ID-INST1-INST2-INST3...-
    //Ahora para los registros y tabla de segmento, para no serializar otro struct, los cargo a los valores indepenedientes, y cuando recibo la pcb, al deserializar los recreo
    //ID-#LIST-(#ID1-ID1-#P11-P11-#P21-P21-INST1)-...-(#IDn-IDn-#P1n-P1n-#P2n-P2n-INSTn)-{AX-BX-CX-DX}-PC-{NS-TS-ITP}

    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pcb-> registrosCpu->AX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pcb-> registrosCpu->BX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pcb-> registrosCpu->CX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pcb-> registrosCpu->DX), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //PROGRAM_COUNTER
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pcb->programCounter), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Segmento
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->segmento), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Pagina
    memcpy(stream + desplazamiento, &(pcbPfDelProceso->pagina), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    paquete->buffer->stream= stream;
}

pcb_page_fault* recibir_pcbPf(int conexion){
    //Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void* buffer = recibir_stream(&tamanioBuffer, conexion);

    //Inicializo todas las estructuras que necesito
    pcb_page_fault* unPcbPf = malloc(sizeof(pcb_page_fault)); //+sizeof(registros)+sizeof(tabla) pcb o hay que hacer la suma de todos incluyen structs?
    pcb* unPcb = malloc(sizeof (pcb));
    t_list* instrucciones = list_create();
    t_list* segmentos = list_create();
    //unPcbPf->pcb->tablaSegmentos= segmentos;
    registros_cpu* registros = malloc(sizeof(registros_cpu));
    uint32_t tamanioLista;
    uint32_t tamanioListaSegmento;

    //En el buffer: ID-#LIST-(#ID1-ID1-#P11-P11-#P21-P21-INST1)-...-(#IDn-IDn-#P1n-P1n-#P2n-P2n-INSTn)-{AX-BX-CX-DX}-PC-{NS-TS-ITP}
    //ID
    memcpy(&(unPcb->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //TAMAÑO_LISTA
    memcpy(&tamanioLista, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);


    //TAMAÑO_LISTA SEGMENTO
    memcpy(&tamanioListaSegmento, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //LISTA_INSTRUCCIONES
    for(int i = 0; i < tamanioLista; i++){

        instr_t* instruccion = malloc(sizeof(instr_t));
        //Tamaño id
        memcpy(&(instruccion->idLength), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);
        //Id
        instruccion->id = malloc(instruccion->idLength +1);
        memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength +1);
        desplazamiento += instruccion->idLength +1;
        if(strcmp(instruccion->id, "EXIT") == 0){
            list_add(instrucciones, instruccion);
            break;
        }
        //Tamaño parametro 1 + parametro 1
        memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);
        instruccion->param1 = malloc(instruccion->param1Length +1);
        memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length +1);
        desplazamiento += instruccion->param1Length +1;
        //Tamaño parametro 2 + parametro 2
        memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);
        instruccion->param2 = malloc(instruccion->param2Length +1);
        memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length +1);
        desplazamiento += instruccion->param2Length +1;

        //Cargo en la lista cada una
        list_add(instrucciones, instruccion);
    }
    unPcb->instr = instrucciones;



    //LISTA DE SEGMENTOS
    for(int i = 0; i < tamanioListaSegmento; i++){
        segmento* seg = malloc(sizeof (segmento));
        //Tamanio de Segmento
        memcpy(&(seg->tamanioSegmento), buffer + desplazamiento, sizeof(uint16_t));
        desplazamiento += sizeof(uint16_t);
        //Indice tabla de paginas
        memcpy(&(seg->indiceTablaPaginas), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        list_add(segmentos, seg);
    }
   unPcb->tablaSegmentos = segmentos;

    //REGISTROS CPU
    memcpy(&(registros->AX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->BX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->CX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(registros->DX), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //PROGRAM COUNTER
    memcpy(&(unPcb->programCounter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Segmento
    memcpy(&(unPcbPf->segmento), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Pagina
    memcpy(&(unPcbPf->pagina), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);



    //Agrego los registro y tabla de segmentos a la pcb
    unPcb->registrosCpu = registros;

    unPcbPf->pcb = unPcb;
    //unPcb->tablaSegmento = tabla;
    //free(buffer);
    //free(registros);
    //free(tabla);
    return unPcbPf;
}



//ARRAY DE INTS

uint32_t enviar_int_array(uint32_t *array, int conexion, op_code codigo, t_log* logger){
    //el array tiene que decir la cantidad de elementos en el slot 0 y desp los elementos
    t_paquete* paquete= crear_paquete(codigo);
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

void enviar_pantalla_teclado(char* registro,int conexion, op_code codigo, t_log* logger){
    t_paquete * paquete = crear_paquete(codigo);
    agregarRegistroAPaquete(paquete,registro);
    enviar_paquete(paquete,conexion);
    eliminar_paquete(paquete,logger_kernel);
}

void agregarRegistroAPaquete(t_paquete* paquete, char* registro){
    //calculamos tamaño buffer
    paquete->buffer->size = strlen(registro)+1;

    //armamos el stream de datos
    void* stream = malloc(paquete->buffer->size);

    memcpy(stream,registro,strlen(registro)+1);

    paquete->buffer->stream = stream;
}

char* recibir_pantalla_teclado(int conexion){
    int tamanioBuffer = 0;
    void* buffer = recibir_stream(&tamanioBuffer,conexion);

    char* registro = malloc(tamanioBuffer);
    memcpy(registro,buffer,tamanioBuffer);
    return registro;
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