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
    else if (esInstruccionConTresParametros(single_inst)){
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
    else{
        printf("Es una instruccion invalida\n");
    }

    //string_array_destroy(inst_and_param);
}


