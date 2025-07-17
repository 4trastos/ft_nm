#include "../incl/ft_nm.h"

void    location_headings(t_stack_file **files)
{
    t_stack_file    *aux;
    void            *sh_table_ptr;
    uint64_t        sh_offset;
    uint16_t        sh_num;
    uint16_t        sh_entsize;
    int             flag;

    aux = *files;
    flag = 0;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            if (aux->bits == BITS_32)
            {
                sh_offset = get_elf_u32(aux->elf32_header->e_shoff, aux->endianness);
                sh_num = get_elf_u32(aux->elf32_header->e_shnum, aux->endianness);
                sh_entsize = get_elf_u32(aux->elf32_header->e_shentsize, aux->endianness);
                flag = 1;
            }
            else
            {
                sh_offset = get_elf_u64(aux->elf64_header->e_shoff, aux->endianness);
                sh_num = get_elf_u64(aux->elf64_header->e_shnum, aux->endianness);
                sh_entsize = get_elf_u64(aux->elf64_header->e_shentsize, aux->endianness);
                flag =  1;
            }
        }   
        if (flag)
        {
            if (sh_offset == 0 || sh_offset >= aux->file_size ||
                (sh_offset + (uint64_t)sh_num * sh_entsize) > aux->file_size)
            {
                aux->validity = 0;
                handle_file_error_two("./ft_nm", aux->file, "Section Header Table is out of file bounds");
            }
            sh_table_ptr = (void *)(aux->file_content_ptr + sh_offset); // Calcular el puntero al inicio de la SHT
            printf("\n--- Localización de SHT para '%s' ---\n", aux->file);
            printf("  Offset de SHT: 0x%lx\n", sh_offset);
            printf("  Número de secciones: %u\n", sh_num);
            printf("  Tamaño de entrada de sección: %u bytes\n", sh_entsize);
            printf("  Puntero a SHT en RAM: %p\n", sh_table_ptr);
            printf("-------------------------------------------\n");
        }
        flag = 0;
        aux = aux->next;
    }
}

void    parsing_header(t_stack_file **files)
{
    t_stack_file    *aux;

    aux = *files;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            if (aux->bits == BITS_32)
                aux->elf32_header = (Elf32_Ehdr*)aux->file_content_ptr;
            else if (aux->bits == BITS_64)
                aux->elf64_header = (Elf64_Ehdr*)aux->file_content_ptr;
            else
            {
                aux->validity = 0;
                printf("ft_nm: %s: Error: Unknown bitness for file \n", aux->file);
                return;
            }
        }
        aux = aux->next;
    }
}