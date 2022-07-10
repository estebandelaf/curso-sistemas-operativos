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
 * Ejemplo de como ignorar diferentes señales de término.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * SIGINT: Signal Interrupt
 * http://en.wikipedia.org/wiki/SIGINT_(POSIX)
 * Utilizada cuando el proceso es interrumpido por el usuario (Ctrl+c).
 * 
 * SIGHUP: Signal Hang Up
 * http://en.wikipedia.org/wiki/SIGHUP
 * Se envía al proceso cuando la terminal que controla al proceso, su
 * padre, es cerrada.
 * 
 * SIGTERM: Signal Terminate
 * http://en.wikipedia.org/wiki/SIGTERM
 * Enviada a un proceso al cual se le solicita terminal, caso por
 * defecto al utilizar kill sobre un proceso.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall signal_ignore.c -o signal_ignore
 * 
 * Ejecutar con:
 *  $ ./signal_ignore
 * 
 * Probar:
 *  - Ctrl+c
 *  - Cerrar la terminal (verificar con ps aux en otra que aun corre)
 *  - Enviar señal term
 * 
 * Para terminar la ejecución:
 *  $ kill -9 PID
 *
 * Ejercicio:
 *
 * Verificar antes de cerrar la terminal con pstree el proceso, ¿de quién es hijo?
 *
 * init─┬─ ...
 *      ├─konsole─┬─bash───signal_ignore
 *      ...
 *
 * Verificar después de cerrar la terminal con pstree el proceso, ¿de quién es hijo?
 *
 * init─┬─ ...
 *      ├─signal_ignore
 *      ...
 *
 * Fuente: http://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
 *
 * Preguntas:
 * 1. ¿Por qué signal_ignore no fue cerrado al cerrar a su padre?
 * 2. ¿Qué función se ejecuta cuando la señal es envíada?
 * 3. De un ejemplo de porque puedo querer ignorar una señal.
 * 4. ¿Qué señal un proceso no puede ignorar?
 * 
 */

#include <signal.h>

int main (void) {
	/* Se ignorarán señales que pueden terminar el proceso */
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	/* Hacer nada */
	for(;;);
	/* Retorno al sistema */
	return 0;
}
