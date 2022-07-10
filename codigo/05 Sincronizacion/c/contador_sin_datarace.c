/**
 * Ejemplo de solución al problema de datarace.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Este ejemplo corresponde al problema "contador_con_datarace.c" pero
 * en este caso se ha realizado la sincronización para evitar el
 * datarace.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall contador_sin_datarace.c -o contador_sin_datarace -lpthread
 * 
 * Ejecutar con:
 * $ for i in `seq 1 100`; do ./contador_sin_datarace; done
 * 
 * El resultado esta vez deberá ser siempre:
 * contador = 100
 * 
 * Preguntas:
 * 1. ¿Qué implica el NULL en "pthread_mutex_init (&semaforo, NULL);"?
 * 2. ¿El mutex al ser inicializado está libre o tomado?
 * 3. ¿Qué sucedería si no tuvieramos "pthread_mutex_unlock(&semaforo);"?
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* prototipo de la función principal de las hebras */
void *ThreadMain (void *threadArgs);

/* variable para controlar y esperar a que terminen las hebras
   esto si está sincronizado (para evitar problemas, esta no es la
   sección crítica "interesante") */
int threads = 0;
pthread_mutex_t mutex;

/* variable compartida (esta será la sección crítica "interesante") */
int contador = 0;
pthread_mutex_t semaforo;

/**
 * Programa principal (lanza hebras)
 */
int main(int argc, char * argv[]) {
	/* variables */
	pthread_t threadID; /* identificador de la hebra */
	int i; /* contador */
	/* inicializar mutex para hebras */
	pthread_mutex_init (&mutex, NULL);
	/* crear hebras */
	for (i=0; i<100; ++i) {
		/* aumentar contador de threads */
		pthread_mutex_lock(&mutex);
		++threads;
		pthread_mutex_unlock(&mutex);
		/* crear theread */
		pthread_create(&threadID, NULL, ThreadMain, NULL);
	}
	/* esperar a que los threads finalicen */
	while(threads>0) sleep(1);
	/* mostrar valor del contador */
	printf("contador = %d\n", contador);
	/* retorno al sistema */
	return EXIT_SUCCESS;
}

/**
 * Programa principal de cada hebra
 */
void *ThreadMain (void *threadArgs) {
	int aux;
	/* aumentar contador */
	pthread_mutex_lock(&semaforo);
	aux = contador;
	contador = aux + 1;
	pthread_mutex_unlock(&semaforo);
	/* decrementar contador de hebras */
	pthread_mutex_lock(&mutex);
	--threads;
	pthread_mutex_unlock(&mutex);
	/* terminar la hebra */
	return NULL;
}
