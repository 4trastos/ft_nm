#include "../incl/ft_nm.h"

int stripped_char(char c)
{
    if (c >= 'A' && c <= 'Z')
        c = c + ('a' - 'A');
    return (c);
}

int ignore_underscores(char *a, char *b)
{
    int x;
    int y;

    while (*a == '_')
        a++;
    while (*b == '_')
        b++;

    while (*a && *b)
    {
        x = stripped_char(*a);
        y = stripped_char(*b);

        if (x < y)
            return (-1);
        else if (x > y)
            return (1);
        
        a++;
        b++;
    }
    
    if (*a)
        return (1);
    if (*b)
        return (-1);
    return (0);
}

void    tilter_collecting(t_stack_file **file)
{
    t_stack_file    *aux;
    t_symbol_info   *sym;
    unsigned char   sym_type;
    //unsigned char   sym_binding;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->symbol_list != NULL)
        {
            sym = aux->symbol_list;
            while (sym)
            {   
                sym->visible = true;

                if (aux->bits == BITS_32)
                {
                    sym_type = ELF32_ST_TYPE(sym->st_info);
                    //sym_binding = ELF32_ST_BIND(sym->st_info);
                }
                else
                {
                    sym_type = ELF64_ST_TYPE(sym->st_info);
                    //sym_binding = ELF64_ST_BIND(sym->st_info);
                }

                //if (sym_type == STT_SECTION || sym_type == STT_FILE)
                if (sym_type == STT_FILE)
                    sym->visible = false;
                //else if (sym_type == STT_NOTYPE && sym_binding == STB_LOCAL && sym->shndx != SHN_UNDEF)
                //    sym->visible = false;
                else if (!sym->name || sym->name[0] == '\0')
                {
                    if (sym->char_type != 'A' && sym->char_type != 'a')
                        sym->visible = false;
                }   

                sym = sym->next;
            }
        }
        aux = aux->next;
    }
}