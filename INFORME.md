Objetivos

  1.Agregar permisos a los archivos:
    Crear un sistema para controlar si un archivo puede ser leído, escrito, ambos, o marcado como inmutable.
  2.Respetar los permisos en operaciones:
    Asegurar que abrir, leer o escribir un archivo solo sea posible si tiene los permisos adecuados.
  3.Agregar la función chmod:
    Permitir cambiar los permisos de un archivo con una nueva llamada al sistema.
  4.Agregar el permiso especial "inmutable":
    Hacer que un archivo marcado como inmutable no pueda ser modificado, ni siquiera sus permisos.
  5.Probar la funcionalidad:
    Crear un programa que demuestre que los permisos funcionan como se espera.

    
Lógica de Implementación

  1.Modificación del inodo:
    Se agregó un campo llamado perm en la estructura del inodo para guardar los permisos.
    Los valores posibles son:
      0: Sin permisos (ni lectura ni escritura).
      1: Solo lectura.
      2: Solo escritura.
      3: Lectura y escritura (por defecto).
      5: Inmutable (solo lectura y no se pueden cambiar los permisos).
      
  2.Cambios en las funciones del sistema:
    Abrir archivos (open): Si un archivo es solo de lectura, no se puede abrir en modo escritura.
    Leer y escribir (read y write): Se verifica si el archivo tiene permisos para estas operaciones antes de realizarlas.
    
  3.Nueva función chmod:
    Permite cambiar los permisos de un archivo, excepto si está marcado como inmutable (perm == 5).
    
  4.Programa de pruebas:

    Se creó un programa (test_t4) para probar todas las funciones:
    Crear un archivo.
    Cambiar permisos.
    Probar que los permisos de solo lectura y los inmutables funcionan correctamente.
    
Cambios Realizados
    1.Estructura del inodo:
      Se agregó el campo perm en el inodo y se inicializó a 3 (lectura y escritura) al crear un archivo.
    
    2.Funciones modificadas:
      sys_open: Verifica los permisos antes de abrir un archivo.
      sys_write: Bloquea la escritura si el archivo no tiene permisos.
      sys_read: Solo permite leer si el archivo tiene permisos de lectura.
      chmod: Cambia los permisos del archivo.
      
    3.Programa test_t4:
      Valida las siguientes situaciones:
        Cambio de permisos de lectura/escritura.
        Bloqueo al intentar escribir en un archivo de solo lectura.
        Bloqueo de modificaciones en archivos marcados como inmutables.
        
Dificultades Encontradas
    1.Ajustes en funciones existentes:
        Asegurarse de que las funciones de apertura, lectura y escritura respeten los nuevos permisos sin romper otras partes del sistema.
    
    2.Manejo del estado inmutable:
      Diseñar la lógica para que un archivo inmutable no permita modificaciones, incluso en sus permisos.

    3.Validación de entradas:
      Asegurar que los valores dados a chmod sean válidos y no generen errores.
    
    4.Pruebas completas:
      Crear escenarios que cubran todos los posibles casos de permisos.

Cómo Ejecutar el Programa de Prueba
      make clean
      make qemu
      $ test_t4
      
Resultados esperados:
  El programa imprimirá mensajes indicando si cada paso se realizó correctamente:
  Creación y escritura inicial.
  Bloqueo de escritura en archivos de solo lectura.
  Restauración de permisos.
  Bloqueo al intentar modificar archivos inmutables.











ChatGPT puede c
