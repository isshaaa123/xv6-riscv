TAREA 3


1.Se incluyeron las funciones sys_mprotect y sys_munprotect en la tabla de llamadas al sistema para que el sistema operativo las reconozca:

extern int sys_mprotect(void);
extern int sys_munprotect(void);

[SYS_mprotect]   sys_mprotect,
[SYS_munprotect] sys_munprotect,

2.Implementación de sys_mprotect y sys_munprotect en sysproc.c

Estas funciones obtienen los argumentos de las llamadas de sistema desde el espacio de usuario y llaman a mprotect y munprotect respectivamente.

int sys_mprotect(void) {
    uint64 addr;
    int len;

    argaddr(0, &addr);
    argint(1, &len);

    return mprotect((void *)addr, len);
}

int sys_munprotect(void) {
    uint64 addr;
    int len;

    argaddr(0, &addr);
    argint(1, &len);

    return munprotect((void *)addr, len);
}


3.Implementación de mprotect y munprotect en vm.c

Aquí se implementa la lógica principal de protección y desprotección de memoria. Las funciones desactivan o activan el bit PTE_W en la tabla de páginas y recargan el TLB para que los cambios tengan efecto inmediato:

int mprotect(void *addr, int len) {
    if (addr == 0 || len <= 0) return -1;
    uint64 addr_aligned = PGROUNDDOWN((uint64) addr);
    uint64 end_addr = (uint64) addr + len * PGSIZE;

    for (uint64 a = addr_aligned; a < end_addr; a += PGSIZE) {
        pte_t *pte = walk(myproc()->pagetable, a, 0);
        if (!pte || (*pte & PTE_V) == 0)
            return -1;
        *pte &= ~PTE_W;
    }
    sfence_vma();  // Recargar el TLB
    return 0;
}

int munprotect(void *addr, int len) {
    if (addr == 0 || len <= 0) return -1;
    uint64 addr_aligned = PGROUNDDOWN((uint64) addr);
    uint64 end_addr = (uint64) addr + len * PGSIZE;

    for (uint64 a = addr_aligned; a < end_addr; a += PGSIZE) {
        pte_t *pte = walk(myproc()->pagetable, a, 0);
        if (!pte || (*pte & PTE_V) == 0)
            return -1;
        *pte |= PTE_W;
    }
    sfence_vma();  // Recargar el TLB
    return 0;
}

4.Modificaciones en usys.S

Se añadieron mprotect y munprotect a usys.S para permitir que estas funciones puedan ser llamadas desde el espacio de usuario:

.global mprotect
mprotect:
    li a7, SYS_mprotect
    ecall
    ret

.global munprotect
munprotect:
    li a7, SYS_munprotect
    ecall
    ret

5.Para registrar estas llamadas en el archivo que contiene entry() (por lo general en el archivo que define todas las syscalls), el código incluiría líneas como las siguientes:

entry("setboost");
entry("mprotect");
entry("munprotect");


6. Errores 
	a.Error de Redefinición de PTE_W:

    		El sistema arrojó un error de redefinición porque PTE_W estaba definido en más de un archivo. La solución fue mantener la definición en un solo archivo (riscv.h) y eliminar cualquier redefinición.

	b.Problemas con argptr y argint:

    		Al implementar sys_mprotect y sys_munprotect, xv6 arrojó errores porque argptr y argint no estaban funcionando correctamente en RISC-V. La solución fue usar argaddr para obtener las direcciones correctamente.

	c.Falta de Alineación en mprotect y munprotect:

    		Inicialmente, no se estaba alineando addr al tamaño de página, lo cual provocó errores. La solución fue utilizar PGROUNDDOWN para alinear la dirección de inicio.

	d.Falta de Alineación en mprotect y munprotect:

    		Inicialmente, no se estaba alineando addr al tamaño de página, lo cual provocó errores. La solución fue utilizar PGROUNDDOWN para alinear la dirección de inicio.

	e.Falta de Recarga del TLB (sfence_vma):

    		Sin sfence_vma(), los cambios en los permisos de las páginas no tenían efecto inmediato, lo que generaba comportamientos inconsistentes. Agregar sfence_vma() al final de las funciones resolvió el problema.
