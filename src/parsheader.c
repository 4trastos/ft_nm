#include "../incl/ft_nm.h"

void    ft_parsing_header(t_stack_file **files)
{
    t_stack_file    *aux;
    unsigned char   *elf;

    aux = *files;
    elf = aux->file_content_ptr;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {

        }
        aux = aux->next;
    }
}