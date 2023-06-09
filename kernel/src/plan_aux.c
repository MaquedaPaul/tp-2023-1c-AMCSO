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
  pcb->tablaSegmentos = NULL;
  //pcb->tablaArchivosAbiertos = list_create();


  return pcb;
}

registros_cpu* crearRegistroCPU() {
    registros_cpu* puntero_registros = malloc(sizeof(registros_cpu)); // Asignar memoria para la estructura

    // Inicializar los registros con valores predeterminados
    strcpy(puntero_registros->registro_AX, "0000");
    strcpy(puntero_registros->registro_BX, "0000");
    strcpy(puntero_registros->registro_CX, "0000");
    strcpy(puntero_registros->registro_DX, "0000");
    strcpy(puntero_registros->registro_EAX, "00000000");
    strcpy(puntero_registros->registro_EBX, "00000000");
    strcpy(puntero_registros->registro_ECX, "00000000");
    strcpy(puntero_registros->registro_EDX, "00000000");
    strcpy(puntero_registros->registro_RAX, "0000000000000000");
    strcpy(puntero_registros->registro_RCX, "0000000000000000");
    strcpy(puntero_registros->registro_RBX, "0000000000000000");
    strcpy(puntero_registros->registro_RDX, "0000000000000000");

    return puntero_registros;
}



//Actualizar pcb

void actualizarTiempoRafaga(t_pcb* pcbEjecutado){

    time_t t_actual = time(NULL) ;
    double t_envioExec = pcbEjecutado->tiempoEnvioExec;
    
    pcbEjecutado->rafagaAnterior = t_actual - t_envioExec;
    
}
