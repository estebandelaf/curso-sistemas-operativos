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
 * Ejemplos de llamadas al sistema.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Las llamadas al sistema presentarán una interfaz para poder hacer
 * uso de los servicios del sistema operativo. Muchas veces no haremos
 * uso directo de las llamadas al sistema del sistema operativo, sino
 * que haremos uso de la API del lenguaje la cual será la que traducirá
 * los requerimientos a las llamadas de sistema correspondientes.
 * 
 * Compilar con:
 *  $ gcc -ansi -pedantic -Wall systemcall.c -o systemcall
 * 
 * Ejecutar con:
 *  $ ./systemcall
 * 
 * Programa quedará en pausa (por la systemcall pause), para verificar
 *  $ ps aux | grep systemcall
 *  8212 pts/0    S+     0:00 ./systemcall
 * La S+ indica que el proceso está suspendido y que espera por un
 * suceso (que nunca llegará) enviar señal TERM para detener, se hace
 * mediante el comando kill que a su vez hace uso de la systemcall kill:
 *  $ kill -TERM 8212
 * Donde 8212 es el PID del proceso
 * 
 * Fuente: http://cursos.delaf.cl/archivos/cursos/sistemas-operativos/material-de-apoyo/Linux%20System%20Call%20Quick%20Reference.pdf
 *
 * Preguntas:
 * 1. ¿Qué hace la función syscall?
 * 2. ¿En qué archivo está declarada la constante SYS_getpid?
 * 3. ¿Qué hace la llamada a sistema SYS_pause?
 * 4. ¿Cuáles son las llamadas a sistema para trabajar con archivos?
 * 5. ¿Cómo sabe Linux qué código ejecutar para cada llamada a sistema de
 *    operaciones sobre dispositivos (específicamente de caracter)?
 * 
 */

#include <syscall.h>
#include <unistd.h>
#include <stdio.h>

void _getpid () {
	/* llamada al sistema usando directamente su número */
	printf("syscall(39)         = %d\n", syscall(39));
	/* llamada al sistema usando su constante */
	printf("syscall(SYS_getpid) = %d\n", syscall(SYS_getpid));
	/* llamada al sistema usando la función de la API de C */
	printf("getpid()            = %d\n", getpid());
	printf("\n");
}

void _getuid () {
	/* llamada al sistema usando su constante */
	printf ("syscall(SYS_getuid) = %d\n", syscall(SYS_getuid));
	printf("\n");
}

void _pause () {
	syscall(SYS_pause);
}

int main(void) {
	_getpid();
	_getuid();
	_pause();
	return 0;
}
