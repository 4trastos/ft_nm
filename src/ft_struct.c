#include "../incl/ft_nm.h"

int ft_findflags(char *str)
{
    int i;

    i = 0;
    if (str[0] != '-')
        return (0);
    i++;
    if (str[i + 1] != '\0')
        return (0);
    if (str[i] != 'a' && str[i] != 'g' && str[i] != 'u' && str[i] != 'r' && str[i] != 'p')
        return (0);
    return (1);
}

void    ft_stack_node(t_stack_file **sfile, t_stack_file *new)
{
    t_stack_file    *aux;

    aux = *sfile;
    if (*sfile == NULL)
    {
        *sfile = new;
        return ;   
    }
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    aux->next = new;
    new->next = NULL;
}

t_stack_file    *ft_create_node(char *str, int pos, int status)
{
    t_stack_file    *new;

    new = (t_stack_file *)malloc(sizeof(t_stack_file));
    if (!new)
        return (NULL);
    new->file = str;
    new->position = pos + 1;
    new->validity = status;
    if (ft_findflags(str))
        new->type = ARG_TYPE_FLAG;
    else
        new->type = ARG_TYPE_FILE;
    new->next = NULL;
    return (new);
}

void ft_print_stack_files(t_stack_file *sfile)
{
    t_stack_file *current = sfile;
    int node_count = 0;

    printf("\n******* Contenido del Stack de Archivos *******");
    if (current == NULL)
    {
        printf("\nEl stack está vacío.\n");
        return;
    }

    // Imprimir la endianness del host una sola vez al principio
    printf("\n--- Endianness del Sistema Host: %s ---\n",
           (g_host_endianness == HOST_ENDIAN_LSB) ? "Little-Endian" :
           (g_host_endianness == HOST_ENDIAN_MSB) ? "Big-Endian" :
           "Desconocida");

    while (current != NULL)
    {
        printf("\nNodo %d:", node_count);
        printf("\n  File Name: '%s'", current->file);
        printf("\n  Position List: %d", current->position);
        printf("\n  Valid: %s", current->validity ? "Yes" : "No");
        printf("\n  Type: %d", current->type);
        printf("\n  Flag: %d", current->flag);
        printf("\n  ELF: %s", current->elf ? "Yes" : "No");
        printf("\n  Architecture Class: %d", current->bits);
        printf("\n  Size: %zu", current->file_size);
        printf("\n  Pointer in RAM: %p", current->file_content_ptr);
        
        // Imprimir la endianness del archivo de forma más legible
        printf("\n  File Endianness: %s",
               (current->endianness == ENDIAN_LSB) ? "Little-Endian (LSB)" :
               (current->endianness == ENDIAN_MSB) ? "Big-Endian (MSB)" :
               "Desconocida");
        
        // Indicar si se necesitará byte swapping
        if (current->validity && current->elf && current->endianness != ENDIAN_UNKNOWN)
        {
             if ((current->endianness == ENDIAN_LSB && g_host_endianness == HOST_ENDIAN_MSB) ||
                 (current->endianness == ENDIAN_MSB && g_host_endianness == HOST_ENDIAN_LSB))
             {
                 printf(" (Requiere Byte Swapping)");
             }
             else
             {
                 printf(" (No requiere Byte Swapping)");
             }
        }
        printf("\n");

        // Imprimir información del encabezado ELF según la arquitectura
        if (current->elf && current->validity) // Solo si es ELF y válido
        {
            // Aquí es donde DEBES usar get_elf_uX para cada campo multi-byte
            // para asegurar que los valores se muestren correctamente
            // independientemente de la endianness del archivo y del host.

            if (current->bits == BITS_32 && current->elf32_header)
            {
                Elf32_Ehdr *header = current->elf32_header;
                printf("\n  ELF32 Header:");
                // e_ident es un array de bytes, no necesita swap
                printf("\n    Magic: %02x %02x %02x %02x", 
                       header->e_ident[EI_MAG0], header->e_ident[EI_MAG1],
                       header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
                printf("\n    Class: %d", header->e_ident[EI_CLASS]);
                printf("\n    ELF Version (e_ident): %d", header->e_ident[EI_VERSION]); // EI_VERSION es un byte

                // *** APLICAR get_elf_uX AQUÍ ***
                printf("\n    Type (e_type): %u", get_elf_u16(header->e_type, current->endianness));
                printf("\n    Machine (e_machine): %u", get_elf_u16(header->e_machine, current->endianness));
                printf("\n    Entry point (e_entry): 0x%X", get_elf_u32(header->e_entry, current->endianness));
                printf("\n    Offset Program Header Table (e_phoff): 0x%X", get_elf_u32(header->e_phoff, current->endianness));
                printf("\n    Section Header Table Offset (e_shoff): 0x%X", get_elf_u32(header->e_shoff, current->endianness));
                printf("\n    Flags (e_flags): 0x%X", get_elf_u32(header->e_flags, current->endianness));
                printf("\n    ELF Header Size (e_ehsize): %u", get_elf_u16(header->e_ehsize, current->endianness));
                printf("\n    Program Header Entry Size (e_phentsize): %u", get_elf_u16(header->e_phentsize, current->endianness));
                printf("\n    Number of Program Header Entries (e_phnum): %u", get_elf_u16(header->e_phnum, current->endianness));
                printf("\n    Section Header Entry Size (e_shentsize): %u", get_elf_u16(header->e_shentsize, current->endianness));
                printf("\n    Number of Section Header Entries (e_shnum): %u", get_elf_u16(header->e_shnum, current->endianness));
                printf("\n    Section Names String Table Index (e_shstrndx): %u\n", get_elf_u16(header->e_shstrndx, current->endianness));
            }
            else if (current->bits == BITS_64 && current->elf64_header)
            {
                Elf64_Ehdr *header = current->elf64_header;
                printf("\n  ELF64 Header:");
                printf("\n    Magic: %02x %02x %02x %02x", 
                       header->e_ident[EI_MAG0], header->e_ident[EI_MAG1],
                       header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
                printf("\n    Class: %d", header->e_ident[EI_CLASS]);
                printf("\n    ELF Version (e_ident): %d", header->e_ident[EI_VERSION]);

                // *** APLICAR get_elf_uX AQUÍ ***
                printf("\n    Type (e_type): %u", get_elf_u16(header->e_type, current->endianness));
                printf("\n    Machine (e_machine): %u", get_elf_u16(header->e_machine, current->endianness));
                printf("\n    Entry point (e_entry): 0x%lX", get_elf_u64(header->e_entry, current->endianness)); // Nota %lX para uint64_t
                printf("\n    Offset Program Header Table (e_phoff): 0x%lX", get_elf_u64(header->e_phoff, current->endianness));
                printf("\n    Section Header Table Offset (e_shoff): 0x%lX", get_elf_u64(header->e_shoff, current->endianness));
                printf("\n    Flags (e_flags): 0x%X", get_elf_u32(header->e_flags, current->endianness)); // e_flags es uint32_t incluso en 64-bit
                printf("\n    ELF Header Size (e_ehsize): %u", get_elf_u16(header->e_ehsize, current->endianness));
                printf("\n    Program Header Entry Size (e_phentsize): %u", get_elf_u16(header->e_phentsize, current->endianness));
                printf("\n    Number of Program Header Entries (e_phnum): %u", get_elf_u16(header->e_phnum, current->endianness));
                printf("\n    Section Header Entry Size (e_shentsize): %u", get_elf_u16(header->e_shentsize, current->endianness));
                printf("\n    Number of Section Header Entries (e_shnum): %u", get_elf_u16(header->e_shnum, current->endianness));
                printf("\n    Section Names String Table Index (e_shstrndx): %u\n", get_elf_u16(header->e_shstrndx, current->endianness));
            }
            else
            {
                printf("\n  Header: No disponible (problema con bits o puntero de encabezado)");
            }
        }
        else
        {
            printf("\n  Header: No es archivo ELF o no es válido");
        }

        current = current->next;
        node_count++;
        printf("\n");
    }
    printf("******* Fin del Stack (Total: %d nodos) *******\n\n", node_count);
}