#include "../incl/ft_nm.h"

void    ft_output(t_stack_file **file, int argc)
{
    t_stack_file    *aux;
    t_symbol_info   *symb;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1)
        {
            if (argc > 2)
            {
                putstr_stderr("\n");
                putstr_stderr(aux->file);
                putstr_stderr(":\n");
            }
        }
        symb = aux->symbol_list;
        while (symb)
        {
            if (symb->visible)
            {
                if (symb->shndx == SHN_UNDEF)
                    {
                        if (aux->bits == BITS_64)
                            write(1, "                ", 16);
                        else
                            write(1, "        ", 8);
                    }
                else
                {
                    ft_puthex(symb->value, (aux->bits == BITS_64) ? 16 : 8);
                }
                write(1, " ", 1);
                write(1, &symb->char_type, 1);
                write(1, " ", 1);
                putstr_stderr(symb->name);
                write(1, "\n", 1);
            }
            symb = symb->next;
        }
        aux = aux->next;
    }
    aux = *file;
    while (aux)
    {
        if (!aux->validity)
            handle_file_error_two("./ft_nm", aux->file, aux->error_msg);
        else if (!aux->elf)
            handle_file_error_two("./ft_nm", aux->file, "file format not recognized");
        aux = aux->next;
    }
}