#include "../incl/ft_nm.h"

void print_stack_files(t_stack_file *sfile)
{
    t_stack_file *current = sfile;
    int node_count = 0;

    printf("\n******* File Stack Contents *******");
    if (current == NULL)
    {
        printf("\nThe stack is empty.\n");
        return;
    }

    // Imprimir la endianness del host una sola vez al principio
    printf("\n--- Endianness of the Host System: %s ---\n",
           (g_host_endianness == HOST_ENDIAN_LSB) ? "Little-Endian" :
           (g_host_endianness == HOST_ENDIAN_MSB) ? "Big-Endian" :
           "Unknown");

    while (current != NULL)
    {
        printf("\nNodo %d:", node_count);
        printf("\n  File Name: '%s'", current->file);
        printf("\n  Position List: %d", current->position);
        printf("\n  Valid: %s", current->validity ? "Yes" : "No");
        printf("\n  Type: %d", current->type);
        printf("\n  Flag: %s",
            (current->flag == NM_ARG_FILE) ? "No Flag" : 
            (current->flag == NM_FLAG_A) ? "-a" :
            (current->flag == NM_FLAG_G) ? "-g" :
            (current->flag == NM_FLAG_P) ? "-p" :
            (current->flag == NM_FLAG_R) ? "-r" : // Corregido: esta línea se duplicaba antes
            (current->flag == NM_FLAG_U) ? "-u" :
            "Unknown");
        printf("\n  ELF: %s", current->elf ? "Yes" : "No");
        printf("\n  Architecture Class: %d", current->bits);
        printf("\n  Size: %zu", current->file_size);
        printf("\n  Pointer in RAM: %p", current->file_content_ptr);
        
        // Imprimir la endianness del archivo de forma más legible
        printf("\n  File Endianness: %s",
               (current->endianness == ENDIAN_LSB) ? "Little-Endian (LSB)" :
               (current->endianness == ENDIAN_MSB) ? "Big-Endian (MSB)" :
               "Unknown");
        
        // Indicar si se necesitará byte swapping
        if (current->validity && current->elf && current->endianness != ENDIAN_UNKNOWN)
        {
             // La lógica de needs_swap es la que se usa internamente en get_elf_uX
             // Aquí solo indicamos al usuario. Asegúrate de que needs_swap en tu otro archivo
             // esté correctamente implementada (file_endianness != host_endianness).
             if ((current->endianness == ENDIAN_LSB && g_host_endianness == HOST_ENDIAN_MSB) ||
                 (current->endianness == ENDIAN_MSB && g_host_endianness == HOST_ENDIAN_LSB))
             {
                 printf(" (Requires Byte Swapping for values)");
             }
             else
             {
                 printf(" (No Byte Swapping required for values)");
             }
        }
        printf("\n");

        // Imprimir información del encabezado ELF según la arquitectura
        if (current->elf && current->validity) // Solo si es ELF y válido
        {
            if (current->bits == BITS_32 && current->elf32_header)
            {
                Elf32_Ehdr *header = current->elf32_header;
                printf("\n  ELF32 Header:");
                printf("\n    Magic: %02x %02x %02x %02x", 
                       header->e_ident[EI_MAG0], header->e_ident[EI_MAG1],
                       header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
                printf("\n    Class: %d", header->e_ident[EI_CLASS]);
                printf("\n    ELF Version (e_ident): %d", header->e_ident[EI_VERSION]);

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

                printf("\n    Type (e_type): %u", get_elf_u16(header->e_type, current->endianness));
                printf("\n    Machine (e_machine): %u", get_elf_u16(header->e_machine, current->endianness));
                printf("\n    Entry point (e_entry): 0x%lX", get_elf_u64(header->e_entry, current->endianness));
                printf("\n    Offset Program Header Table (e_phoff): 0x%lX", get_elf_u64(header->e_phoff, current->endianness));
                printf("\n    Section Header Table Offset (e_shoff): 0x%lX", get_elf_u64(header->e_shoff, current->endianness));
                printf("\n    Flags (e_flags): 0x%X", get_elf_u32(header->e_flags, current->endianness));
                printf("\n    ELF Header Size (e_ehsize): %u", get_elf_u16(header->e_ehsize, current->endianness));
                printf("\n    Program Header Entry Size (e_phentsize): %u", get_elf_u16(header->e_phentsize, current->endianness));
                printf("\n    Number of Program Header Entries (e_phnum): %u", get_elf_u16(header->e_phnum, current->endianness));
                printf("\n    Section Header Entry Size (e_shentsize): %u", get_elf_u16(header->e_shentsize, current->endianness));
                printf("\n    Number of Section Header Entries (e_shnum): %u", get_elf_u16(header->e_shnum, current->endianness));
                printf("\n    Section Names String Table Index (e_shstrndx): %u\n", get_elf_u16(header->e_shstrndx, current->endianness));
            }
            else
            {
                printf("\n  Header: Not available (problem with bits or header pointer)");
            }

            // --- AÑADIR INFORMACIÓN DE LA SHT AQUÍ ---
            // Solo imprimir si el archivo es válido y los punteros a la SHT han sido calculados
            if (current->validity && 
                ((current->bits == BITS_32 && current->elf32_sh_table != NULL) ||
                 (current->bits == BITS_64 && current->elf64_sh_table != NULL)))
            {
                uint64_t sh_offset = (current->bits == BITS_32) ? 
                                     get_elf_u32(current->elf32_header->e_shoff, current->endianness) :
                                     get_elf_u64(current->elf64_header->e_shoff, current->endianness);
                uint16_t sh_num = (current->bits == BITS_32) ? 
                                  get_elf_u16(current->elf32_header->e_shnum, current->endianness) :
                                  get_elf_u16(current->elf64_header->e_shnum, current->endianness);
                uint16_t sh_entsize = (current->bits == BITS_32) ? 
                                      get_elf_u16(current->elf32_header->e_shentsize, current->endianness) :
                                      get_elf_u16(current->elf64_header->e_shentsize, current->endianness);

                printf("\n  --- Section Header Table (SHT) Info ---");
                printf("\n    SHT Offset (e_shoff): 0x%lX", sh_offset);
                printf("\n    Number of Sections (e_shnum): %u", sh_num);
                printf("\n    Section Entry Size (e_shentsize): %u bytes", sh_entsize);
                printf("\n    SHT Pointer in RAM: %p\n", (current->bits == BITS_32) ? 
                    (void*)current->elf32_sh_table : (void*)current->elf64_sh_table);
            }
            else
            {
                // Si la SHT no se pudo localizar o no es válida
                printf("\n  Section Header Table: Not located or invalid.\n");
            }

        }
        else // Si el archivo no es ELF o no es válido
        {
            printf("\n  Header: It is not an ELF file or it is not valid");
        }

        current = current->next;
        node_count++;
        printf("\n"); // Línea en blanco entre nodos
    }
    printf("******* End of Stack (Total: %d nodes) *******\n\n", node_count);
}