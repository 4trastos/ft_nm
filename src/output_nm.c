#include "../incl/ft_nm.h"

void    ft_output(t_stack_file **file, int argc)
{
    t_stack_file    *aux;
    t_symbol_info   *symb;
    const char      *hex_chars = "0123456789abcdef";
    char            hex_buffer[16]; // Para almacenar dígitos hexadecimales (64-bit)
    int             i;
    uint64_t        temp_value;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1)
        {
            if (argc > 2)
            {
                write(1, "\n", 1);
                write(1, aux->file, ft_strlen(aux->file));
                write(1, ":\n", 2);
            }
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
                        // Convertir valor a hexadecimal manualmente
                        int width = (aux->bits == BITS_64) ? 16 : 8;
                        temp_value = symb->value;
                        
                        // Llenar el buffer de derecha a izquierda
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
        else if (aux->validity == 1 && aux->elf == 0)
            handle_file_error_two("./ft_nm", aux->file, "file format not recognized");
        else
            handle_file_error_two("./ft_nm", aux->file, aux->error_msg);
        aux = aux->next;
    }
}