TAREA 3
El propósito de esta tarea fue mejorar el sistema operativo xv6, basado en la arquitectura RISC-V, implementando nuevas funcionalidades de protección de memoria y de ajuste de prioridad de procesos. Esto incluyó la creación de dos syscalls (mprotect y munprotect) para manejar los permisos de acceso a la memoria y una syscall adicional llamada setboost para modificar la prioridad de los procesos. La tarea incluyó varios pasos de implementación y pruebas, en los cuales se encontraron algunos errores que fueron corregidos para lograr un comportamiento consistente y esperado en el sistema.
2. Descripción de las Funciones Implementadas
2.1 Función mprotect
La syscall mprotect permite a un proceso marcar una región de memoria como de solo lectura, evitando que el proceso escriba en esa área. Esta funcionalidad es esencial para proteger áreas críticas de la memoria y prevenir que errores de programación en un proceso afecten la estabilidad del sistema.

    Argumentos:
        * addr: La dirección de inicio de la región de memoria que se desea proteger.
        * len: La longitud (en páginas) de la región que se quiere proteger.
2.2 Función munprotect
La syscall munprotect permite revertir la protección de solo lectura en una región de memoria, permitiendo nuevamente que el proceso escriba en esa área.

    Argumentos:
        *addr: La dirección de inicio de la región de memoria que se desea desproteger.
        *len: La longitud (en páginas) de la región que se quiere desproteger.

3. Cambios Realizados en el Código y su Propósito
Para implementar estas funciones, se realizaron varias modificaciones en diferentes archivos del sistema operativo xv6. A continuación, se describen cada una de estas modificaciones y el propósito de cada cambio

3.1 Modificaciones en syscall.h
Se agregaron identificadores únicos para las nuevas syscalls mprotect, munprotect, y setboost. En xv6, cada syscall necesita un número de identificación único para que el kernel pueda reconocerla. Estos valores son necesarios para que el sistema operativo pueda asociar cada nombre de syscall con el número de syscall correspondiente.

    #define SYS_mprotect 26
    #define SYS_munprotect 27

3.2 Modificaciones en syscall.c
Se declararon e integraron las funciones sys_mprotect, sys_munprotect, y sys_setboost en la tabla de syscalls del sistema operativo. Esto le permite al kernel saber qué función ejecutar en el espacio del kernel cuando se llama a cada una de estas syscalls desde el espacio de usuario.

extern int sys_mprotect(void);
extern int sys_munprotect(void);

[SYS_mprotect]   sys_mprotect,
[SYS_munprotect] sys_munprotect,

3.3 Implementación en sysproc.c
 (sys_mprotect, sys_munprotect, y sys_setboost) reciben los argumentos de usuario desde el espacio de usuario y los pasan a las funciones correspondientes en el kernel (mprotect, munprotect, y setboost). argaddr y argint se utilizan para extraer los argumentos que el usuario pasa en la syscall.

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

3.4 Implementación en vm.c
mprotect: Esta función marca una región de memoria como de solo lectura desactivando el bit PTE_W en las entradas de la tabla de páginas de la región especificada. Recarga el TLB con sfence_vma() para asegurar que los cambios se apliquen de inmediato.
munprotect: Rehabilita el permiso de escritura en la región de memoria especificada activando el bit PTE_W en cada entrada de la tabla de páginas en esa región.}}

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
        sfence_vma();
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
        sfence_vma();
        return 0;
    }

3.5 Modificaciones en usys.S
Se añaden las entradas mprotect, munprotect, y setboost en usys.S, lo cual permite que estas funciones sean llamadas desde el espacio de usuario. La instrucción ecall realiza la llamada al sistema, y el número de syscall se coloca en el registro a7. Esto establece la conexión entre el espacio de usuario y el espacio del kernel para estas funciones.

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



4. Errores Encontrados y Soluciones Aplicadas

	a.Error de Redefinición de PTE_W:

    		El sistema arrojó un error de redefinición porque PTE_W estaba definido en más de un archivo. La solución fue mantener la definición en un solo                   archivo (riscv.h) y eliminar cualquier redefinición.

	b.Problemas con argptr y argint:

    		Al implementar sys_mprotect y sys_munprotect, xv6 arrojó errores porque argptr y argint no estaban funcionando correctamente en RISC-V. La solución               fue usar argaddr para obtener las direcciones correctamente.

	c.Falta de Alineación en mprotect y munprotect:

    		Inicialmente, no se estaba alineando addr al tamaño de página, lo cual provocó errores. La solución fue utilizar PGROUNDDOWN para alinear la                      dirección de inicio.

	d.Falta de Alineación en mprotect y munprotect:

    		Inicialmente, no se estaba alineando addr al tamaño de página, lo cual provocó errores. La solución fue utilizar PGROUNDDOWN para alinear la                      dirección de inicio.


