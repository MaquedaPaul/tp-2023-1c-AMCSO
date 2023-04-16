//
// Created by utnso on 4/7/23.
//

#include <init.h>

char* path_pseudo;
t_list* inst_list;



bool cargar_configuracion(char *path)
{
    file_cfg_console = config_create(path);
    cfg_console->IP_KERNEL = strdup(config_get_string_value(file_cfg_console, "IP_KERNEL"));
    log_trace(trace_logger, "IP Cargada Correctamente: %s", cfg_console->IP_KERNEL);
    cfg_console->PUERTO_KERNEL = strdup(config_get_string_value(file_cfg_console, "PUERTO_KERNEL"));
    log_trace(trace_logger, "Puerto Cargado Correctamente: %s", cfg_console->PUERTO_KERNEL);
    log_trace(trace_logger, "Archivo de configuracion cargado correctamente");
    config_destroy(file_cfg_console);
    return true;
}

t_list * crear_lista_de_instrucciones(char *path) {
    FILE* file = fopen(path, "r");
    struct stat stat_file;
    stat(path, &stat_file);
    if (file == NULL) {
        return NULL;
    }
    char* buffer = calloc(1, stat_file.st_size + 1);
    fread(buffer, stat_file.st_size, 1, file);
    for (int i = 1; !feof(file); i++) {
        fread(buffer, stat_file.st_size, 1, file);
    }

    char **inst_per_line = string_split(buffer, "\n"); //divido el archivo leido por lineas


    inst_list = list_create();    //creo la lista de instrucciones


    string_iterate_lines(inst_per_line, closure_instrucciones);
    //string_array_destroy(inst_per_line);
    free(buffer);
    //free(inst_per_line);
    fclose(file);

    return inst_list;
}


void closure_instrucciones(char *line) {
    char** inst_and_param = string_split(line, " ");    //divido la linea por " " y me crea un vector y me lo mete segun las divisiones
    instr_t *single_inst = malloc(sizeof(instr_t));     //reservo memoria para estructura instr_t
    single_inst->idLength= strlen(inst_and_param[0]);
    single_inst->id= malloc(single_inst->idLength + 1);
    memcpy(&single_inst->id,&inst_and_param[0],single_inst->idLength +1);        //copio el id de la instruccion
    //single_inst->nroDeParam= tamanioArray(inst_and_param)-1;     //guardo la cantidad de parametros

    if(esInstruccionSinParametros(single_inst)){
        single_inst->param1Length = 0;
        single_inst->param1 = NULL;
        single_inst->param2Length = 0;
        single_inst->param2 = NULL;
        single_inst->param3Length = 0;
        single_inst->param3 = NULL;
        list_add(inst_list, single_inst); //agrego la instruccion a la lista
    }
    else if (esInstruccionConUnParametro(single_inst)){
        single_inst->param1Length = strlen(inst_and_param[1]);
        single_inst->param1 = malloc(single_inst->param1Length + 1);
        memcpy(&single_inst->param1,&inst_and_param[1],single_inst->param1Length +1);
        single_inst->param2Length =0;
        single_inst->param2 = NULL;
        single_inst->param3Length =0;
        single_inst->param3 = NULL;
        list_add(inst_list, single_inst); //agrego la instruccion a la lista
    }
    else if(esInstruccionConDosParametros(single_inst)){
        single_inst->param1Length = strlen(inst_and_param[1]);
        single_inst->param1 = malloc(single_inst->param1Length + 1);
        memcpy(&single_inst->param1,&inst_and_param[1],single_inst->param1Length +1);

        single_inst->param2Length =strlen(inst_and_param[2]);
        single_inst->param2 = malloc(single_inst->param2Length + 1);
        memcpy(&single_inst->param2,&inst_and_param[2],single_inst->param2Length +1);

        single_inst->param3Length =0;
        single_inst->param3 = NULL;
        list_add(inst_list, single_inst); //agrego la instruccion a la lista
    }
    else{
        single_inst->param1Length = strlen(inst_and_param[1]);
        single_inst->param1 = malloc(single_inst->param1Length + 1);
        memcpy(&single_inst->param1,&inst_and_param[1],single_inst->param1Length +1);

        single_inst->param2Length =strlen(inst_and_param[2]);
        single_inst->param2 = malloc(single_inst->param2Length + 1);
        memcpy(&single_inst->param2,&inst_and_param[2],single_inst->param2Length +1);

        single_inst->param3Length =strlen(inst_and_param[3]);
        single_inst->param3 = malloc(single_inst->param3Length + 1);
        memcpy(&single_inst->param3,&inst_and_param[3],single_inst->param3Length +1);

        list_add(inst_list, single_inst); //agrego la instruccion a la lista
    }

    //string_array_destroy(inst_and_param);
}


bool esInstruccionSinParametros(instr_t* instruccion){
    return (strcmp(instruccion->id, "EXIT") == 0) || (strcmp(instruccion->id, "YIELD") == 0);
}

bool esInstruccionConUnParametro(instr_t* instruccion){
    bool io = (strcmp(instruccion->id, "I/O") == 0);
    bool delete_segment = (strcmp(instruccion->id, "DELETE_SEGMENT") == 0);
    bool f_open = (strcmp(instruccion->id, "F_OPEN") == 0);
    bool f_close = (strcmp(instruccion->id, "F_CLOSE") == 0);
    bool wait = (strcmp(instruccion->id, "WAIT") == 0);
    bool signal = (strcmp(instruccion->id, "SIGNAL") == 0);

    return  io || delete_segment || f_open || f_close || wait || signal;
}


bool esInstruccionConDosParametros(instr_t* instruccion){

    bool set = (strcmp(instruccion->id, "SET") == 0);
    bool mov_out = (strcmp(instruccion->id, "MOV_OUT") == 0);
    bool mov_in = (strcmp(instruccion->id, "MOV_IN") == 0);
    bool f_truncate = (strcmp(instruccion->id, "F_TRUNCATE") == 0);
    bool f_seek = (strcmp(instruccion->id, "F_SEEK") == 0);
    bool create_segment = (strcmp(instruccion->id, "CREATE_SEGMENT") == 0);
    return set || mov_out || mov_in || f_truncate || f_seek || create_segment;
}

bool esInstruccionConTresParametros(instr_t* instruccion){
    return (strcmp(instruccion->id, "F_WRITE") == 0) || (strcmp(instruccion->id, "F_READ") == 0);
}
