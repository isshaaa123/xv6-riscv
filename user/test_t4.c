#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    char *filename = "testfile";
    int fd;

    // Crear archivo con permisos de lectura/escritura
    fd = open(filename, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf("Error al crear el archivo.\n");
        exit(1);
    }
    printf("Archivo creado.\n");


    // Cambiar permisos a solo lectura
    if (chmod(filename, 1) < 0) {
        printf("Error al cambiar permisos a solo lectura.\n");
        exit(1);
    }
    printf("Permisos cambiados a solo lectura.\n");

    // Intentar escribir con permisos de solo lectura
    fd = open(filename, O_WRONLY);
    if (fd >= 0) {
        if (write(fd, "Test", 4) == -1) {
            printf("Error esperado: no se puede escribir en el archivo.\n");
        } else {
            printf("Error: se pudo escribir en el archivo con solo lectura.\n");
        }
        close(fd);
    } else {
        printf("Error esperado al abrir el archivo en modo escritura.\n");
    }

    // Restaurar permisos a lectura/escritura
    if (chmod(filename, 3) < 0) {
        printf("Error al restaurar permisos.\n");
        exit(1);
    }
    printf("Permisos restaurados a lectura/escritura.\n");

    // Escritura final
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        printf("Error al abrir el archivo para escritura final.\n");
        exit(1);
    }
    if (write(fd, "Final Test", 10) != 10) {
        printf("Error al escribir en el archivo tras restaurar permisos.\n");
        close(fd);
        exit(1);
    }
    printf("Escritura final exitosa.\n");
    close(fd);

    exit(0);
}

