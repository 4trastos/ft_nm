#include "../incl/ft_nm.h"

void    tilter_collecting(t_stack_file **file)
{
    t_stack_file    *aux;
    t_symbol_info   *sym;
    unsigned char   sym_type;
    unsigned char   sym_binding;

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
                    sym_binding = ELF32_ST_BIND(sym->st_info);
                }
                else
                {
                    sym_type = ELF64_ST_TYPE(sym->st_info);
                    sym_binding = ELF64_ST_BIND(sym->st_info);
                }
                if (sym_type == STT_SECTION || sym_type == STT_FILE)
                    sym->visible = false;
                else if (sym_type == STT_NOTYPE && sym_binding == STB_LOCAL && sym->shndx != SHN_UNDEF)
                    sym->visible = false;
                else if ((!sym->name || sym->name[0] == '\0') && sym->char_type != 'A' && sym->char_type != 'a')
                    sym->visible = false;

                sym = sym->next;
            }
        }
        aux = aux->next;
    }
}