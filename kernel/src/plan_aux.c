#include <plan_aux.h>


void eliminarElementoLista(pcb *pcbBuscado, t_list *listaX){

  pcb *pcbAux;
  int aux;
  for (size_t i = 0; i < list_size(listaX); i++)
  {
    pcbAux = list_get(listaX, i);
    if(pcbBuscado->pid == pcbAux->pid){
      aux=i;
      i=(list_size(listaX)+1);
      list_remove(listaX, i);
    }

  }
}

// ::::::::::::::::::::PCB:::::::::::::::::::::


pcb *crear_pcb(int socketCliente)
{
  pcb *pcb = malloc(sizeof(pcb));
  pcb->pid = idProcesoGlobal++;
  pcb->instr = recibirListaInstrucciones( socketCliente);
  pcb->programCounter = 0;
  pcb->rafagaAnterior = 0;
  pcb->estimacionRafaga = cfg_kernel->ESTIMACION_INICIAL;
  pcb->tiempoLlegadaReady = 0;
  pcb->tiempoEnvioExec = 0;
  pcb->registrosCpu = crearRegistroCPU();
  //pcb->socketConsola = socketCliente;
  pcb->tablaSegmentos = list_create();
  pcb->tablaArchivosAbiertos = list_create();

  list_destroy(listaInstrucciones);
  list_destroy(listaSegmentos);

  return pcb;
}


registros_cpu *crearRegistroCPU (){

    registros_cpu *reg = malloc(sizeof(registros_cpu));
    reg->AX = 0;
    reg->BX = 0;
    reg->CX = 0;
    reg->DX = 0;
    reg->EAX = 0;
    reg->EBX = 0;
    reg->ECX = 0;
    reg->EDX = 0;
    reg->RAX = 0;
    reg->RBX = 0;
    reg->RCX = 0;
    reg->RDX = 0;

    return reg;
}


//Actualizar pcb

void actualizarTiempoUltimaRafaga(pcb* pcbEjecutado){
    int pid = pcbEjecutado->pid;

    time_t t_actual = time(NULL) ;
    double t_envioExec = pcbEjecutado->tiempoEnvioExec;
    
    pcbEjecutado->rafagaAnterior = t_actual - t_envioExec;
    
}