/**
 * Ejemplo de creación de procesos pesados (hijos) usando fork.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * La creación de procesos pesados puede ser realizada de diferentes
 * formas, una de ellas corresponde a la usando fork, esta función
 * permitirá crear un hijo que será identico al padre en t=0, para luego
 * según se haya establecido en el programa seguir ejecutandose. Esto
 * creará un proceso pesado, con su propio PID y espacio en memoria.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall fork.c -o fork
 * 
 * Ejecutar con:
 *  $ ./fork
 *
 * Preguntas:
 * 1. ¿Qué pasa si no se utiliza wait?
 * 2. ¿Este programa se ejecuta en paralelo?
 * 2. En la implementación de un servidor web, se puede utilizar wait(). Justifique.
 * 3. Modificar el programa, de tal forma que si el status del proceso hijo es de fallo (hint: simular con modulo) el proceso padre deje de producir hijos.
 * 4. Modificar el programa, de tal forma que se ejecuten en paralelo los procesos hijos y el padre.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define HIJOS 5

int main(void) {
	/* crear hijos */
	int i, processID;
	int status;
	for (i=0; i<HIJOS; ++i) {
		/* hace un fork al proceso creando un hijo */
		if ((processID = fork()) < 0) {
			perror("No se ha podido hacer fork()");
			exit(EXIT_FAILURE);
		}
		/* si este es el proceso padre */
		else if (processID > 0) {
			/* mostrar mensaje de nuevo hijo creado */
			printf("Creado nuevo hijo con PID %d\n", processID);
			wait(&status);
			printf("Terminando el proceso con PID %d y estado %d\n", processID, status);
		}
		/* si este es el proceso hijo */
		else if (processID == 0) {
			/* fin de la ejecución */
			printf("Mi padre es el proceso con PID %d\n", getppid());
			exit(EXIT_SUCCESS);
		}
	}
	/* retorno al sistema */
	return EXIT_SUCCESS;
}
