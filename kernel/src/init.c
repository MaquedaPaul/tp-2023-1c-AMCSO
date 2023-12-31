//
// Created by utnso on 4/7/23.
//

#include <init.h>

bool recursosCargados = false;
bool estadosCargados = false;
bool semaforosCargados = false;
bool tablasFsCargadas = false;
bool semaforoDinamicoCargado = false;


//COLAS
t_queue* colaNew;
t_list* colaExec;
t_queue* colaExit;
t_list* colaBloq;
t_list* estadoBlockRecursos;
t_queue* colaReady_FIFO; //en caso de FIFO
t_list* colaReady; //en caso de HRRN


//TABLA GLOBAL ARCHIVOS ABIERTOS
t_list* tablaGlobal_ArchivosAbiertos; //Es una lista de t_archivoPeticion
t_list* listaPeticionesArchivos; //Es una lista de t_archivoPeticion. Aca se encolan los procesos que esperan por un archivo
pthread_mutex_t mutex_TGAA;


//CONTADORES Y MUTEX
int procesosTotales_MP;
int idProcesoGlobal;
int contadorPeticionesFs;
pthread_mutex_t mutex_colaNew;
pthread_mutex_t mutex_ColaReady; 
pthread_mutex_t mutex_colaExec;
pthread_mutex_t mutex_colaBloq;
pthread_mutex_t mutex_colaExit;
pthread_mutex_t mutex_MP;
pthread_mutex_t mutex_listaPeticionesArchivos;
pthread_mutex_t mutex_debug_logger;
pthread_mutex_t mutex_contadorPeticionesFs;

pthread_mutex_t m_pcb;

//SEMAFOROS
sem_t sem_procesosEnNew;
sem_t sem_procesosReady;
sem_t sem_procesosExit;
sem_t sem_cpuLibre;
sem_t sem_atenderCompactacion;


//HILOS
 pthread_t conexion_con_consola;
 pthread_t conexion_con_cpu;
 pthread_t conexion_con_memoria;
 pthread_t conexion_con_filesystem;
 pthread_t hilo_planificador_LP;
 pthread_t hilo_planificador_corto;
 pthread_t hilo_liberador_procesos;

//Manejo de recursos
t_list* listaRecursos;
pthread_mutex_t* semaforos_io;

int cargar_configuracion(char *path) {

    cfg_kernel->IP_MEMORIA = strdup(config_get_string_value(file_cfg_kernel, "IP_MEMORIA"));
    log_trace(trace_logger, "IP_MEMORIA Cargada Correctamente: %s", cfg_kernel->IP_MEMORIA);

    cfg_kernel->PUERTO_MEMORIA = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_MEMORIA"));
    log_trace(trace_logger, "PUERTO_MEMORIA Cargada Correctamente: %s", cfg_kernel->PUERTO_MEMORIA);

    cfg_kernel->IP_FILESYSTEM = strdup(config_get_string_value(file_cfg_kernel, "IP_FILESYSTEM"));
    log_trace(trace_logger, "IP_FILESYSTEM Cargada Correctamente: %s", cfg_kernel->IP_FILESYSTEM);

    cfg_kernel->PUERTO_FILESYSTEM = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_FILESYSTEM"));
    log_trace(trace_logger, "PUERTO_FILESYSTEM Cargada Correctamente: %s", cfg_kernel->PUERTO_FILESYSTEM);

    cfg_kernel->IP_CPU = strdup(config_get_string_value(file_cfg_kernel, "IP_CPU"));
    log_trace(trace_logger, "IP_CPU Cargada Correctamente: %s", cfg_kernel->IP_CPU);

    cfg_kernel->PUERTO_CPU = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_CPU"));
    log_trace(trace_logger, "PUERTO_CPU Cargada Correctamente: %s", cfg_kernel->PUERTO_CPU);

    cfg_kernel->PUERTO_ESCUCHA = strdup(config_get_string_value(file_cfg_kernel, "PUERTO_ESCUCHA"));
    log_trace(trace_logger, "PUERTO_ESCUCHA Cargada Correctamente: %s", cfg_kernel->PUERTO_ESCUCHA);

    cfg_kernel->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(file_cfg_kernel, "ALGORITMO_PLANIFICACION"));
    log_trace(trace_logger, "ALGORITMO_PLANIFICACION Cargada Correctamente: %s", cfg_kernel->ALGORITMO_PLANIFICACION);

    cfg_kernel->ESTIMACION_INICIAL = config_get_int_value(file_cfg_kernel, "ESTIMACION_INICIAL");
    log_trace(trace_logger, "ESTIMACION_INICIAL Cargada Correctamente: %d",
             cfg_kernel->ESTIMACION_INICIAL);

    cfg_kernel->HRRN_ALFA = config_get_double_value(file_cfg_kernel, "HRRN_ALFA");
    log_trace(trace_logger, "HRRN_ALFA Cargada Correctamente: %f",
             cfg_kernel->HRRN_ALFA);


    cfg_kernel->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(file_cfg_kernel, "GRADO_MAX_MULTIPROGRAMACION");
    log_trace(trace_logger, "GRADO_MAX_MULTIPROGRAMACION Cargada Correctamente: %d",
             cfg_kernel->GRADO_MAX_MULTIPROGRAMACION);


    cfg_kernel->RECURSOS = config_get_array_value(file_cfg_kernel, "RECURSOS");
    //log_trace(logger_kernel, "DISPOSITIVOS_IO Cargada Correctamente: %s", cfg_kernel->DISPOSITIVOS_IO);

    cfg_kernel->INSTANCIAS_RECURSOS = config_get_array_value(file_cfg_kernel, "INSTANCIAS_RECURSOS");
    //log_trace(logger_kernel, "TIEMPOS_IO Cargada Correctamente: %s", cfg_kernel->TIEMPOS_IO);


    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");


    int dim = tamanioArray(cfg_kernel->RECURSOS);
    semaforos_io = calloc(dim,sizeof(pthread_mutex_t));
    iniciarSemaforoDinamico(semaforos_io, dim);
    semaforoDinamicoCargado = true;
    cargarRecursos();

    return true;
}

void inicializar_kernel(){

//COLAS
    colaNew = queue_create();
    colaReady = list_create();
    colaExec = list_create();
    colaBloq = list_create();
    colaExit = queue_create();

    estadosCargados = true;

//TABLA GLOBAL ARCHIVOS ABIERTOS
    tablaGlobal_ArchivosAbiertos = list_create();
    listaPeticionesArchivos = list_create();

    tablasFsCargadas = true;

    pthread_mutex_init(&mutex_TGAA, NULL);//Mutex de la tabla global de archivos abiertos
    pthread_mutex_init(&mutex_listaPeticionesArchivos,NULL);
    pthread_mutex_init(&m_pcb,NULL);

//CONTADORES Y MUTEX

    procesosTotales_MP = 0;
    idProcesoGlobal = 0;
    contadorPeticionesFs = 0;

    pthread_mutex_init(&mutex_colaNew, NULL);
    pthread_mutex_init(&mutex_ColaReady, NULL);
    pthread_mutex_init(&mutex_colaExec, NULL);
    pthread_mutex_init(&mutex_colaBloq, NULL);
    pthread_mutex_init(&mutex_colaExit, NULL);
    pthread_mutex_init(&mutex_MP, NULL);
    pthread_mutex_init(&mutex_debug_logger, NULL);
    pthread_mutex_init(&mutex_contadorPeticionesFs, NULL);
    //SEMAFOROS
    sem_init(&sem_procesosEnNew,0,0);
    sem_init(&sem_procesosReady,0,0);
    sem_init(&sem_procesosExit,0,0);
    sem_init(&sem_cpuLibre,0,1);
    sem_init(&sem_atenderCompactacion,0,0);

    semaforosCargados = true;

 //HILOS
    pthread_create(&conexion_con_consola, NULL,(void*)crearServidor, NULL);
    pthread_create(&conexion_con_cpu, NULL, (void*)conectarConCPU, NULL);
    pthread_create(&conexion_con_memoria, NULL, (void*)conectarConMemoria, NULL);
    pthread_create(&conexion_con_filesystem, NULL, (void*)conectarConFileSystem, NULL);
    pthread_create(&hilo_planificador_LP, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_create(&hilo_planificador_corto, NULL, (void*)planificador_corto_plazo, NULL);
    pthread_create(&hilo_liberador_procesos, NULL, (void*)liberar_procesos, NULL);

    pthread_join(conexion_con_consola, NULL);
    pthread_join(conexion_con_cpu, NULL);
    pthread_join(conexion_con_memoria, NULL);
    pthread_join(conexion_con_filesystem, NULL);
    pthread_join(hilo_planificador_LP, NULL);
    pthread_join(hilo_planificador_corto,NULL);
    pthread_join(hilo_liberador_procesos,NULL);


}
int tamanioArray(char** array){
    int n=0;
    for(int i=0 ;*(array+i)!= NULL; i++)
        n++;
    return n;
}

void iniciarSemaforoDinamico(pthread_mutex_t* semaforo, int dim){
    for (int i = 0; i <dim ; ++i) {
       // sem_init(&semaforo[i],0,0);
        pthread_mutex_init(&semaforos_io[i],NULL);
    }
}

int cargarRecursos(){
    listaRecursos = list_create();
    int dim = tamanioArray(cfg_kernel->RECURSOS);
    char** recursos = cfg_kernel->RECURSOS;
    char** instancias = cfg_kernel->INSTANCIAS_RECURSOS;
    for(int i = 0 ; i < dim ; i++){
        t_recurso* recurso = malloc(sizeof (t_recurso));
        recurso->nombreRecurso = recursos[i];
        recurso->instanciasRecurso = atoi(instancias[i]);
        recurso->indiceSemaforo = i;
        recurso->cola = list_create();
        list_add(listaRecursos,recurso);
    }
    estadoBlockRecursos = listaRecursos;
    recursosCargados = true;
    return true;
}

void cerrar_programa() {

    if(recursosCargados){
        liberarRecursosKernel();
    }
    if(estadosCargados){
        liberarEstadosKernel();
    }
    if(semaforosCargados){
        liberarSemaforos();
    }
    if(tablasFsCargadas){
        liberarManejoFs();
    }
    if(semaforoDinamicoCargado){
        liberarSemaforoDinamico();
    }

    if(configCreado){
        destruirConfig();
    }

    if(cfgCreado){
        destruirCfg();
    }

    cortar_conexiones();
    cerrar_servers();

    if(logsCreados){
        destruirLoggers();
    }

    printf("\nCierre exitoso\n");
}

