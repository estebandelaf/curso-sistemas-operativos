/**
 * Ejemplo de Hola Mundo, versión módulo de Linux.
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Un módulo corresponde a código que será añadida en tiempo de
 * ejecución al área de memoria del sistema operativo, por lo cual los
 * módulos funcionarán en modo núcleo.
 * 
 * Para poder compilar un módulo se requieren las cabeceras del núcleo,
 * en un sistema Debian GNU/Linux (o Ubuntu) las instalamos con:
 * 
 *  # apt-get install linux-headers-`uname -r`
 * 
 * Teniendo el archivo Makefile y este fuente en el mismo directorio, se
 * compila con:
 *  $ make
 * 
 * Cargar con:
 *  # insmod holamundo.ko
 * Verificar que cargó:
 *  $ dmesg | tail
 *  [...]
 *  [12334.283059] Hola Mundo!
 * 
 * Descargar con: 
 *  # rmmod holamundo.ko
 * Verificar que descargó:
 *  $ dmesg | tail
 *  [...]
 *  [12446.529927] Chao Mundo!
 */

#include <linux/module.h>	/* Requerido por todos los módulos */
#include <linux/init.h>		/* Para usar module_init y module_exit */
#include <linux/kernel.h>	/* Para usar printk */

/* Información del módulo */
MODULE_AUTHOR("Esteban De La Fuente Rubio");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ejemplo de Hola Mundo!");

/* Prototipos de funciones */
static int module_load (void);
static void module_unload (void);

/* Mapeo de funciones para carga y descarga */
module_init(module_load);	/* Carga con insmod */
module_exit(module_unload);	/* Descarga con rmmod */
 
/* Implementación de las funciones */

static int module_load (void) {
     printk(KERN_INFO "Hola Mundo!\n");
     return 0;			/* 0 ok, !0 error en la carga */
}

static void module_unload (void) {
     printk(KERN_INFO "Chao Mundo!\n");
}
