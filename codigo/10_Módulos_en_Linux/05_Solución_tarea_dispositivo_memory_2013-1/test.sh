#/bin/bash
#
# Test de tarea 3 Sistemas Operativos (2013-1)
# by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
#
# Ejecutar pruebas con: $ bash test.sh
# Dejar todo limpio con: $ bash test.sh clean
#

# hasta que número menor crear dispositivos (desde 0)
# además se creará uno extra de número menor igual a 0
MINOR_MAX=5

# número mayor que se utilizará
MAJOR=70

# limpiar pantalla
clear

# borrar dispositivos si existen
echo "Borrando $(($MINOR_MAX+1)) dispositivos (si existen)..."
for MINOR in `seq 0 $((MINOR_MAX+1))`; do
	rm -f memory$MINOR
done

# limpiar directorio
echo "Limpiando código compilado del módulo (si existe)..."
make clean > /dev/null

# descargar módulo si ya está cargado
echo "Descargando módulo (si está cargado)..."
if [ `lsmod | grep memory | wc -l` -eq 1 ]; then
	sudo rmmod memory
fi

# si solo se pidió limpiar se detiene ejecución aquí
if [ "$1" = "clean" ]; then
	exit
fi

# crear dispositivos
echo "Creando $(($MINOR_MAX+1)) dispositivos..."
for MINOR in `seq 0 $MINOR_MAX`; do
	sudo mknod memory$MINOR c $MAJOR $MINOR
done
sudo mknod memory$(($MINOR+1)) c $MAJOR 0

# ajustar propietario de los dispositivos recién creados (para evitar sudo)
echo "Cambiando propietario de los dispositivos a `whoami`..."
for MINOR in `seq 0 $((MINOR_MAX+1))`; do
	sudo chown `whoami`: memory$MINOR
done

# compilar módulo
echo "Compilando módulo..."
make > /dev/null

# cargar módulo
echo "Cargando módulo..."
sudo insmod memory.ko

# ejecutar pruebas
echo ""
echo "Ejecutando pruebas:"
echo ""

# prueba 1
echo "1. Escribiendo 'Soy X' en cada dispositivo, con X={0...$((MINOR_MAX+1))}"
echo "   Se espera que memory0 tenga lo mismo que memory$((MINOR_MAX+1)), todos"
echo "   los demás dispositivos deberán tener contenido diferente."
echo ""
for MINOR in `seq 0 $((MINOR_MAX+1))`; do
	echo "Soy $MINOR" > memory$MINOR
done
for MINOR in `seq 0 $((MINOR_MAX+1))`; do
	echo -n "      memory$MINOR: "
	cat memory$MINOR
done
echo ""

# prueba 2
echo "2. Escribiendo 'Ahora seré memoryX' en los dispositivos memoryX, con"
echo "   X={0...$((MINOR_MAX-2))}. Se espera que desde memory0 a"
echo "   memory$((MINOR_MAX-2)) este el nuevo contenido, en los otros debe"
echo "   estar el contenido antiguo. Notar que memory$((MINOR_MAX+1)) debe"
echo "   cambiar, a pesar de que no se ha tocado."
echo ""
for MINOR in `seq 0 $((MINOR_MAX-2))`; do
	echo "Ahora seré memory$MINOR" > memory$MINOR
done
for MINOR in `seq 0 $((MINOR_MAX+1))`; do
	echo -n "      memory$MINOR: "
	cat memory$MINOR
done
echo ""
