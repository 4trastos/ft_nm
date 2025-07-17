#include "../incl/ft_nm.h"

void    location_headings(t_stack_file **files)
{
    t_stack_file    *aux;
    void            *sh_table_ptr;
    uint64_t        sh_offset;
    uint16_t        sh_num;
    uint16_t        sh_entsize;   

    aux = *files;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            if (aux->bits == BITS_32)
            {
                if (aux->elf32_header == NULL)
                {
                    
                }
            }
            else
            {

            }
        }   
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