## **listado exhaustivo de pruebas** para tu `ft_nm` (proyecto de 42 Outer Core) 
que cubra todos los casos críticos y edge cases. El objetivo es demostrar que se comporta 

**exactamente como el `nm` original** y es **robusto frente a intentos de "romperlo"**. 

Basado en la hoja de corrección típica y conocimientos avanzados de `nm`:

---

### 📜 **Listado de Pruebas Obligatorias** (Para defensa)

#### **1. Archivos Objeto Básicos**
```bash
# Objeto estático (32 bits)
ft_nm obj32.o
nm obj32.o

# Objeto dinámico (64 bits)
ft_nm obj64.o
nm obj64.o

# Comparación directa
diff <(ft_nm obj64.o) <(nm obj64.o)
```

#### **2. Binarios Ejecutables**
```bash
# Ejecutable simple
ft_nm /bin/ls
nm /bin/ls

# Ejecutable con símbolos debug
ft_nm -a a.out
nm -a a.out
```

#### **3. Opciones de Visualización**
```bash
# Símbolos externos
ft_nm -g a.out
nm -g a.out

# Sólo símbolos undefined
ft_nm -u libft.a
nm -u libft.a

# Orden numérico
ft_nm -n a.out
nm -n a.out
```

#### **4. Archivos Especiales**
```bash
# Biblioteca estática
ft_nm libft.a
nm libft.a

# Biblioteca dinámica
ft_nm /usr/lib/libc.so
nm /usr/lib/libc.so

# Archivo vacío
touch empty.o && ft_nm empty.o
nm empty.o
```

#### **5. Casos de Error (¡IMPORTANTES!)**
```bash
# Archivo inexistente
ft_nm no_existe.o
nm no_existe.o

# Directorio
ft_nm .
nm .

# Archivo sin permisos
chmod 000 no_read.o && ft_nm no_read.o
nm no_read.o

# Archivo no ELF (ej: .c)
ft_nm file.c
nm file.c
```

---

### 💣 **Pruebas de Robustez (Para romper tu ft_nm)**
```bash
# Archivo ELF corrupto (¡debe manejar el error!)
ft_nm corrupted.o

# Binario enorme (>1GB)
ft_nm large_binary

# Símbolos con nombres raros
ft_nm weird_symbols.o

# Múltiples argumentos
ft_nm *.o *.a

# Combinación de opciones
ft_nm -gnu a.out obj.o
```

---

### 🔍 **Pruebas Específicas de 42**
```bash
# Proyecto libft
ft_nm libft.a | grep "ft_memset"

# Archivos minimizados
ft_nm tiny.o  # ELF con solo 1 sección

# Sin argumentos (debe mostrar a.out)
ft_nm
```

---

### 🛠️ **Herramientas Útiles**
1. **Generar archivos de prueba**:
   ```bash
   # Crear objeto simple
   echo "int main(){return 0;}" > test.c && gcc test.c -o test.o
   ```

2. **Comparación automática**:
   ```bash
   # Script para comparar outputs
   for file in *.o; do
       diff <(ft_nm "$file") <(nm "$file")
   done
   ```

3. **Test de fugas** (con `valgrind`):
   ```bash
   valgrind --leak-check=full ./ft_nm a.out
   ```

---

### 📌 **Checklist de Corrección (Lo que suelen evaluar)**
1. **Manejo de errores**:
   - ¿Muestra los mismos errores que `nm`?
   - ¿Gestión de permisos/archivos inválidos?

2. **Formato de salida**:
   - ¿Columnas alineadas?
   - ¿Tipos de símbolos correctos (T, U, W, etc.)?

3. **Opciones**:
   - ¿`-a`, `-g`, `-u` funcionan igual?

4. **Robustez**:
   - ¿No hace segfault con entradas extrañas?
   - ¿Maneja archivos grandes?

5. **Bonus**:
   - ¿Ordenación idéntica?
   - ¿Soporta 32/64 bits?

---

### 💡 **Consejo Final**:
Prepara un **script automatizado** que ejecute todas estas pruebas y compare con `nm`. Por ejemplo:
```bash
#!/bin/bash
TEST_FILES="test1.o test2.o a.out libft.a no_existe.o"

for file in $TEST_FILES; do
    echo "Testing $file..."
    diff <(./ft_nm "$file") <(nm "$file")
    if [ $? -eq 0 ]; then
        echo "✅ Passed"
    else
        echo "❌ Failed"
    fi
done
```

---

## Aquí tienes los comandos para **generar los objetos de prueba** (32-bit y 64-bit) y cómo validarlos:

---

### 🔧 **Generación de Objetos de Prueba**

#### 1. **Objeto estático 32-bit (`obj32.o`)**:
```bash
echo -e '#include <stdio.h>\nvoid func32(){printf("32-bit\\n");}' > test32.c
gcc -m32 -c test32.c -o obj32.o
```

#### 2. **Objeto dinámico 64-bit (`obj64.o`)**:
```bash
echo -e '#include <stdio.h>\nvoid func64(){printf("64-bit\\n");}' > test64.c
gcc -c test64.c -o obj64.o
```

#### 3. **Verificación de los objetos**:
```bash
file obj32.o  # Debe mostrar: ELF 32-bit LSB relocatable
file obj64.o  # Debe mostrar: ELF 64-bit LSB relocatable
```

---

### 🧪 **Pruebas con `ft_nm` vs `nm`**

#### 1. **Comparación para 32-bit**:
```bash
ft_nm obj32.o > ft_nm_output.txt
nm obj32.o > nm_output.txt
diff ft_nm_output.txt nm_output.txt
```

#### 2. **Comparación para 64-bit**:
```bash
ft_nm obj64.o > ft_nm_output.txt
nm obj64.o > nm_output.txt
diff ft_nm_output.txt nm_output.txt
```

#### 3. **Comparación directa (usando proceso sustitución)**:
```bash
diff <(ft_nm obj64.o) <(nm obj64.o)
```

---

### 💣 **Casos Adicionales para Robustez**

#### 1. **Objeto con símbolos complejos**:
```bash
echo -e 'int global; static int local; void unused(){}' > complex.c
gcc -c complex.c -o complex.o
ft_nm complex.o
```

#### 2. **Objeto sin símbolos**:
```bash
echo 'int main(){return 0;}' > minimal.c
gcc -c minimal.c -o minimal.o
ft_nm minimal.o
```

#### 3. **Archivo ELF corrupto** (para test de errores):
```bash
head -c 100 /bin/ls > fake.o
ft_nm fake.o  # Debe mostrar error claro
```

---

### 📌 **Notas Clave**:
1. **Dependencias**:
   - Necesitas `gcc` y soporte para 32-bit (`gcc-multilib` en Ubuntu).
   - Instálalo con:
     ```bash
     sudo apt install gcc-multilib  # Para Linux
     ```

2. **Si falla la compilación 32-bit**:
   - Asegúrate de tener las librerías necesarias:
     ```bash
     sudo apt install libc6-dev-i386
     ```

3. **Para pruebas exhaustivas**:
   - Usa el script de comparación masiva:
     ```bash
     for obj in *.o; do
         echo "Testing $obj..."
         diff <(ft_nm "$obj") <(nm "$obj")
     done
     ```

---

### 🔍 **¿Qué debes verificar en la salida?**
1. **Formato idéntico** (direcciones, tipos, nombres).
2. **Orden idéntico** de símbolos.
3. **Mismos errores** en casos inválidos.

¡Con estos objetos podrás demostrar que tu `ft_nm` es **tan robusto como el original**! 🚀
