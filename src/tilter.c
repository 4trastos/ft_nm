#include "../incl/ft_nm.h"

const char *nm_type_order = "UuWwVvTtDdRrBbCcSsIiAa?";

int get_type_sort_priority(char type_char)
{
    char *pos = ft_strchr((char *)nm_type_order, type_char);
    if (pos != NULL)
        return (int)(pos - nm_type_order);
    return (999);
}

bool is_alphanum(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}

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

    while (true)
    {
        while (*a != '\0' && !is_alphanum(*a))
            a++;
        while (*b != '\0' && !is_alphanum(*b))
            b++;
        
        x = stripped_char(*a);
        y = stripped_char(*b);

        if (x != y)
            return (x - y);

        if (x == '\0')
            return (0);
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
                    sym_type = ELF32_ST_TYPE(sym->st_info);
                else
                    sym_type = ELF64_ST_TYPE(sym->st_info);
                if (sym_type == STT_FILE)
                    sym->visible = false;
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