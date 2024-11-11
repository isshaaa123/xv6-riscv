#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
#include "../kernel/memlayout.h"
#define PGSIZE 4096


int main() {
    char *addr = (char *)((uint64)sbrk(4096) & ~(PGSIZE - 1));  // Alinear al tamaño de página
    sbrk(4096);  // Reservar una página

    // Intentar proteger la nueva página
    if (mprotect(addr, 1) == -1) {
        printf("mprotect falló\n");
        exit(1);
    }

    // Intentar escribir en la página protegida
    char *ptr = addr;
    *ptr = 'A';  // Esto debería fallar si la protección es exitosa
    printf("Valor en la dirección: %c\n", *ptr);  // Verificar el valor

    // Revertir la protección
    if (munprotect(addr, 1) == -1) {
        printf("munprotect falló\n");
        exit(1);
    }

    // Intentar escribir de nuevo
    *ptr = 'B';  // Esto debería tener éxito si la protección se revirtió
    printf("Valor en la dirección: %c\n", *ptr);

    exit(0);
}
