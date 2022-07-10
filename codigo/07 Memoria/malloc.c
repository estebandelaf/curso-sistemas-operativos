/**
 * Ejemplo de solicitud de memoria dinámica utilizando malloc
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Un programa esta formado por dos partes: código y datos, esto último
 * corresponde a los datos estáticos del programa. Aquel espacio para
 * datos que es solicitado de forma dinámica a medida que el programa
 * se va ejecutando se conoce como memoria dinámica. En este ejemplo se
 * podrá ver como pedir memoria dinámica y el crecimiendo del espacio
 * de memoria que tiene asignada el proceso.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall malloc.c -o malloc
 * 
 * Ejecutar con:
 *  $ ./malloc
 * 
 * Una vez que se ejecute las opciones son:
 *	B	para pedir 1 byte
 *	KB	para pedir 1 kilobyte
 *	MB	para pedir 1 megabyte
 *	GB	para pedir 1 gigabyte
 *	0	para salir del programa
 * 
 * Una vez ejecutado el programa verificar la memoria que usa con:
 *  $ pmap `pidof malloc`
 * Debería indicar algo como: total             3928K
 * Ingresar 3 veces MB en las opciones del menú del programa y volver a
 * verificar la memoria utilizada, ahora debería mostrar algo como:
 *  total             7012K (ahora el proceso ha solicitado más memoria)
 * 
 * Preguntas:
 * 1. La memoria que se está solicitando, ¿dónde es almacenada?
 * 2. ¿Cuál es la diferencia entre malloc y calloc?
 * 3. Al listar la memoria con pmap aparecen líneas como:
 *    00007fb25b168000   1524K r-x--  /lib/x86_64-linux-gnu/libc-2.13.so
 *    ¿a qué corresponden estás líneas?
 * 4. ¿Qué es un "memory leak"?
 * 5. Si descomenta la línea free(puntero); y verifica la memoria usando
 *    pmap al ir pidiendo memoria verá que después de ciertas
 *    solicitudes la memoria solicitada no aumenta, ¿a qué se debe esto?
 * 6. En algunos lenguajes, como Java, existe un "garbage collector",
 *    ¿en qué consiste? ¿qué ventajas entrega?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B 1
#define KB B*1024
#define MB KB*1024
#define GB MB*1024

int main (void) {
	/* variables */
	char tamanio[2];
	short int salir = 0;
	char *puntero = NULL;
	/* ciclo "infinito" para el menú */
	while(!salir) {
		/* solicitar tamaño de memoria a cargar */
		printf("B = 1 byte\n");
		printf("KB = 1 kilobyte\n");
		printf("MB = 1 megabyte\n");
		printf("GB = 1 gigabyte\n");
		printf("¿Cuánta memoria desea solicitar? (0 para salir): ");
		scanf("%s", tamanio);
		printf("\n");
		/* si es 0 se cierra el programa */
		if(!strcmp(tamanio, "0")) {
			salir = 1;
			continue;
		}
		/* pedir la memoria solicitada */
		if(!strcmp(tamanio, "B")) puntero = (char *) malloc(B);
		else if(!strcmp(tamanio, "KB")) puntero = (char *) malloc(KB);
		else if(!strcmp(tamanio, "MB")) puntero = (char *) malloc(MB);
		else if(!strcmp(tamanio, "GB")) puntero = (char *) malloc(GB);
		/* mostrar información del puntero */
		printf("Puntero creado = %ld\n", (long int)puntero);
		/* liberar memoria del puntero */
/*		free(puntero); */
		/* salto de línea para separar ejecuciones */
		printf("\n");
	}
	return EXIT_SUCCESS;
}

