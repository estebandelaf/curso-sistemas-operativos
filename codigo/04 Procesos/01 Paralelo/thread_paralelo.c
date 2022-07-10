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
 *  $ gcc -ansi -pedantic -Wall thread_paralelo.c -o thread_paralelo -lpthread
 * 
 * Ejecutar con:
 *  $ ./thread_paralelo
 *
 * Preguntas:
 * 1. ¿Por qué es necesario utilizar pthread_mutex_lock y pthread_mutex_unlock?
 * 2. Si no se utilizara pthread_mutex_lock y pthread_mutex_unlock ¿qué podría ocurrir con el proceso?
 * 3. ¿Qué pasa si no se utiliza pthread_detach?
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREADS 5

typedef struct {
	int i;
	int *threads;
	pthread_mutex_t *mutex;
} ThreadArgs;

void *ThreadMain (void *threadArgs);

int main(int argc, char * argv[]) {
	/* declaracion de variables */
	pthread_t threadID; /* thread id de pthread_create() */
	ThreadArgs *threadArgs; /* puntero a los argumentos del thread */
	/* crear hebras */
	int i;
	int threads = 0; /* contador para los threads */
	pthread_mutex_t mutex; /* para sección crítica "threads" */
	pthread_mutex_init (&mutex, NULL);
	for (i=0; i<THREADS; ++i) {
		/* aumentar contador de threads */
		pthread_mutex_lock (&mutex);
		++threads;
		pthread_mutex_unlock (&mutex);
		/* crear espacio de memoria para los argumentos de la hebra */
		threadArgs = malloc(sizeof(ThreadArgs));
		/* colocar argumentos para la hebra */
		threadArgs->i = i;
		threadArgs->threads = &threads;
		threadArgs->mutex = &mutex;
		/* crear thread */
		pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs);
		/* mostrar mensaje de nueva hebra creada */
		printf("Creada nueva hebra con ID %ld\n", (long int) threadID);
	}
	/* esperar a que los threads finalicen */
	while(threads>0) sleep(1);
	/* retorno al sistema */
	return EXIT_SUCCESS;
}

void *ThreadMain (void *threadArgs) {
	/* recuperar parámetros pasados a la hebra */
	ThreadArgs *args = (ThreadArgs *) threadArgs;
	/* marcar la hebra como separada, se liberaran los recursos cuando termine la hebra, no requiere hacer join */
	pthread_detach(pthread_self());
	/* fin de la conexion */
	printf("Fin de la hebra lanzada con i=%d, con ID %ld\n", args->i, pthread_self());
	/* decrementar contador de hebras */
	pthread_mutex_lock (args->mutex);
	--(*args->threads);
	pthread_mutex_unlock (args->mutex);
	/* terminar la hebra */
	return NULL;
}
