#include "../incl/ft_nm.h"

void    ft_output(t_stack_file **file, int argc)
{
    t_stack_file    *aux;
    t_symbol_info   *symb;
    bool            is_undefined;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1)
        {
            if (argc > 2)
                printf("\n %s \n", aux->file);
        }
        symb = aux->symbol_list;
        while (symb)
        {
            if (symb->visible)
            {
                is_undefined = (symb->shndx == SHN_UNDEF);
                if (aux->bits == BITS_64)
                {
                    if (is_undefined)
                        printf("%16s", "");
                    else
                        printf("%016lx", symb->value);
                }
                else
                {
                    if (is_undefined)
                        printf("%8s", "");
                    else
                        printf("%08lx", symb->value);
                }
                printf(" %c %s\n", symb->char_type, symb->name);
            }
            symb = symb->next;
        }
        if (aux->validity  == 1 && aux->elf == 0)
            handle_file_error_two("./ft_nm", aux->file, "file format not recognized");
        aux = aux->next;
    }
}