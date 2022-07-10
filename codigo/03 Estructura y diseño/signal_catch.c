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
 * Ejemplo de como capturar y manejar diferentes señales de término.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Nota: revisar explicación en signal_ignore.c
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall signal_catch.c -o signal_catch
 * 
 * Ejecutar con:
 *  $ ./signal_catch
 * 
 * Probar (verificar mensajes en archivo log que se genera):
 *  - Ctrl+c
 *  - Enviar señal TERM
 * 
 * Para terminar la ejecución:
 *  $ kill -9 PID
 *
 * Problema del ejemplo: si se presiona Ctrl+c dos veces el programa
 * se cierra, ¿por qué?, lo mismo ocurre al enviar dos veces la señal
 * TERM. Ejercicio propuesto: soluciónelo.
 *
 * Preguntas:
 * 1. ¿Cuándo se gatilla SIGINT?
 * 2. ¿Cuando se gatilla SIGTERM?
 * 3. ¿Por qué es necesario el for(;;);?
 * 4. ¿Que hace el comando kill?
 * 5. ¿Qué hace la señal KILL o -9?
 * 6. ¿Cuál es la diferencia entre los comándos kill y killall?
 *
 */

#include <signal.h>
#include <stdio.h>

void sigint_handler (int signal) {
	printf("\nTrataste de cerrarme con SIGINT/%d (Ctrl+c)!\n", signal);
}

void sigterm_handler (int signal) {
	printf("Recibí la señal SIGTERM/%d, pero no me cerraré!\n", signal);
}

int main (void) {
	/* Se ignorarán señales que pueden terminar el proceso */
	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigterm_handler);
	/* Hacer nada */
	for(;;);
	/* Retorno al sistema */
	return 0;
}
