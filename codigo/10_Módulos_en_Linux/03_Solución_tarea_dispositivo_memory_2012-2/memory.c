/**
 * Ejemplo de módulo para un dispositivo virtual
 * by Esteban De La Fuente Rubio, DeLaF (esteban[at]delaf.cl)
 * 
 * Para ver como compilar el módulo y cargarlo revisar ejemplo de módulo
 * holamundo.c que encontrará disponible en:
 * http://cursos.delaf.cl/cursos/sistemas-operativos/codigo#01holamundo
 * 
 * En /dev se encuentran los dispositivos que el sistema operativo puede
 * manipular. El ejemplo creará un dispositivo /dev/memory sobre el cual
 * y utilizando este módulo se podrá escribir y luego rescatar lo que se
 * haya escrito.
 * 
 * Ejemplo de uso:
 *  # echo hola mundo > /dev/memory
 *  # cat /dev/memory
 *  hola mundo
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
 * Para facilidad de la programación el dispositivo /dev/memory será un
 * dispositivo de tipo caracter y tendrá un tamaño fijo, de tal forma
 * que se deberá pasar una cantidad limitada de caracteres para
 * almacenar. Adicionalmente, y para disminuir la complejidad del
 * ejercicio, no se realizarán accesos concurrentes al dispositivo, o
 * sea, solo un comando echo se ejecutará al mismo tiempo sobre
 * /dev/memory, lo mismo para cat.
 * 
 * echo hará uso de la llamada a sistema write para escribir
 * cat hará uso de la llamada a sistema read para leer
 *
 * Por lo anterior se deberán implementar las funciones que se mapearan
 * a las llamadas a sistema write y read.
 * 
 * Para crear el dispositivo de caracter con número mayor 70 y menor 0
 * utilizar:
 *  # mknod /dev/memory c 70 0
 *  $ ls -lh /dev/memory 
 *  crw-r--r-- 1 root root 70, 0 nov  8 22:56 /dev/memory
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

// Bibliotecas
#include <linux/module.h>	// Requerido por todos los módulos
#include <linux/init.h>		// module_init y module_exit
#include <linux/kernel.h>	// printk y KERN_INFO
#include <linux/fs.h>		// struct file_operations
#include <linux/slab.h>		// kmalloc
#include <asm/uaccess.h>	// copy_to_user y copy_from_user

// Definiciones
#define BUFFER_SIZE 100		// Tamaño máximo del buffer (dispositivo)

// Información del módulo
MODULE_AUTHOR("Esteban De La Fuente Rubio");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ejemplo simple de un buffer");
MODULE_SUPPORTED_DEVICE("/dev/memory");

// Prototipos de funciones
static int module_load (void);
static void module_unload (void);
int device_open (struct inode *inode, struct file *filp);
int device_release (struct inode *inode, struct file *filp);
ssize_t device_read (struct file *filp, char *buf, size_t length, loff_t *offset);
ssize_t device_write (struct file *filp, const char *buf, size_t length, loff_t *offset);

// Estructura con funciones para manejar el dispositivo
// Se hace un mapeo entre las llamadas a sistema y las funciones definidas
struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write
};

// Mapeo de funciones para carga y descarga
module_init(module_load);	// Carga con insmod
module_exit(module_unload);	// Descarga con rmmod

// Variables globales
short major = 70;			// Número mayor que identifica el módulo
char *buffer = NULL;		// Este ES el dispositivo virtual
char *pBuffer = NULL;		// Puntero para recorrer el buffer
 
/**
 * Función llamada al cargar el módulo:
 *  - Registra el dispositivo de caracter con register_chrdev
 *  - Verifica que no existió error al registrar
 *  - Solicita memoria para el buffer con kmalloc en el área del núcleo
 *  - Verifica que no hubo error al solicitar memoria
 *  - Inicializa el buffer en 0 con memset
 *  - Genera mensaje de tipo KERN_INFO
 * @return 0 en caso de éxito, -1 en caso de error
 */
static int module_load (void) {
	short status;
	// mensaje del kernel
	printk(KERN_INFO "Cargando modulo memory\n");
	// registrar el dispositivo de caracter
	// http://www.fsl.cs.sunysb.edu/kernel-api/re941.html
	status = register_chrdev (
		major,		// número mayor
		"memory",	// nombre para los dispositivos
		&fops		// operaciones asociadas al dispositivo (el "mapeo")
	);
	// verificar que no ha habido error (retorno negativo) al registrar
	// el dispositivo
	if(status < 0) {
		printk(KERN_INFO "A ocurrido un error al ejecutar register_chrdev\n");
		return status;
	}
	// solicitar memoria para el buffer
	// http://www.fiveanddime.net/man-pages/kmalloc.9.html
	buffer = kmalloc (
		BUFFER_SIZE,	// Tamaño de la memoria solicitada
		GFP_KERNEL		// Solicita memoria en el espacio del núcleo
	);
	// en caso de que no se haya asignado memoria error
	if(buffer==NULL) {
		printk(KERN_INFO "Ha ocurrido un error al asignar memoria con kmalloc\n");
		return -1;
	}
	// inicializar en ceros la memoria
	// $ man memset
	memset(buffer, 0, BUFFER_SIZE);
	// mensaje indicando que el modulo se cargó
	printk(KERN_INFO "Modulo memory cargado correctamente\n");
	printk(KERN_INFO "Numero mayor en uso para el dispositivo: %d\n", major);
	// retornar todo ok
	return 0;
}

/**
 * Función llamada al descargar el módulo
 *  - Desregistra el dispositivo de caracter con unregister_chrdev
 *  - Libera la memoria solicitada para el buffer con kfree
 *  - Genera mensaje de tipo KERN_INFO
 */
static void module_unload (void) {
	// desregistrar
	unregister_chrdev(major, "memory");
	// liberar memoria
	kfree(buffer);
	// mensaje indicando que se descargó el módulo
	printk(KERN_INFO "Modulo memory descargado\n");
}

/**
 * Función que abre el dispositivo:
 *  - Pone el puntero del buffer al inicio del buffer
 * @param inode Información sobre el archivo
 * @param filp Representación del archivo que se abrirá
 * @return Siempre 0, ya que no se hace nada especial
 */
int device_open (struct inode *inode, struct file *filp) {
	pBuffer = buffer;
	return 0;
}

/**
 * Función que abre el dispositivo
 * No se hace nada, ya que el dispositivo esta en RAM
 * @param inode Información sobre el archivo
 * @param filp Representación del archivo abierto
 * @return Siempre 0, ya que no se hace nada especial
 */
int device_release (struct inode *inode, struct file *filp) {
	/*pBuffer = NULL;*/
	return 0;
}

/**
 * Función que lee desde el dispositivo:
 *  - Verifica que el puntero este en una posición válida
 *  - Copia desde el dispositivo virtual al buffer entregado por el usuario
 * Sobre las estructuras de datos: http://www.tldp.org/LDP/tlk/ds/ds.html
 * @param filp Representación del archivo abierto
 * @param buff Apunta al espacio de direcciones del proceso (no es de fiar, para copiar usar put_user)
 * @param length Cantidad de bytes que se leeran
 * @param offset Desde que byte se leera
 * @return Cantidad de bytes leídos desde el dispositivo
 */
ssize_t device_read (struct file *filp, char *buff, size_t length, loff_t *offset) {
	int copied = 0;
	// si no hay más datos entonces es fin de archivo (EOF)
	if (!(*pBuffer)) return 0;
	// copiar
	while (length && *pBuffer)  {
		put_user(*(pBuffer++), buff++);
		length--;
		copied++;
	}
	printk(KERN_INFO "Se han leido %d bytes\n", copied);
	return copied;
}

/**
 * Función que escribe en el dispositivo
 * Copia desde el buffer entregado por el usuario al dispositivo (sobre escribiendo)
 * Sobre las estructuras de datos: http://www.tldp.org/LDP/tlk/ds/ds.html
 * @param filp Representación del archivo abierto
 * @param buff Apunta al espacio de direcciones del proceso
 * @param length Cantidad de bytes que se leeran
 * @param offset Desde que byte se leera
 * @return Cantidad de bytes escritos en el dispositivo
 */
ssize_t device_write (struct file *filp, const char *buff, size_t length, loff_t *offset) {
	int copied = 0;
	memset(buffer, 0, BUFFER_SIZE);
	// copiar
	while(length && copied<BUFFER_SIZE) {
		buffer[copied] = buff[copied];
		copied++;
		length--;
	}
	printk(KERN_INFO "Se han escrito %d bytes\n", copied);
	return copied;
}
