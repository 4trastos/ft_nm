#include "../incl/ft_nm.h"

void    ft_output(t_stack_file **file, int argc)
{
    t_stack_file    *aux;
    t_stack_file    *member;
    t_symbol_info   *symb;
    const char      *hex_chars = "0123456789abcdef";
    char            hex_buffer[17];
    int             i;
    uint64_t        temp_value;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 0)
        {
            if (aux->error_msg)
                handle_file_error_two("ft_nm", aux->file, aux->error_msg);
        }
        else if (aux->validity == -2)
        {
            write(STDERR_FILENO, "ft_nm: Warning: «", 18);
            write(STDERR_FILENO, aux->file, ft_strlen(aux->file));
            write(STDERR_FILENO, "» is a directory\n", 18);
        }
        else if (aux->elf == 2)
        {
            if (argc > 2)
            {
                write(1, "\n", 1);
                write(1, aux->file, ft_strlen(aux->file));
                write(1, ":\n", 2);
            }
            
            member = aux->dinamic_bin;
            while (member)
            {
                if (member->validity == 1 && member->symbol_list)
                {
                    write(1, "\n", 1);
                    write(1, member->file, ft_strlen(member->file));
                    write(1, ":\n", 2);

                    symb = member->symbol_list;
                    while (symb)
                    {
                        if (symb->visible)
                        {
                            if (symb->shndx == SHN_UNDEF)
                            {
                                write(1, (member->bits == BITS_64) ? "                " : "        ",
                                      (member->bits == BITS_64) ? 16 : 8);
                            }
                            else
                            {
                                int width = (member->bits == BITS_64) ? 16 : 8;
                                temp_value = symb->value;
                                
                                i = width;
                                while (i-- > 0)
                                {
                                    hex_buffer[i] = hex_chars[temp_value & 0xF];
                                    temp_value >>= 4;
                                }
                                write(1, hex_buffer, width);
                            }
                            write(1, " ", 1);
                            write(1, &symb->char_type, 1);
                            write(1, " ", 1);
                            write(1, symb->name, ft_strlen(symb->name));
                            write(1, "\n", 1);
                        }
                        symb = symb->next;
                    }
                }
                member = member->next;
            }
        }
        else if (aux->elf == 1)
        {
            if (argc > 2)
            {
                write(1, "\n", 1);
                write(1, aux->file, ft_strlen(aux->file));
                write(1, ":\n", 2);
            }
            if (aux->symbol_list) {
                symb = aux->symbol_list;
                while (symb)
                {
                    if (symb->visible)
                    {
                        if (symb->shndx == SHN_UNDEF)
                        {
                            write(1, (aux->bits == BITS_64) ? "                " : "        ",
                                  (aux->bits == BITS_64) ? 16 : 8);
                        }
                        else
                        {
                            int width = (aux->bits == BITS_64) ? 16 : 8;
                            temp_value = symb->value;
                            
                            i = width;
                            while (i-- > 0)
                            {
                                hex_buffer[i] = hex_chars[temp_value & 0xF];
                                temp_value >>= 4;
                            }
                            write(1, hex_buffer, width);
                        }
                        write(1, " ", 1);
                        write(1, &symb->char_type, 1);
                        write(1, " ", 1);
                        write(1, symb->name, ft_strlen(symb->name));
                        write(1, "\n", 1);
                    }
                    symb = symb->next;
                }
            } else {
                 handle_file_error_two("ft_nm", aux->file, "no symbols");
            }
        }
        else
            handle_file_error_two("ft_nm", aux->file, "file format not recognized");
        aux = aux->next;
    }
}