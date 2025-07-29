#include "../incl/ft_nm.h"

void    logic_deterc_type(bool upper, bool lower, t_symbol_info *sym, unsigned char type)
{
    if (lower == true)
    {
        
    }
    else
    {

    }
    return;
}

void    logic_deter_symbol(t_symbol_info *sym, uint16_t shndx,
    unsigned char type, unsigned char bindign)
{
    bool    upper = false;
    bool    lower = false;

    if (shndx == SHN_ABS || shndx == SHN_COMMON)
    {
        if (shndx == SHN_COMMON)
        {
            sym->char_type = 'C';
            return;
        }
        sym->char_type = 'A';
        return;
    }
    if (bindign == STB_WEAK)
    {
        if (bindign == STB_WEAK && shndx == SHN_UNDEF)
        {
            sym->char_type = 'w';
            return;
        }    
        sym->char_type = 'W';
        return;
    }
    else if (bindign == STB_LOCAL)
    {
        if (bindign == STB_LOCAL && shndx == SHN_UNDEF)
        {
            sym->char_type = 'U';
            return;
        }
        lower = true;        
    }
    else if (bindign == STB_GLOBAL)
        upper = true;
    logic_deterc_type(&upper, &lower, &sym, type);
    return;
}

void    extr_detc_symbol_type(t_stack_file **file)
{
    t_stack_file    *aux;
    t_symbol_info   *sym;
    unsigned char   st_info_val;
    unsigned char   symbol_type;
    unsigned char   symbol_bindign;
    uint16_t        shndx_val;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->symbol_list != NULL)
        {
            sym = aux->symbol_list;
            while (sym)
            {
                st_info_val = sym->st_info;
                shndx_val = sym->shndx;
                symbol_type = 0;
                symbol_bindign = 0;

                if (aux->bits == BITS_32)
                {
                    symbol_type = ELF32_ST_TYPE(st_info_val);
                    symbol_bindign = ELF32_ST_BIND(st_info_val);
                }
                else
                {
                    symbol_type = ELF64_ST_TYPE(st_info_val);
                    symbol_bindign = ELF64_ST_BIND(st_info_val);
                }
                logic_deter_symbol(&sym, shndx_val, symbol_type, symbol_bindign);
                sym = sym->next;
            }
            
        }
        aux = aux->next;
    }
}
