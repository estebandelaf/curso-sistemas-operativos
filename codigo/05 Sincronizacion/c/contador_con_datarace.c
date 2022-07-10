/**
 * Ejemplo de datarace.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * El problema de datarace ocurre al no realizar la sincronización de
 * los procesos (en este caso livianos, con hebras) al querer acceder
 * a una sección crítica (memoria compartida). El ejemplo mostrará (en
 * el peor de los casos) como el valor del contador puede ser totalmente
 * diferente.
 * 
 * En vez de utilizar la estructura para pasar las variables compartidas
 * (mediante punteros), se utilizarán variables globales (esto desde el
 * punto de vista de la correcta programación no se debe hacer) ya que
 * son más simples y se parecen más a los ejemplos vistos en clases.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall contador_con_datarace.c -o contador_con_datarace -lpthread
 * 
 * Ejecutar con:
 *  $ ./contador_con_datarace
 * 
 * Recordar que la situación que lleva al datarace es no determinística
 * por lo cual a veces podría dar un resultado correcto, se recomienda
 * ejecutar varias veces para ver que ocurre. Esto se puede hacer así:
 * 
 * $ for i in `seq 1 100`; do ./contador_con_datarace; done
 * 
 * Con "mala suerte" el resultado contendrá algo como:
 * contador = 100
 * contador = 100
 * [...]
 * contador = 96      <- ¡DATARACE!
 * contador = 100
 * [...]
 * contador = 100
 * 
 * Preguntas:
 * 1. ¿Por qué contador es sección crítica?
 * 2. ¿Cómo se evitan los datarace en threads?
 * 3. ¿Qué hace "while(threads>0) sleep(1);"?
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* prototipo de la función principal de las hebras */
void *ThreadMain (void *threadArgs);

/* variable para controlar0 y esperar a que terminen las hebras
   esto si está sincronizado (para evitar problemas, esta no es la
   sección crítica "interesante") */
int threads = 0;
pthread_mutex_t mutex;

/* variable compartida (esta será la sección crítica "interesante") */
int contador = 0;

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
	/* aumentar contador */
	contador = contador + 1;
	/* decrementar contador de hebras */
	pthread_mutex_lock(&mutex);
	--threads;
	pthread_mutex_unlock(&mutex);
	/* terminar la hebra */
	return NULL;
}
