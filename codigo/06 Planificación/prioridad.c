/**
 * Ejemplos de uso de la llamada a sistema nice para la modificación
 * de la prioridad estática de un proceso.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * La llamada a sistema nice permite modificar la prioridad estática de
 * un proceso, esta prioridad servirá como una referencia para iniciar
 * el cálculo la prioridad dinámica del proceso. Es esta última, la
 * prioridad dinámica, la que se utilizará para la planificación.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall prioridad.c -o prioridad
 * 
 * Ejecutar con (analizar cada caso por separado):
 *  $ ./prioridad
 *  $ nice -n 10 ./prioridad
 *  $ ./prioridad 5
 *  $ nice -n 7 ./prioridad 3
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>

int main (int n, char* args[]) {
	/* crear variables para guardar las prioridades */
	int prioridadActual, prioridadNueva;
	/* determinar prioridad nueva */
	if(n==2) prioridadNueva = atoi(args[1]);
	else prioridadNueva = 0;
	/* mostrar prioridad con que se llamo al proceso */
	prioridadActual = getpriority(PRIO_PROCESS, 0) + 120;
	printf("Prioridad original: %d\n", prioridadActual);
	/* cambiar prioridad y mostrarla */
	setpriority(PRIO_PROCESS, 0, prioridadNueva);
	prioridadActual = getpriority(PRIO_PROCESS, 0) + 120;
	printf("Prioridad cambiada: %d\n", prioridadActual);
	/* salir del programa :-) */
	return EXIT_SUCCESS;
}
