/**
 * Ejemplo de creación de procesos livianos (hebras).
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * La creación de un proceso liviado implicará que los procesos que se
 * ejecutan compartirán la memoria con entre ellos. Se compartirán
 * variables globales y cualquier otra variables local que sea pasada
 * mediante un puntero a los argumentos de la hebra. Para poder utilizar
 * estas variables compartidas de forma segura es necesario sincronizar,
 * es por esto que en este ejemplo se hace uso de un mutex para la
 * sincronización de la variable "threads".
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall thread.c -o thread -lpthread
 * 
 * Ejecutar con:
 *  $ ./thread
 *
 * Preguntas:
 * 1. ¿Ejecución en paralelo? Justifique.
 * 2. ¿Para qué es utilizada la estructura ThreadArgs?
 * 3. ¿Para que es el segundo parámetro de pthread_create? ¿Qué significa que sea NULL?
 * 4. Modificar el programa para que el valor de (int)status sea distinto a 0.
 * 5. ¿Se puede recuperar a través de pthread_join un valor de retorno diferente a int?
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREADS 5

typedef struct {
	int i;
} ThreadArgs;

void *ThreadMain (void *threadArgs);

int main(int argc, char * argv[]) {
	/* declaracion de variables */
	pthread_t threadID; /* thread id de pthread_create() */
	ThreadArgs *threadArgs; /* puntero a los argumentos del thread */
	int status;
	/* crear hebras */
	int i;
	for (i=0; i<THREADS; ++i) {
		/* crear espacio de memoria para los argumentos de la hebra */
		threadArgs = malloc(sizeof(ThreadArgs));
		/* colocar argumentos para la hebra */
		threadArgs->i = i;
		/* crear thread */
		pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs);
		/* mostrar mensaje de nueva hebra creada */
		printf("Creada nueva hebra con ID %ld\n", (long int) threadID);
		/* esperar por la hebra */
		pthread_join(threadID, (void *)&status);
		printf("Retorno de la hebra con ID %ld fue %d\n", (long int) threadID, (int) status);
		/* liberar memoria de los argumentos */
		free(threadArgs);
	}
	/* retorno al sistema */
	return EXIT_SUCCESS;
}

void *ThreadMain (void *threadArgs) {
	/* recuperar parámetros pasados a la hebra */
	ThreadArgs *args = (ThreadArgs *) threadArgs;
	/* fin de la conexion */
	printf("Fin de la hebra lanzada con i=%d, con ID %ld\n", args->i, pthread_self());
	/* terminar la hebra */
	return NULL;
}
