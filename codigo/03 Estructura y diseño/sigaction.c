/**
 * Copyright (C) 2014 Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
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
 * Ejemplo de como saber quien me envió una señal
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * Original: www.linuxprogrammingblog.com/code-examples/sigaction
 * 
 * Compilar con:
 *  $ gcc -Wall -pedantic sigaction.c -o sigaction
 * 
 * Ejecutar con:
 *  $ ./sigaction
 * 
 * Probar enviar señales SIGUSR1 o SIGUSR2 con:
 *  $ kill -SIGUSR1 PID
 *  $ kill -SIGUSR2 PID 
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void signal_handler (int signal, siginfo_t *info, void *context)
{
	printf ("Recibí la señal %d enviada por el proceso %d\n",
						signal, info->si_pid);
}
 
int main()
{
	struct sigaction action;
	printf("Soy el PID %d\n", getpid());
	action.sa_sigaction = &signal_handler;
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);
	for(;;);
	return 0;
}
