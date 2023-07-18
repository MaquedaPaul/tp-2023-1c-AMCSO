# Obtener la ubicación del script
script_dir=$(dirname "$0")

# Directorio relativo al script
directorio=$(realpath "$0")
directorioEjecutable=$(dirname "$directorio")
directorioModulo=$(dirname "$directorioEjecutable")
directorioTp=$(dirname "$directorioModulo")


test="$1"
testNro="$2"

directorioConfig="$directorioTp/pruebas/configs/$test"
directorioInstrucciones="$directorioTp/pruebas/instrucciones/$test/$1_$testNro"
directorioBin="$directorioModulo/bin/"


# Cambiar al directorio
cd "$directorioBin"
# Ejecutar el programa con parámetros determinados
./console.out "$directorioConfig/console.config" "$directorioInstrucciones"