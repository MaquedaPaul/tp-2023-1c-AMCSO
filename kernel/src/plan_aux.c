#include <plan_aux.h>


void eliminarElementoLista(t_pcb* pcbBuscado, t_list *listaX){

  t_pcb *pcbAux;
  for (int i = 0; i < list_size(listaX); i++)
  {
     pcbAux = list_get(listaX, i);
    if(pcbBuscado->id == pcbAux->id){
      list_remove(listaX, i);
    }
  }
}

// ::::::::::::::::::::PCB:::::::::::::::::::::


t_pcb* crearPcb(t_list* listaInstrucciones)
{
  t_pcb *pcb = malloc(sizeof(t_pcb));
  pcb->id = idProcesoGlobal++;
  pcb->programCounter = 0;
  pcb->rafagaAnterior = 0;
  pcb->estimacionRafaga = cfg_kernel->ESTIMACION_INICIAL;
  pcb->tiempoLlegadaReady = 0;
  pcb->tiempoEnvioExec = 0;
  pcb->registrosCpu = crearRegistroCPU();
  pcb->instr = listaInstrucciones;
  pcb->tablaSegmentos = list_create();
  //pcb->tablaArchivosAbiertos = list_create();


  return pcb;
}

//TODO Revisar esta funcion
registros_cpu *crearRegistroCPU (){

    registros_cpu *reg = malloc(sizeof(registros_cpu));
    /*
    reg->registro_AX = "0000";
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
*/
    return reg;
}


//Actualizar pcb

void actualizarTiempoRafaga(t_pcb* pcbEjecutado){

    time_t t_actual = time(NULL) ;
    double t_envioExec = pcbEjecutado->tiempoEnvioExec;
    
    pcbEjecutado->rafagaAnterior = t_actual - t_envioExec;
    
}
