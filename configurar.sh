# Obtener la ubicación del script
script_dir=$(dirname "$0")

# Directorio relativo al script
directorio=$(realpath "$0")
directorioEjecutable=$(dirname "$directorio")

directorioTp="$directorioEjecutable"
directorioConfigs="$directorioTp/pruebas/configs"
directorioBASE="$directorioConfigs/BASE"
directorioERROR="$directorioConfigs/ERROR"
directorioFS="$directorioConfigs/FS"
directorioMEMORIA="$directorioConfigs/MEMORIA"
directorioDEADLOCK="$directorioConfigs/DEADLOCK"

cons="console.config"
ker="kernel.config"
cpu="cpu.config"
fs="fs.config"
mem="memory.config"



# Función que se ejecutará cuando se interrumpa el script
function limpiar_y_salir {
    echo "Saliendo del script..."
    exit 0
}

# Asignar la función "limpiar_y_salir" para manejar la interrupción del script
trap limpiar_y_salir SIGINT

# Comprobar si se pasaron dos parámetros

modulo=$1
ip=$2
puerto=$3

echo "Si algo esta mal escrito, no hinchemos, lo hice en la madrugada"

function reemplazo {
  local ip=$1
  local key=$2
  local config=$3
  sed -i "s/^$key=.*/$key=$ip/" "$directorioBASE/$config.config"
  echo "Actualizado en BASE"
  sed -i "s/^$key=.*/$key=$ip/" "$directorioMEMORIA/$config.config"
  echo "Actualizado en MEMORIA"
  sed -i "s/^$key=.*/$key=$ip/" "$directorioFS/$config.config"
  echo "Actualizado en FS"
  sed -i "s/^$key=.*/$key=$ip/" "$directorioDEADLOCK/$config.config"
  echo "Actualizado en DEADLOCK"
  sed -i "s/^$key=.*/$key=$ip/" "$directorioERROR/$config.config"
  echo "Actualizado en ERROR"

}



function kernelIp {
    local ip=$1
    echo "La IP que se cambiara sera $ip en console.config"
    reemplazo "$ip" "IP_KERNEL" "console"
}

function cpuIp() {
      local ip=$1
      echo "La IP que se cambiara sera $ip en kernel.config"
      reemplazo "$ip" "IP_CPU" "kernel"
}

function filesystemIp() {
      local ip=$1
      echo "La IP que se cambiara sera $ip en kernel.config"
      reemplazo "$ip" "IP_FILESYSTEM" "kernel"
}
function memoryIp() {
      local ip=$1
      echo "La IP que se cambiara sera $ip en kernel.config"
      reemplazo "$ip" "IP_MEMORIA" "kernel"
      reemplazo "$ip" "IP_MEMORIA" "cpu"
      reemplazo "$ip" "IP_MEMORIA" "filesystem"
}

function kernelPuerto {
    local puerto=$1
    echo "El puerto que se cambiara sera $puerto en console.config y kernel.config"
    reemplazo "$puerto" "PUERTO_KERNEL" "console"
    reemplazo "$puerto" "PUERTO_ESCUCHA" "kernel"
}
function cpuPuerto {
    local puerto=$1
    echo "El puerto que se cambiara sera $puerto en kernel.config y cpu.config"
    reemplazo "$puerto" "PUERTO_CPU" "kernel"
    reemplazo "$puerto" "PUERTO_ESCUCHA" "cpu"
}
function filesystemPuerto {
    local puerto=$1
    echo "El puerto que se cambiara sera $puerto en kernel.config y filesystem.config"
    reemplazo "$puerto" "PUERTO_FILESYSTEM" "kernel"
    reemplazo "$puerto" "PUERTO_ESCUCHA" "filesystem"
}
function memoryPuerto {
    local puerto=$1
    echo "El puerto que se cambiara sera $puerto en kernel.config, cpu.config, filesystem.config, memory.config"
    reemplazo "$puerto" "PUERTO_MEMORIA" "kernel"
    reemplazo "$puerto" "PUERTO_MEMORIA" "cpu"
    reemplazo "$puerto" "PUERTO_MEMORIA" "filesystem"
    reemplazo "$puerto" "PUERTO_ESCUCHA" "memory"
}
function autoconfigurar {
    local inet=$1
    while true; do
            read -p "Ingrese el modulo a autoconfigurar: " modulo
            if [ "$modulo" = "console" ]; then
              echo "No hace falta autoconfigurar, depende de kernel"
            elif [ "$modulo" = "kernel" ]; then
              kernelIp "$inet"
              kernelPuerto "8000"
            elif [ "$modulo" = "cpu" ]; then
              cpuIp "$inet"
              cpuPuerto "8001"
            elif [ "$modulo" = "filesystem" ]; then
              filesystemIp "$inet"
              filesystemPuerto "8003"
            elif [ "$modulo" = "memory" ]; then
              memoryIp "$inet"
              memoryPuerto "8002"
            else
              echo "Modulo invalido"
            fi
    done

}

while true; do
    comp=$1
    read -p "Ingrese LITERALMENTE modalidad auto o manual: " comp
    if [ "$comp" = "auto" ]; then
    read -p "Ingrese LITERALMENTE modalidad auto1 o auto2 o info: " comp
      if [ "$comp" = "info" ]; then
          echo "auto1 toma el valor encontrado en el primer inet (de enp0s3)"
          echo "auto2 toma el segundo valor encontrado en el segundo inet (de lo)"

      elif [ "$comp" = "auto1" ]; then
          primer_inet=$(ifconfig | awk '/inet / {print $2; exit}')
          echo "La direccion ip del primer inet: $primer_inet"
          autoconfigurar "$primer_inet"

      elif [ "$comp" = "auto2" ]; then
          segundo_inet=$(ifconfig | awk '/inet / {if (++count == 2) print $2}')
          echo "La direccion ip del segundo inet: $segundo_inet"
          autoconfigurar "$segundo_inet"
      else
          echo "Modalidad invalida"
      fi
    elif [ "$comp" = "manual" ]; then
      break
    else
      echo "Modalidad invalida"
    fi

done





# Bucle infinito para recibir continuamente los parámetros
while true; do
    comp=$1
    read -p "Ingrese LITERALMENTE puerto; o ip: " comp
    if [ "$comp" = "puerto" ]; then
    read -p "Ingrese el nombre del modulo: " modulo
    read -p "Ingrese el nuevo puerto : " puerto

                          if [ "$modulo" = "kernel" ]; then
                              kernelPuerto "$puerto"
                          fi
                          if [ "$modulo" = "cpu" ]; then
                              cpuPuerto "$puerto"
                          fi
                          if [ "$modulo" = "filesystem" ]; then
                              filesystemPuerto "$puerto"
                          fi
                          if [ "$modulo" = "memory" ]; then
                              memoryPuerto "$puerto"
                          fi

                          echo "Módulo actualizado: $modulo"
                          echo "PUERTO actualizado: $puerto"

    elif [ "$comp" = "ip" ]; then
                          read -p "Ingrese el nombre del modulo: " modulo
                          read -p "Ingrese la nueva direccion IP: " ip

                          if [ "$modulo" = "kernel" ]; then
                              kernelIp "$ip"
                          fi
                          if [ "$modulo" = "cpu" ]; then
                              cpuIp "$ip"
                          fi
                          if [ "$modulo" = "filesystem" ]; then
                              filesystemIp "$ip"
                          fi
                          if [ "$modulo" = "memory" ]; then
                              memoryIp "$ip"
                          fi

                          echo "Módulo actualizado: $modulo"
                          echo "IP actualizada: $ip"
    else
          echo "Se ingreso $comp, intente nuevamente"
    fi

done