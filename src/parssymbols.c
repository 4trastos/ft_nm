#include "../incl/ft_nm.h"

const char  *get_symbol_name(uint32_t offset_name, void *strtab_ptr, size_t strtab_size)
{
    if (offset_name >= strtab_size)
        return NULL;
    return ((const char *)(strtab_ptr + offset_name));
}

void    parsing_symbol_ent(t_stack_file **file)
{
    t_stack_file    *aux;
    uint32_t        num_symbol;
    uint32_t        i;
    char            symbol_name;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->strtab_ptr != NULL &&
            aux->symtab_ptr != NULL && aux->symtab_size > 0)
        {
            clear_symbol_list(&aux->symbol_list);
            if (aux->bits == BITS_32)
            {

            }
            else
            {
                
            }
        }
        aux = aux->next;
    }
}