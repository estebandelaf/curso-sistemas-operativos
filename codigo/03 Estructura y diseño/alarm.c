/**
 * Copyright (C) 2013 Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 *
 * Este programa es software libre: usted puede redistribuirlo y/o
 * modificarlo bajo los términos de la Licencia Pública General GNU
 * publicada por la Fundación para el Software Libre, ya sea la versión
 * 3 de la Licencia, o (a su elección) cualquier versión posterior de la
 * misma.
 *
 * Este programa se distribuye con la esperanza de que sea útil, pero
 * SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
 * MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
 * Consulte los detalles de la Licencia Pública General GNU para obtener
 * una información más detallada.
 *
 * Debería haber recibido una copia de la Licencia Pública General GNU
 * junto a este programa.
 * En caso contrario, consulte <http://www.gnu.org/licenses/gpl.html>.
 */

/**
 * Ejemplo de timeout utilizando la señal ALARM.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Una alarma corresponde a un tipo de señal, que llama a la función
 * asociada a dicha la señal después de que el tiempo para el cual se
 * programo la alarma. En este ejemplo se utiliza esto para programar
 * una función que terminará el proceso por timeout después de 5
 * segundos, tiempo que se programa para que ocurra la alarma. Si la
 * ejecución el proceso dura menos de 5 segundos (determinado por sleep)
 * entonces tendrá un termino normal.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall alarm.c -o alarm
 * 
 * Ejecutar con (analizar diferentes casos):
 *  $ time ./alarm
 *  $ time ./alarm 10
 *  $ time ./alarm 3
 *
 * Preguntas:
 * 1. ¿Qué hace la función signal?
 * 2. ¿Qué significa el parámetro SIGALRM en la función signal?
 * 3. ¿Cuándo se ejecuta la función timeout_exit?
 * 4. ¿Para qué es la función sleep?
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void timeout_exit () {
	printf("Terminando por timeout\n");
	exit(EXIT_FAILURE);
}

int main (int argc, char **argv) {
	/* Crear una señal de alarma para generar timeout */
	signal(SIGALRM, timeout_exit);
	alarm(5);
	/* "Pausar" la ejecución del proceso por X segundos */
	if(argc==2) sleep(atoi(argv[1]));
	/* Desactivar la alarma */
	alarm(0);
	/* Mostrar mensaje de termino normal */
	printf("Terminando con normalidad\n");
	/* Retorno del sistema */
	return EXIT_SUCCESS;
}

