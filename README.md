Es fundamental entender a fondo qué hace una herramienta como `nm` antes de intentar recrearla.

El comando `nm` es una utilidad de línea de comandos en sistemas tipo Unix que se utiliza para **inspeccionar los archivos objeto, librerías estáticas y librerías compartidas (ejecutables) para listar los símbolos que contienen.** Estos símbolos son los nombres de funciones y variables que un programa usa o define.

Vamos a explicar paso a paso qué ocurre cuando ejecutas `nm` con uno o más argumentos:

### **Qué hace `nm` (Proceso Interno Detallado para cada Archivo)**

Cuando `nm` recibe el nombre de un archivo (o nombres de varios archivos), para **cada archivo individualmente**, realiza el siguiente proceso:

1.  **Paso 1: Recepción del Argumento (Ruta del Archivo)**
    * `nm` recibe la ruta completa o relativa del archivo que debe analizar como un argumento de la línea de comandos.

2.  **Paso 2: Apertura y Validación Básica del Archivo**
    * Intenta abrir el archivo especificado en modo de lectura.
    * Verifica si el archivo existe y si tiene los permisos de lectura necesarios. Si no, `nm` reporta un error (ej. "No such file or directory" o "Permission denied") y pasa al siguiente archivo si hay más.

3.  **Paso 3: Identificación del Formato del Archivo (Magic Bytes / ELF Header)**
    * Lee los primeros bytes del archivo. Estos son los "números mágicos" (magic bytes) que indican el tipo de formato del archivo.
    * En sistemas Linux, `nm` espera principalmente encontrar archivos en formato **ELF (Executable and Linkable Format)**.
    * Verifica los bytes iniciales del ELF Header para confirmar que es un archivo ELF válido.
    * A partir del ELF Header, determina la **clase del ELF** (si es un binario de 32 bits o de 64 bits) y la **arquitectura de la máquina** para la que fue compilado (ej. x86, ARM).
    * Si el archivo no es un formato reconocido (ej. es un archivo de texto, una imagen, o un formato binario diferente), `nm` informa de un error (ej. "file format not recognized") y pasa al siguiente archivo.

4.  **Paso 4: Mapeo del Archivo en Memoria (`mmap`)**
    * Una vez que se confirma que es un archivo ELF, `nm` utiliza la llamada al sistema `mmap()` para mapear todo el contenido del archivo directamente en la memoria virtual de su propio proceso.
    * Esto es crucial para el rendimiento, ya que permite acceder a cualquier parte del archivo como si fuera un array en memoria, sin tener que hacer múltiples lecturas de disco.

5.  **Paso 5: Parseo del Encabezado ELF Principal**
    * Accede al ELF Header mapeado en memoria.
    * Lee información clave como:
        * `e_shoff`: El offset (desplazamiento) dentro del archivo donde comienza la Tabla de Encabezados de Sección (Section Header Table - SHT).
        * `e_shnum`: El número de entradas en la SHT.
        * `e_shentsize`: El tamaño de cada entrada en la SHT.
        * `e_shstrndx`: El índice de la sección dentro de la SHT que contiene los nombres de las secciones (Section Header String Table).

6.  **Paso 6: Localización de la Tabla de Encabezados de Sección (SHT)**
    * Utiliza `e_shoff` para navegar directamente a la SHT en la memoria mapeada.
    * La SHT es un array de estructuras `Elf_Shdr` (o `Elf64_Shdr` para 64 bits, `Elf32_Shdr` para 32 bits), cada una describiendo una sección lógica del binario.

7.  **Paso 7: Localización de la Tabla de Nombres de Secciones (`.shstrtab`)**
    * Usando `e_shstrndx`, `nm` identifica cuál entrada de la SHT corresponde a la tabla de cadenas de nombres de secciones (`.shstrtab`).
    * Obtiene el offset y el tamaño de `.shstrtab` de su entrada `Elf_Shdr`. Esta tabla es fundamental porque contiene los nombres de todas las demás secciones (como `.symtab`, `.strtab`, `.text`, `.data`), lo que permite a `nm` encontrarlas por su nombre.

8.  **Paso 8: Localización de las Tablas de Símbolos (`.symtab`, `.dynsym`) y Cadenas Asociadas (`.strtab`, `.dynstr`)**
    * `nm` itera a través de cada entrada en la SHT (usando `e_shnum` y `e_shentsize`).
    * Para cada entrada, usa el offset del nombre de la sección (`sh_name`) en `.shstrtab` para obtener el nombre real de la sección.
    * Busca secciones con nombres como:
        * **`.symtab`:** La tabla de símbolos principal, que contiene la mayoría de los símbolos estáticos y globales.
        * **`.strtab`:** La tabla de cadenas asociada a `.symtab`, donde se almacenan los nombres reales de los símbolos.
        * **`.dynsym`:** Una tabla de símbolos para símbolos dinámicos (usados para enlace en tiempo de ejecución, como funciones de librerías compartidas).
        * **`.dynstr`:** La tabla de cadenas asociada a `.dynsym`.
    * Cuando encuentra estas secciones, guarda sus offsets, tamaños y tipos (`SHT_SYMTAB`, `SHT_STRTAB`).

9.  **Paso 9: Parseo de las Entradas de la Tabla de Símbolos**
    * Para cada tabla de símbolos encontrada (`.symtab` y `.dynsym`), `nm` itera sobre sus entradas. Cada entrada es una estructura `Elf_Sym` (o `Elf64_Sym` / `Elf32_Sym`).
    * Calcula el número de símbolos dividiendo el tamaño de la sección de símbolos por el tamaño de una entrada `Elf_Sym`.

10. **Paso 10: Extracción y Determinación del Tipo de Símbolo**
    * Para cada `Elf_Sym` en la tabla de símbolos:
        * **Nombre del Símbolo:** Accede a la tabla de cadenas de símbolos (`.strtab` o `.dynstr`) usando el offset `st_name` del símbolo para obtener su nombre legible.
        * **Valor/Dirección del Símbolo:** Obtiene la dirección o valor del símbolo desde el campo `st_value`.
        * **Tipo del Símbolo (el carácter `T`, `D`, `U`, etc.):** Esta es la parte más compleja. El tipo de símbolo se deriva de una combinación de factores, principalmente:
            * **`st_info`:** Este campo contiene dos piezas de información:
                * **`ELF_ST_TYPE(st_info)`:** Indica la categoría del símbolo (función, objeto de datos, sección, etc.).
                * **`ELF_ST_BIND(st_info)`:** Indica la vinculación del símbolo (local, global, débil).
            * **`st_shndx`:** El índice de la sección a la que se refiere el símbolo. Valores especiales como `SHN_UNDEF` (indefinido, el símbolo se usa pero no se define aquí), `SHN_ABS` (valor absoluto), `SHN_COMMON` (símbolo "común" no asignado).
            * `nm` tiene una lógica interna compleja para mapear estas combinaciones a un solo carácter (ej. `'T'` para funciones globales en la sección de texto, `'t'` para funciones locales, `'D'` para datos globales inicializados, `'U'` para indefinidos, `'W'` para símbolos débiles, etc.).
            * (Opcional, pero relevante para nm completo): Considera también el tamaño del símbolo (`st_size`).

11. **Paso 11: Filtrado y Recolección de Símbolos Relevantes**
    * `nm` ignora ciertos símbolos (ej. símbolos de depuración, símbolos de sección interna sin interés para el usuario, algunos símbolos locales si no se usa la opción `-a`). Los símbolos que cumplen los criterios se añaden a una lista interna.

12. **Paso 12: Ordenamiento de Símbolos**
    * La lista recolectada de símbolos se ordena. El criterio principal es el **orden alfabético por el nombre del símbolo**.
    * Si dos símbolos tienen el mismo nombre, se aplican criterios secundarios de desempate (ej. por valor/dirección del símbolo, por tipo de símbolo).

13. **Paso 13: Formateo e Impresión de la Salida**
    * `nm` recorre la lista de símbolos ya ordenada y los imprime en la salida estándar (`stdout`) en un formato específico:
        * **Dirección:** Un valor hexadecimal (rellenado con ceros a la izquierda si es necesario para la arquitectura de 32 o 64 bits).
        * **Tipo de Símbolo:** El carácter simple (ej. `T`, `D`, `U`).
        * **Nombre del Símbolo:** La cadena legible.
    * Añade espacios o padding para alinear las columnas y mejorar la legibilidad.

14. **Paso 14: Limpieza de Memoria y Cierre del Archivo**
    * Una vez que se han procesado e impreso todos los símbolos del archivo, `nm` libera la memoria mapeada (`munmap()`) y cierra el descriptor de archivo (`close()`).

### **Comportamiento con Múltiples Argumentos (`nm file1 file2 ...`)**

Si `nm` se invoca con más de un argumento de archivo, repite el **proceso completo descrito anteriormente para cada archivo en el orden en que fueron dados**.

* **Encabezado por Archivo:** Antes de listar los símbolos de un nuevo archivo (si no es el primero), `nm` suele imprimir un encabezado que indica el nombre del archivo que se está procesando actualmente, por ejemplo:
    ```
    file1.o:
    (lista de símbolos de file1.o)

    file2.so:
    (lista de símbolos de file2.so)
    ```
* **Independencia de Errores:** Si un archivo falla en su procesamiento (ej. no existe, formato no reconocido), `nm` reporta el error para ese archivo en particular, pero **continúa intentando procesar los siguientes archivos** en la lista de argumentos. No se detiene en el primer error.

En resumen, `nm` es una herramienta vital para la introspección de binarios, y su funcionalidad, aunque parece sencilla desde fuera, implica un conocimiento profundo de la estructura de archivos ELF y un manejo robusto de la memoria y el sistema de archivos.

---

## 📚 Estructura de un Archivo ELF: Una Guía Detallada

Un archivo ELF (Executable and Linkable Format) es el formato estándar para ejecutables, bibliotecas compartidas, archivos de objetos y volcados de memoria (`core dumps`) en sistemas Unix-like (como Linux). Es una estructura altamente organizada que permite al sistema operativo cargar y ejecutar programas, o al enlazador combinar diferentes piezas de código.

Un archivo ELF se compone de varias partes principales:

### 1. Cabecera ELF (`ELF Header`) 📖

Es lo primero en cualquier archivo ELF y describe la estructura general del archivo. Es como la **tarjeta de identificación** del archivo. Contiene campos cruciales para interpretar el resto del archivo.

**Información clave para `ft_nm`:**

* **Magic Number (`e_ident`):** Los primeros 4 bytes son `0x7F 'E' 'L' 'F'`, que identifican el archivo como ELF. ¡Ya lo estás comprobando!
* **Clase de Arquitectura (`e_ident[EI_CLASS]`):** Indica si el ELF es de 32 bits (`ELFCLASS32`) o 64 bits (`ELFCLASS64`). ¡Ya lo estás detectando con `find_bits`!
* **Endianness (`e_ident[EI_DATA]`):** Especifica el orden de bytes (little-endian o big-endian). ¡Ya lo estás detectando con `find_endianness`! Es crucial para leer correctamente valores de múltiples bytes (direcciones, offsets, tamaños).
* **Versión ELF (`e_ident[EI_VERSION]`):** La versión del formato ELF.
* **Tipo de Objeto (`e_type`):**
    * `ET_REL`: Archivo reubicable (objeto, `.o`).
    * `ET_EXEC`: Archivo ejecutable.
    * `ET_DYN`: Biblioteca compartida (`.so`) o ejecutable cargado dinámicamente.
    * `ET_CORE`: Volcado de memoria.
* **Arquitectura de la Máquina (`e_machine`):** Indica la arquitectura del procesador (ej. `EM_X86_64` para x86-64, `EM_386` para i386, `EM_AARCH64` para ARM64).
* **Versión (`e_version`):** Debe ser `EV_CURRENT`.
* **Dirección de Entrada (`e_entry`):** Para ejecutables, es la dirección de memoria virtual donde el sistema operativo debe comenzar la ejecución.
* **Offset de la Tabla de Cabeceras de Programa (`e_phoff`):** La posición dentro del archivo donde comienza la Tabla de Cabeceras de Programa.
* **Offset de la Tabla de Cabeceras de Sección (`e_shoff`):** La posición dentro del archivo donde comienza la Tabla de Cabeceras de Sección.
* **Flags (`e_flags`):** Flags específicos de la arquitectura.
* **Tamaño de la Cabecera ELF (`e_ehsize`):** Tamaño de esta cabecera en bytes.
* **Tamaño de las Entradas de la Cabecera de Programa (`e_phentsize`):** Tamaño de una entrada en la Tabla de Cabeceras de Programa.
* **Número de Entradas de la Cabecera de Programa (`e_phnum`):** Número de entradas en la Tabla de Cabeceras de Programa.
* **Tamaño de las Entradas de la Cabecera de Sección (`e_shentsize`):** Tamaño de una entrada en la Tabla de Cabeceras de Sección.
* **Número de Entradas de la Cabecera de Sección (`e_shnum`):** Número de entradas en la Tabla de Cabeceras de Sección.
* **Índice de la Sección de Nombres (`e_shstrndx`):** El índice de la entrada en la Tabla de Cabeceras de Sección que contiene los nombres de las secciones.

---

### 2. Tabla de Cabeceras de Programa (`Program Header Table` o `Segment Header Table`) 🚀

Esta tabla es crucial para el **cargador de programas** del sistema operativo. Describe cómo el archivo debe ser mapeado en la memoria en tiempo de ejecución. Cada entrada en esta tabla se llama **Program Header** o **Segment Descriptor**.

**Información clave para `ft_nm`:** (Aunque `nm` se enfoca más en las secciones, entender los segmentos es útil para el contexto).

* **Tipo de Segmento (`p_type`):** Indica el propósito del segmento (ej., `PT_LOAD` para segmentos cargables, `PT_DYNAMIC` para información de enlace dinámico).
* **Offset de Archivo (`p_offset`):** Offset del segmento dentro del archivo ELF.
* **Dirección Virtual (`p_vaddr`):** Dirección virtual donde se cargará el segmento en memoria.
* **Tamaño en Archivo (`p_filesz`):** Tamaño del segmento en el archivo.
* **Tamaño en Memoria (`p_memsz`):** Tamaño del segmento en memoria (puede ser mayor que `p_filesz` si incluye datos inicializados a cero).
* **Flags (`p_flags`):** Permisos de lectura/escritura/ejecución (`PF_R`, `PF_W`, `PF_X`).
* **Alineación (`p_align`):** Requisito de alineación del segmento en memoria.

---

### 3. Tabla de Cabeceras de Sección (`Section Header Table`) 🧩

Esta tabla describe las secciones del archivo. Las **secciones** son unidades más pequeñas y lógicas dentro de los segmentos, utilizadas por el enlazador y las herramientas de depuración. Aquí es donde `nm` busca la mayoría de su información.

**Información clave para `ft_nm`:**

Cada entrada en esta tabla es un **Section Header** y contiene:

* **Nombre de la Sección (`sh_name`):** Un offset al string que contiene el nombre de la sección en la sección de strings de los nombres de sección (`.shstrtab`).
    * **Necesitas encontrar la sección `.shstrtab`** para resolver estos nombres.
* **Tipo de Sección (`sh_type`):** El propósito de la sección (ej., `SHT_PROGBITS` para código o datos, `SHT_SYMTAB` para la tabla de símbolos, `SHT_STRTAB` para la tabla de strings).
* **Flags (`sh_flags`):** Atributos de la sección (ej., `SHF_ALLOC` si se carga en memoria, `SHF_EXECINSTR` si contiene código ejecutable, `SHF_WRITE` si es escribible).
* **Dirección Virtual (`sh_addr`):** Dirección virtual de la sección si se carga en memoria.
* **Offset de Archivo (`sh_offset`):** La posición de la sección dentro del archivo ELF.
* **Tamaño de la Sección (`sh_size`):** El tamaño de la sección en bytes.
* **Enlace (`sh_link`):** Índice de otra sección relacionada (ej., para una tabla de símbolos, esto puede apuntar a su tabla de strings asociada).
* **Información (`sh_info`):** Información adicional específica del tipo de sección.
* **Alineación (`sh_addralign`):** Requisito de alineación de la sección en memoria.
* **Tamaño de Entrada (`sh_entsize`):** Si la sección contiene entradas de tamaño fijo (como la tabla de símbolos), este es el tamaño de cada entrada.

---

### 4. Secciones Importantes para `ft_nm` 🎯

Dentro de la Tabla de Cabeceras de Sección, hay algunas secciones específicas que `nm` necesita analizar:

* **`.symtab` (Symbol Table Section):**
    * Contiene la **tabla de símbolos** del archivo. Cada entrada es una estructura `Elf32_Sym` o `Elf64_Sym`.
    * **Necesitas:**
        * Su `sh_offset` para saber dónde empieza en el archivo.
        * Su `sh_size` para saber cuántos bytes ocupa.
        * Su `sh_entsize` para saber el tamaño de cada entrada de símbolo.
        * Su `sh_link` para encontrar la tabla de strings de símbolos (`.strtab`) asociada.

* **`.strtab` (String Table Section):**
    * Contiene las **cadenas de caracteres** (nombres) de los símbolos listados en `.symtab`. Los símbolos en `.symtab` solo almacenan un offset a esta sección para su nombre.
    * **Necesitas:**
        * Su `sh_offset` para saber dónde empieza.
        * Su `sh_size` para saber su tamaño.

* **`.shstrtab` (Section Header String Table Section):**
    * Contiene los **nombres de todas las secciones** del archivo. La cabecera ELF (`e_shstrndx`) apunta a esta sección.
    * **Necesitas:**
        * Su `sh_offset` para saber dónde empieza.
        * Su `sh_size` para saber su tamaño.

---

### 5. Entradas de la Tabla de Símbolos (`Symbol Table Entry`) 🔖

Cada entrada en `.symtab` es un **símbolo**. Aquí es donde obtendrás la información que `nm` imprime.

**Información clave para `ft_nm`:**

Para cada `Elf32_Sym` o `Elf64_Sym` (la estructura depende de si el ELF es de 32 o 64 bits):

* **`st_name`:** Un offset a la cadena de caracteres que es el nombre del símbolo, dentro de la sección `.strtab` asociada.
* **`st_value`:** El valor del símbolo. Puede ser una dirección, un tamaño, o cualquier otra cosa dependiendo del contexto. Para la mayoría de los símbolos, es la dirección del símbolo.
* **`st_size`:** El tamaño del símbolo.
* **`st_info`:** Contiene el **tipo** y el **binding** del símbolo.
    * `ELF32_ST_BIND(st_info)` / `ELF64_ST_BIND(st_info)`: Extrae el binding (ej., `STB_LOCAL`, `STB_GLOBAL`, `STB_WEAK`).
    * `ELF32_ST_TYPE(st_info)` / `ELF64_ST_TYPE(st_info)`: Extrae el tipo (ej., `STT_NOTYPE`, `STT_OBJECT`, `STT_FUNC`, `STT_SECTION`, `STT_FILE`).
* **`st_other`:** Usado para visibilidad (ej., `STV_DEFAULT`, `STV_HIDDEN`).
* **`st_shndx`:** El índice de la sección a la que se refiere el símbolo. Por ejemplo, si es una función, este es el índice de la sección de código donde reside. Puede ser un valor especial como `SHN_ABS` (valor absoluto), `SHN_COMMON` (símbolo común) o `SHN_UNDEF` (símbolo indefinido, es decir, externo).

---

## Guía Clara para Construir tu `ft_nm` (Flujo de Información) 🗺️

Basado en lo que ya tienes y la información del ELF, aquí está la secuencia lógica para construir tu `nm`:

1.  **Lectura del Archivo y Mapeo en Memoria (¡Ya lo haces!):**
    * Abre el archivo.
    * Obtén su tamaño (`fstat`).
    * Mapea el archivo en memoria (`mmap`).
    * Verifica la firma ELF (`0x7F 'E' 'L' 'F'`).
    * Detecta la **clase (32/64 bits)** y el **endianness**. (¡Ya lo tienes!)

2.  **Parsear la Cabecera ELF (`Elf32_Ehdr` o `Elf64_Ehdr`):**
    * Dependiendo de `aux->bits`, castear `aux->file_content_ptr` a `Elf32_Ehdr *` o `Elf64_Ehdr *`. Almacenar esto en `aux->elf32_header` o `aux->elf64_header`.
    * Esta cabecera te dará los offsets y números de entradas para las Tablas de Cabeceras de Programa y Sección.
    * **Crucial:** El `e_shoff` (offset a la tabla de cabeceras de sección) y `e_shnum` (número de secciones) serán vitales. También `e_shstrndx` (índice de la sección `.shstrtab`).

3.  **Localizar la Tabla de Cabeceras de Sección:**
    * Usa `e_shoff` de la cabecera ELF para encontrar el inicio de la Tabla de Cabeceras de Sección en la memoria mapeada.
    * Necesitarás un puntero a `Elf32_Shdr` o `Elf64_Shdr` (dependiendo de `aux->bits`).

4.  **Localizar la Sección de Nombres de Sección (`.shstrtab`):**
    * Usa el índice `e_shstrndx` de la Cabecera ELF para acceder a la entrada correspondiente en la Tabla de Cabeceras de Sección.
    * De esa entrada (la que describe `.shstrtab`), obtén su `sh_offset` y `sh_size`.
    * Esto te dará la dirección y tamaño de la sección que contiene los nombres de todas las demás secciones. Esto es esencial para **nombrar las secciones**.

5.  **Iterar a través de la Tabla de Cabeceras de Sección:**
    * Recorre las `e_shnum` entradas de la Tabla de Cabeceras de Sección.
    * Para cada entrada, necesitas:
        * Obtener su **nombre** usando `sh_name` como offset en la sección `.shstrtab` que acabas de localizar.
        * Buscar secciones con tipo `SHT_SYMTAB` y `SHT_STRTAB` (para la tabla de símbolos y su tabla de strings asociada, respectivamente).
        * Registrar sus offsets, tamaños y enlaces (`sh_link`).

6.  **Localizar la Tabla de Símbolos (`.symtab`) y su Tabla de Strings (`.strtab`):**
    * Una vez que has iterado por las cabeceras de sección, identifica la sección `SHT_SYMTAB`. Esta es tu tabla de símbolos.
    * Usa su campo `sh_link` para encontrar el índice de la sección `SHT_STRTAB` asociada (su tabla de strings de símbolos).
    * Obtén los offsets y tamaños de estas dos secciones.

7.  **Parsear las Entradas de la Tabla de Símbolos:**
    * Usa el `sh_offset` de `.symtab` y el `sh_entsize` de `.symtab` para iterar sobre cada `Elf32_Sym` o `Elf64_Sym` en la memoria mapeada.
    * Para cada símbolo:
        * Obtén su **nombre** usando `st_name` como offset en la sección `.strtab` que acabas de localizar.
        * Extrae el **tipo de símbolo** y el **binding** de `st_info` (usando las macros `ELF32_ST_TYPE`, `ELF32_ST_BIND` o sus equivalentes para 64 bits).
        * Obtén su **valor** de `st_value`.
        * Identifica su **sección** usando `st_shndx`.

8.  **Determinar el Tipo de Símbolo (el carácter que muestra `nm`):**
    * Esta es la parte más compleja. El carácter (T, t, D, d, B, b, U, W, etc.) depende de una combinación de:
        * **`st_shndx`**: Si es `SHN_UNDEF` (U), `SHN_ABS` (A), `SHN_COMMON` (C).
        * **`st_type`**: Si es `STT_FUNC` (T o t), `STT_OBJECT` (D, d, B, b), `STT_SECTION` (S o s).
        * **`st_bind`**: Si es `STB_GLOBAL` (mayúscula) o `STB_LOCAL` (minúscula), `STB_WEAK` (W o w).
        * **Flags de la sección referenciada**: Si la sección es escribible (`SHF_WRITE`), ejecutable (`SHF_EXECINSTR`), etc. Por ejemplo, `.text` es T, `.data` es D, `.bss` es B.
        * **Visibilidad (`st_other`):** Afecta si un símbolo débil es `W` o `w`.

9.  **Filtrar y Ordenar Símbolos (para las flags de `nm`):**
    * Aplica las reglas de las flags que soportes (`-a`, `-g`, `-u`, `-r`, `-p`).
    * Ordena los símbolos. Por defecto, `nm` los ordena alfabéticamente. La flag `-r` los invierte.

10. **Imprimir la Salida:**
    * Formatea la salida según las convenciones de `nm` (dirección, tipo de símbolo, nombre).

---

---

¡Excelente! Esta es la parte más desafiante y central para replicar el comportamiento de `nm`. Aquí es donde tomas toda la información que has parseado sobre un símbolo y determinas el carácter que lo representa en la salida de `nm`. Vamos a desglosar cada componente y cómo se interrelacionan.

---

## La Lógica para Determinar el `char_type` de un Símbolo

El `char_type` (el carácter que ves en la segunda columna de la salida de `nm`, como `T`, `U`, `D`, `B`, etc.) no se lee directamente del archivo ELF. Se **deriva** de una combinación de información sobre el símbolo y la sección a la que pertenece.

`nm` utiliza una serie de reglas y prioridades para asignar este carácter. Si un símbolo cumple múltiples condiciones, se aplica la regla de mayor prioridad.

Aquí están los elementos clave que influyen en el `char_type`, y cómo se usan:

### 1. `symbol_binding` (STB_LOCAL, STB_GLOBAL, STB_WEAK)

Este valor indica la **visibilidad o el "enlace" del símbolo**. Lo extraes del campo `st_info` del símbolo usando las macros `ELFXX_ST_BIND`.

* **`STB_GLOBAL` (Global):** Son símbolos visibles fuera de su unidad de compilación (archivo `.o` o biblioteca). Generalmente se representan con una letra **mayúscula** en la salida de `nm` (ej. `T`, `D`, `B`).
* **`STB_LOCAL` (Local):** Son símbolos visibles solo dentro de su unidad de compilación. Se representan con una letra **minúscula** (ej. `t`, `d`, `b`).
* **`STB_WEAK` (Débil):** Son símbolos que pueden ser sobrescritos por un símbolo global con el mismo nombre en otro archivo.
    * Si un símbolo débil no está definido (su `st_shndx` es `SHN_UNDEF`), se representa con `w`.
    * Si está definido, se representa con `W`.
* **Otros *bindings*:** Hay otros como `STB_GNU_UNIQUE`, `STB_LOPROC`, `STB_HIPROC`, pero para un `ft_nm` básico, los tres anteriores son los más relevantes.

**Dependencia:** El `symbol_binding` es crucial para decidir si el carácter final es **mayúscula o minúscula**. Si es `STB_GLOBAL` o `STB_WEAK` (y definido), será mayúscula. Si es `STB_LOCAL`, será minúscula (con algunas excepciones como `U` o `w` que siempre son minúsculas por su naturaleza de "no definido").

### 2. `symbol_type` (STT_NOTYPE, STT_OBJECT, STT_FUNC, STT_SECTION, STT_FILE, etc.)

Este valor describe la **naturaleza del símbolo** (qué representa). Lo extraes también del campo `st_info` usando las macros `ELFXX_ST_TYPE`.

* **`STT_NOTYPE`:** Tipo no especificado. `nm` a menudo lo representa con `?` o, si la sección lo permite, como `U` si es indefinido.
* **`STT_OBJECT`:** Símbolo que se refiere a datos (variables). Se mapea a `D` (datos inicializados), `B` (datos no inicializados/BSS), o `R` (datos de solo lectura).
* **`STT_FUNC`:** Símbolo que se refiere a una función (código ejecutable). Se mapea a `T` (código).
* **`STT_SECTION`:** Símbolo asociado a una sección del archivo. Normalmente se representa con `s` o `S`.
* **`STT_FILE`:** Símbolo que da el nombre del archivo fuente. A menudo no se imprime por `nm` o se usa internamente.
* **`STT_COMMON`:** Símbolo de "common block" (un tipo de datos no inicializados). Se mapea a `C`.
* **`STT_TLS`:** Símbolo de Thread Local Storage.
* **Otros tipos:** `STT_GNU_IFUNC`, `STT_LOOS`, `STT_HIOS`, `STT_LOPROC`, `STT_HIPROC`.

**Dependencia:** El `symbol_type` es la base para determinar la categoría principal del símbolo (¿es una función, un dato, etc.?).

### 3. `st_shndx_val` (SHN_ABS, SHN_COMMON, SHN_UNDEF, etc.)

Este valor es el **índice de la sección a la que pertenece el símbolo**. Es un campo directo de la estructura `ElfX_Sym`.

* **`SHN_UNDEF` (Undefined):** El símbolo no está definido en este archivo. Es un símbolo **externo** que será resuelto por el enlazador. Siempre se representa con `U` (o `w` si es débil).
* **`SHN_ABS` (Absolute):** El valor del símbolo es absoluto y no cambia durante el enlace. Se representa con `A`.
* **`SHN_COMMON` (Common):** Indica un símbolo "common block", una forma de datos no inicializados. Se representa con `C`.
* **`SHN_XINDEX`:** Indica que el índice de sección real es demasiado grande para `st_shndx` y está en la tabla `SHN_XINDEX`. (Caso avanzado, probablemente no necesario para tu `ft_nm`).
* **Otros valores (índices de sección reales):** Si `st_shndx_val` es un número mayor que 0 y menor que `SHN_LORESERVE` (generalmente 0xFF00), significa que el símbolo está definido en la sección de la tabla de secciones con ese índice. Aquí es donde se vuelve más complejo, ya que necesitas consultar los **flags de esa sección**.

**Dependencia:** `st_shndx_val` tiene la **mayor prioridad** para algunos casos especiales (`U`, `A`, `C`, `w`). Si el símbolo no cae en estas categorías especiales, entonces `st_shndx_val` te indica qué sección debes examinar para determinar el `char_type`.

### 4. El Tipo y los Flags de la Sección a la que Apunta `st_shndx_val`

Si `st_shndx_val` es un índice de sección real (no `SHN_UNDEF`, `SHN_ABS`, `SHN_COMMON`), entonces necesitas ir a la **tabla de cabeceras de sección (`aux->elf32_sh_table` o `aux->elf64_sh_table`)** y buscar la entrada correspondiente a ese índice (`st_shndx_val`).

Una vez que tengas esa cabecera de sección (`ElfX_Shdr`), deberás revisar dos de sus campos:

* **`sh_type` (Tipo de Sección):**
    * `SHT_PROGBITS`: Contiene datos definidos por el programa (código o datos).
    * `SHT_NOBITS`: Contiene datos que ocupan espacio en memoria pero no en el archivo (BSS).
    * `SHT_STRTAB`: Tabla de cadenas.
    * `SHT_SYMTAB`: Tabla de símbolos.
    * ... y muchos más.

* **`sh_flags` (Flags de Sección):** Estos flags indican las propiedades de la sección.
    * **`SHF_ALLOC`:** La sección ocupa memoria durante la ejecución. (Casi todas las secciones de código/datos importantes tienen esto).
    * **`SHF_EXECINSTR`:** La sección contiene código ejecutable. (Clave para `T` o `t`).
    * **`SHF_WRITE`:** La sección puede ser escrita durante la ejecución (datos modificables). (Clave para `D` o `d`, `B` o `b`).
    * **`SHF_MERGE`, `SHF_STRINGS`, etc.**

**Dependencia:** Esta es la parte más compleja. El `char_type` para `STT_OBJECT` y `STT_FUNC` a menudo depende de los `sh_type` y `sh_flags` de la sección en la que residen.

* Si `symbol_type == STT_FUNC` y la sección tiene `SHF_EXECINSTR` y `SHF_ALLOC`, es probable que sea `T` (o `t` si es local).
* Si `symbol_type == STT_OBJECT` y la sección tiene `SHF_ALLOC` y `SHF_WRITE` y `sh_type == SHT_PROGBITS`, podría ser `D` (o `d`).
* Si `symbol_type == STT_OBJECT` y la sección tiene `SHF_ALLOC` y `SHF_WRITE` y `sh_type == SHT_NOBITS`, es `B` (o `b`, para BSS).
* Si `symbol_type == STT_OBJECT` y la sección tiene `SHF_ALLOC` pero NO `SHF_WRITE`, y `sh_type == SHT_PROGBITS`, podría ser `R` (o `r`, para solo lectura).

---

## Orden de Prioridad de la Lógica

La lógica para determinar el `char_type` sigue un orden de prioridad, ya que un símbolo puede cumplir varias condiciones. Por ejemplo, un símbolo puede ser `STB_WEAK` y `SHN_UNDEF` al mismo tiempo, lo cual lo convierte en `w`.

Un orden común de evaluación (de mayor a menor prioridad) es:

1.  **Símbolos Indefinidos (`SHN_UNDEF`):**
    * Si `st_shndx_val == SHN_UNDEF`:
        * Si `symbol_binding == STB_WEAK`: `char_type = 'w'`
        * En cualquier otro caso: `char_type = 'U'`
    *(Estas son generalmente las primeras comprobaciones)*
2.  **Símbolos Comunes (`SHN_COMMON`):**
    * Si `st_shndx_val == SHN_COMMON`: `char_type = 'C'`
3.  **Símbolos Absolutos (`SHN_ABS`):**
    * Si `st_shndx_val == SHN_ABS`: `char_type = 'A'`
4.  **Símbolos Débiles Definidos (`STB_WEAK`):**
    * Si `symbol_binding == STB_WEAK` (y ya sabemos que NO es `SHN_UNDEF`): `char_type = 'W'`
5.  **Símbolos de Sección (`STT_SECTION`):**
    * Si `symbol_type == STT_SECTION`: `char_type = 'S'`
6.  **Símbolos de Archivo (`STT_FILE`):**
    * Si `symbol_type == STT_FILE`: `nm` a menudo ignora estos o los deja con un tipo `?`.
7.  **Símbolos en Secciones Específicas (Objetos y Funciones):**
    * Aquí es donde necesitas la función auxiliar que te devuelva la `ElfX_Shdr` dado un `st_shndx_val`.
    * Si la sección es una de "bits de programa" (`SHT_PROGBITS`) y tiene el flag **`SHF_EXECINSTR`**: `char_type = 'T'` (para funciones, etc.).
    * Si es `SHT_PROGBITS` y tiene **`SHF_WRITE`** (y `SHF_ALLOC`): `char_type = 'D'` (datos inicializados).
    * Si es `SHT_NOBITS` y tiene **`SHF_WRITE`** (y `SHF_ALLOC`): `char_type = 'B'` (datos no inicializados, BSS).
    * Si es `SHT_PROGBITS` y tiene **`SHF_ALLOC`** pero NO `SHF_WRITE`: `char_type = 'R'` (datos de solo lectura).
    * También hay otros casos para `N` (debug sections), `V` (versión), etc.

8.  **Conversión a minúscula (Local vs. Global):**
    * Después de determinar el carácter base (`U`, `A`, `C`, `T`, `D`, `B`, `R`, `W`, `S`), si `symbol_binding == STB_LOCAL` (y el carácter no es `U` o `w` que ya son minúsculas), entonces conviertes el carácter a minúscula.

---

### Lo que Necesitas Implementar

1.  **Función para Obtener Cabecera de Sección:**
    Necesitas una función auxiliar, por ejemplo:
    `Elf32_Shdr *get_shdr32(uint16_t shndx_val, Elf32_Shdr *sh_table, uint16_t sh_num);`
    `Elf64_Shdr *get_shdr64(uint16_t shndx_val, Elf64_Shdr *sh_table, uint16_t sh_num);`
    Estas funciones te darían el puntero a la cabecera de sección para el índice dado, permitiéndote examinar `sh_type` y `sh_flags`.

2.  **La Lógica Completa en `extr_detc_symbol_type`:**
    Dentro de tu bucle que itera sobre `aux_sym`, aplicarás toda la cadena de `if/else if` basada en las prioridades y combinaciones de `symbol_binding`, `symbol_type`, `st_shndx_val`, y los flags de sección.

3.  **Manejo de Errores/Por Defecto:** Si después de todas las comprobaciones no se ha asignado un `char_type`, un `?` suele ser el valor por defecto para símbolos de tipo desconocido.

Este paso, `extr_detc_symbol_type`, es una función clave que iterará sobre tu `aux_file->symbol_list` y rellenará el campo `char_type` de cada `t_symbol_info` con el carácter adecuado.

¿Más claro cómo estos componentes se conectan y por qué la lógica es "grande"? Esta es la esencia de `nm` y es donde se separan los `ft_nm` básicos de los más completos.
