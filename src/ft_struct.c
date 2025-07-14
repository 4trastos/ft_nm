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

    while (current != NULL)
    {
        printf("\nNodo %d:", node_count);
        printf("\n  Archivo: '%s'", current->file);
        printf("\n  Posición original: %d", current->position);
        printf("\n  Válido: %s", current->validity ? "Sí" : "No");
        printf("\n  Tipo: %d", current->type);
        printf("\n  Flag: %d", current->flag);
        printf("\n  ELF: %s", current->elf ? "Sí" : "No");
        printf("\n  Bits: %d", current->bits);
        printf("\n  Size: %zu", current->file_size);
        printf("\n  ELF en RAM: %p", current->file_content_ptr);
        printf("\n  Endianness: %d", current->endianness);

        // Imprimir información del encabezado ELF según la arquitectura
        if (current->elf)
        {
            if (current->bits == BITS_32 && current->elf32_header)
            {
                printf("\n  ELF32 Header:");
                printf("\n    Magic: %02x %02x %02x %02x", 
                       current->elf32_header->e_ident[0],
                       current->elf32_header->e_ident[1],
                       current->elf32_header->e_ident[2],
                       current->elf32_header->e_ident[3]);
                printf("\n    Class: %d", current->elf32_header->e_ident[EI_CLASS]);
                printf("\n    Type: %d", current->elf32_header->e_type);
                printf("\n    Machine: %d", current->elf32_header->e_machine);
                printf("\n    Entry point: 0x%x", current->elf32_header->e_entry);
            }
            else if (current->bits == BITS_64 && current->elf64_header)
            {
                printf("\n  ELF64 Header:");
                printf("\n    Magic: %02x %02x %02x %02x", 
                       current->elf64_header->e_ident[0],
                       current->elf64_header->e_ident[1],
                       current->elf64_header->e_ident[2],
                       current->elf64_header->e_ident[3]);
                printf("\n    Class: %d", current->elf64_header->e_ident[EI_CLASS]);
                printf("\n    Type: %d", current->elf64_header->e_type);
                printf("\n    Machine: %d", current->elf64_header->e_machine);
                printf("\n    Entry point: 0x%lx", current->elf64_header->e_entry);
            }
            else
            {
                printf("\n  Header: No disponible");
            }
        }
        else
        {
            printf("\n  Header: No es archivo ELF");
        }

        current = current->next;
        node_count++;
        printf("\n");
    }
    printf("******* Fin del Stack (Total: %d nodos) *******\n\n", node_count);
}