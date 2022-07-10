/**
 * Ejemplo de módulo para un dispositivo virtual
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Para ver como compilar el módulo y cargarlo revisar ejemplo de módulo
 * holamundo.c que encontrará disponible en:
 * http://cursos.delaf.cl/cursos/sistemas-operativos/codigo#01holamundo
 * 
 * En /dev se encuentran los dispositivos que el sistema operativo puede
 * manipular. Para este ejemplo se crearán varios dispositivo /dev/memoryX,
 * donde la X>=0, sobre los cuales y utilizando este módulo se podrá escribir
 * y luego rescatar lo que se haya escrito. Cada dispositivo /dev/memoryX tendrá
 * su propio número menor, donde si es diferente será tratado como un buffer
 * diferente.
 * 
 * Ejemplo de uso:
 *  # echo hola mundo > /dev/memory0
 *  # echo chao mundo > /dev/memory1
 *  # cat /dev/memory0
 *  hola mundo
 *  # cat /dev/memory1
 *  chao mundo
 * 
 * Existen básicamente dos tipos de dispositivos:
 *  - De caracter (identificados por una "c" al hacer ls -l): son
 *    dispositivos que son accesados, típicamente, en orden secuencial.
 *    No utilizan buffers y se accede directamente al dispositivo.
 *    Se escriben y leen bytes.
 *    Son más simples de trabajar que los de bloque (con read y write).
 *  - De bloques (identificados por una "b" al hacer ls -l): son
 *    dispositivos que deben poder ser accesados de forma aleatoria.
 *    Utilizan buffering, por lo cual se deben acceder a través de una
 *    cache.
 *    Se escriben y leen bloques de datos.
 *    Son más complicados de utilizar que los de caracter.
 * http://www.cab.u-szeged.hu/linux/doc/khg/node18.html
 * 
 * Los números mayores y menores se pueden observar entre el grupo y la
 * fecha, ejemplo:
 * 
 * brw-rw---T 1 root disk 8, 0 jun 14 10:30 /dev/sda
 * 
 *  - Número mayor 8, usado para identificar el módulo que procesará el
 *    dispositivo.
 *  - Número menor 0, usado para identificar diferentes dispositivos que
 *    son atendidos por el mismo módulo.
 * http://www.makelinux.net/ldd3/chp-3-sect-2
 * 
 * Para facilidad de la programación el dispositivo /dev/memoryX será un
 * dispositivo de tipo caracter y tendrá un tamaño fijo, de tal forma
 * que se deberá pasar una cantidad limitada de caracteres para
 * almacenar. Adicionalmente, y para disminuir la complejidad del
 * ejercicio, no se realizarán accesos concurrentes al dispositivo, o
 * sea, solo un comando echo se ejecutará al mismo tiempo sobre un
 * /dev/memoryX, lo mismo para cat.
 * 
 * echo hará uso de la llamada a sistema write para escribir
 * cat hará uso de la llamada a sistema read para leer
 *
 * Se deberá modificar el código de tal forma de que permita utilizar un buffer
 * por cada número menor que pueda tener un dispositivo. No puede poner un
 * límite a la cantidad de dispositivos con número menor diferente (o buffers).
 * Se recomienda revisar el código "03_Solución_tarea_dispositivo_memory_2012-2"
 * disponible en:
 * http://cursos.delaf.cl/cursos/sistemas-operativos/codigo
 * 
 * Para crear el dispositivo de caracter con número mayor 70 y menor 0
 * utilizar:
 *  # mknod /dev/memory0 c 70 0
 *  $ ls -lh /dev/memory0 
 *  crw-r--r-- 1 root root 70, 0 nov  8 22:56 /dev/memory0
 * De la misma forma crear más dispositivos con otros números menores.
 * 
 * Antes de poder utilizar echo y cat sobre el dispositivo recordar
 * cargar el módulo (previa compilación del mismo), algo como:
 *  $ make
 *  # insmod memory.ko
 *  $ dmesg
 *  [...]
 *  [ 6684.586869] Cargando modulo memory
 *  [ 6684.586879] Modulo memory cargado correctamente
 *  [ 6684.586883] Numero mayor en uso para el dispositivo: 70
 */

/*
Resumen de comando para descargar, compilar, cargar y verificar:
clear; sudo rmmod memory; make clean all; sudo insmod memory.ko; dmesg | tail
*/

/* Bibliotecas */
#include <linux/module.h>	/* requerido por todos los módulos */
#include <linux/init.h>		/* module_init y module_exit */
#include <linux/kernel.h>	/* printk y KERN_INFO */
#include <linux/fs.h>		/* struct file_operations */
#include <linux/slab.h>		/* kmalloc */
#include <asm/uaccess.h>	/* copy_to_user y copy_from_user */

/* Definiciones */
#define DEV_MAJOR 70		/* número mayor que identifica el módulo */
#define BUFFER_SIZE 100		/* tamaño máximo del buffer (dispositivo) */
#define DEV_MAXS ((128*1024)/(BUFFER_SIZE)) /* límite de kmalloc es 128 kB */

/* Información del módulo */
MODULE_AUTHOR ("Esteban De La Fuente Rubio");
MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("Ejemplo de múltiples buffers virtuales");
MODULE_SUPPORTED_DEVICE ("/dev/memory*");

/* Prototipos de funciones */
static int module_load (void);
static void module_unload (void);
int device_open (struct inode *inode, struct file *filp);
int device_release (struct inode *inode, struct file *filp);
ssize_t device_read (struct file *filp, char *buf, size_t length,
			loff_t *offset);
ssize_t device_write (struct file *filp, const char *buf, size_t length,
			loff_t *offset);

/* Estructura con funciones para manejar el dispositivo. Se hace un mapeo entre
las llamadas a sistema y las funciones definidas */
struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write
};

/* Mapeo de funciones para carga y descarga */
module_init (module_load);	/* Carga con insmod */
module_exit (module_unload);	/* Descarga con rmmod */

/* Variables globales */
char *buffer = NULL;		/* este ES el dispositivo virtual */
char *pBuffer = NULL;		/* puntero para recorrer el buffer */
int devs_actual_count = 0;	/* cantidad actual de dispositivos */
 
/**
 * Función llamada al cargar el módulo:
 *  - Registra el dispositivo de caracter con register_chrdev
 *  - Verifica que no existió error al registrar
 *  - Genera mensaje de tipo KERN_INFO
 * @return 0 en caso de éxito, -1 en caso de error
 */
static int module_load (void)
{
	/* variable para el estado devuelto por register_chrdev */
	short status = -1;
	/* mensaje del kernel */
	printk (KERN_INFO "Cargando modulo memory\n");
	/* registrar el dispositivo de caracter
	http://www.fsl.cs.sunysb.edu/kernel-api/re941.html */
	/* [...] */
	/* verificar que no ha habido error (retorno negativo) al registrar el
	dispositivo */
	/* [...] */
	/* mensaje indicando que el modulo se cargó */
	printk (KERN_INFO "Modulo memory cargado correctamente\n");
	printk (KERN_INFO "Numero mayor en uso para el dispositivo: %d\n",
	       DEV_MAJOR);
	/* retornar todo ok */
	return 0;
}

/**
 * Función llamada al descargar el módulo
 *  - Desregistra el dispositivo de caracter con unregister_chrdev
 *  - Libera la memoria solicitada para el buffer con kfree
 *  - Genera mensaje de tipo KERN_INFO
 */
static void module_unload (void)
{
	/* desregistrar */
	/* [...] */
	/* liberar memoria */
	/* [...] */
	/* mensaje indicando que se descargó el módulo */
	printk (KERN_INFO "Modulo memory descargado\n");
}

/**
 * Función que abre el dispositivo:
 * Pone el puntero del buffer al inicio del buffer para el número menor
 * solicitado.
 * @param inode Información sobre el archivo
 * @param filp Representación del archivo que se abrirá
 * @return 0 en caso de éxito, -1 en caso de error
 */
int device_open (struct inode *inode, struct file *filp)
{
	/* buffer auxiliar, si no hay memoria suficiente en el buffer para el
	número menor solicitado se deberá pedir más, copiar y luego liberar,
	este buffer temporal permite guardar la memoria solicitada para crecer
	*/
	/* [...] */
	/* obtener número menor del dispositivo al que se está accediendo
	http://www.makelinux.net/ldd3/chp-3-sect-3 */
	/* [...] */
	/* si el número menor es mayor que los permitidos error */
	/* [...] */
	/* si el puntero es null se inicializa vacío para que soporte a lo menos
	hasta el número menor minor solicitado */
	if (buffer == NULL) {
		/* solicitar memoria para los buffers hasta al menos el número
		menor en el espacio de memoria del kernel
		http://www.fiveanddime.net/man-pages/kmalloc.9.html */
		/* [...] */
		/* en caso de que no se haya asignado memoria error */
		/* [...] */
		/* inicializar en ceros la memoria: $ man memset */
		/* [...] */
		/* actualizar el conteo de dispositivos disponibles */
		/* [...] */
	}
	/* aquí estamos seguro que el buffer está creado */
	/* se debe verificar ahora que exista espacio en el buffer para el
	número menor solicitado, si no hay espacio se debe pedir nuevo espacio y
	copiar los datos del buffer actual al nuevo espacio liberando el
	anterior (se usa buffer auxiliar para la asignación temporal de la
	nueva memoria) */
	if (devs_actual_count < (minor + 1)) {
		/* pedir nuevo espacio de memoria para el nuevo buffer */
		/* [...] */
		/* en caso de que no se haya asignado memoria error */
		/* [...] */
		/* inicializar en ceros la memoria: $ man memset */
		/* [...] */
		/* copiar lo del buffer antiguo al nuevo buffer: $ man memcpy */
		/* [...] */
		/* liberar memoria del antiguo buffer */
		/* [...] */
		/* actualizar puntero del buffer al nuevo buffer */
		/* [...] */
		/* actualizar el conteo de dispositivos disponibles */
		/* [...] */
	}
	/* aquí estamos seguros que hay espacio en el buffer para el número
	menor solicitado */
	/* poner el puntero al inicio del buffer del número menor para realizar
	la operación que se haya solicitado con posterioridad por echo o cat */
	/* [...] */
	/* todo ha ido ok */
	return 0;
}

/**
 * Función que libera el dispositivo
 * No se hace nada, ya que el dispositivo esta en RAM
 * @param inode Información sobre el archivo
 * @param filp Representación del archivo abierto
 * @return Siempre 0, ya que no se hace nada especial
 */
int device_release (struct inode *inode, struct file *filp)
{
	return 0;
}

/**
 * Función que lee desde el dispositivo:
 *  - Verifica que el puntero este en una posición válida
 *  - Copia desde el dispositivo virtual al buffer entregado por el usuario
 * Sobre las estructuras de datos: http://www.tldp.org/LDP/tlk/ds/ds.html
 * @param filp Representación del archivo abierto
 * @param buff Apunta al espacio de direcciones del proceso (no es de fiar)
 * @param length Cantidad de bytes que se leeran
 * @param offset Desde que byte se leera
 * @return Cantidad de bytes leídos desde el dispositivo
 */
ssize_t device_read (struct file *filp, char *buff, size_t length,
			loff_t *offset)
{
	int copied = 0;
	/* si no hay más datos entonces es fin de archivo (EOF) */
	if (!(*pBuffer)) return 0;
	/* copiar */
	/* [...] */
	/* mensaje de lo leído y retornar */
	printk(KERN_INFO "Se han leido %d bytes\n", copied);
	return copied;
}

/**
 * Función que escribe en el dispositivo
 * Copia desde el buffer entregado por el usuario al dispositivo (sobreescribe)
 * Sobre las estructuras de datos: http://www.tldp.org/LDP/tlk/ds/ds.html
 * @param filp Representación del archivo abierto
 * @param buff Apunta al espacio de direcciones del proceso
 * @param length Cantidad de bytes que se escribiran
 * @param offset Desde que byte se leera
 * @return Cantidad de bytes escritos en el dispositivo
 */
ssize_t device_write (struct file *filp, const char *buff, size_t length,
			loff_t *offset)
{
	int copied = 0;
	memset(pBuffer, 0, BUFFER_SIZE);
	/* copiar */
	/* [...] */
	/* mensaje de lo escrito y retornar */
	printk(KERN_INFO "Se han escrito %d bytes\n", copied);
	return copied;
}
