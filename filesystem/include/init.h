//
// Created by utnso on 4/7/23.
//

#ifndef TPSO_INIT_H
#define TPSO_INIT_H
#include <loggers_configs.h>
#include <comunicacion.h>
#include <gestion_filesystem.h>
#include <fcntl.h>
#include <commons/bitarray.h>
#include <sys/stat.h>

typedef enum {
    PATH_SUPERBLOQUE,
    PATH_BITMAP,
    PATH_BLOQUES,
    PATH_FCB
}tipo_path;

extern t_list* lista_bloques;
extern pthread_mutex_t mutex_cliente_socket;
extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_config* file_cfg_filesystem;
extern t_config_filesystem *cfg_filesystem;
extern t_config_superbloque* cfg_superbloque;
extern t_bloques* archivoBloques;
extern char* path_config;
int cargar_configuracion(char *path);
extern t_bitarray* bitarrayBitmapDeBloques;
extern t_list* lista_FCBs;
extern t_bitarray* bitmap;

extern pthread_mutex_t mutex_ArchivosUsados;
void reasignarPathSiEsNecesario(tipo_path tipo);
bool crearSemaforos();

bool levantarSuperbloque();
bool levantarBitmapBloques();
bool levantarArchivoBloques();

bool crearBitmapBloques();
bool crearSuperbloque();
bool crearArchivoDeBloques();
bool recorrerDirectorioFcb();

int existe_archivoFCB(char *nombre_archivo) ;
bool crearEstructuras();
bool existeArchivo(int* fd, char *path);

void crear_fcbs_del_directorio() ;

int obtener_tamanio_en_bytes();
void inicializarBitmap(int tamanio_del_bitarray);
bool iniciarFilesystem();
int esDirectorio(const char* nombre);
bool crear_bitmap_de_bloques();
bool crear_archivo_de_bloques();
bool levantarArchivosExistentes();
bool iniciarEstructurasAdministrativas(char* nombre_path);
#endif //TEMPLATE_INIT_H
