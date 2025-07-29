#include "../incl/ft_nm.h"

void    complete_type(t_symbol_info *sym, t_stack_file *aux,
    unsigned char type, unsigned char shndx, bool lower)
{
    Elf64_Shdr  *sect_header;
    char        *sect_name;

    if (type == STT_OBJECT)
    {
        if (aux->bits == BITS_32)
        {
            sect_header = &aux->elf32_sh_table[shndx];
            sect_name = (char *)(aux->shstrtab_ptr + sect_header->sh_name);
        }
        else
        {
            sect_header = &aux->elf64_sh_table[shndx];
            sect_name = (char *)(aux->shstrtab_ptr + sect_header->sh_name);
        }
        if (ft_strcmp(sect_name, ".data") == 0 || ft_strcmp(sect_name, ".data.rel.ro") == 0)
            sym->char_type = 'D';
        else if (ft_strcmp(sect_name, ".rodata") == 0)
            sym->char_type = 'R';
        else if (ft_strcmp(sect_name, ".bss") == 0)
            sym->char_type = 'B';
        else
            sym->char_type = '?';
    }
    if (lower == true && (sym->char_type == 'D' || sym->char_type == 'R' ||
        sym->char_type == 'B'))
        sym->char_type = ft_tolower(sym->char_type);
}

void    logic_deterc_type(bool lower, t_symbol_info *sym, unsigned char type,
    unsigned char shndx)
{
    if (type == STT_SECTION)
    {
        if (lower == true)
            sym->char_type = 's';
        else
            sym->char_type = 'S';
    }
    else if (type == STT_NOTYPE)
    {
        if (shndx == SHN_UNDEF)
            sym->char_type = 'U';
        else
            sym->char_type = '?';
    }
    else if (type == STT_FUNC)
    {
        if (lower == true)
            sym->char_type = 't';
        else
            sym->char_type = 'T';
    }
    else if (type == STT_COMMON)
        sym->char_type = 'C';
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
            sym->char_type = 'C';
        else
            sym->char_type = 'A';
    }
    else if (bindign == STB_WEAK)
    {
        if (bindign == STB_WEAK && shndx == SHN_UNDEF)
            sym->char_type = 'w';
        else
            sym->char_type = 'W';
    }
    else if (bindign == STB_LOCAL)
    {
        if (bindign == STB_LOCAL && shndx == SHN_UNDEF)
            sym->char_type = 'U';
        else
            lower = true;        
    }
    else if (bindign == STB_GLOBAL)
        upper = true;
    logic_deterc_type(&lower, &sym, type, shndx);
    return;
}

void    extr_detc_symbol_type(t_stack_file **file)
{
    t_stack_file    *aux;
    t_symbol_info   *sym;
    unsigned char   st_info_val;
    unsigned char   symbol_type;
    unsigned char   symbol_binding;
    uint16_t        shndx_val;
    bool            lower;

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
                symbol_binding = 0;

                if (aux->bits == BITS_32)
                {
                    symbol_type = ELF32_ST_TYPE(st_info_val);
                    symbol_binding = ELF32_ST_BIND(st_info_val);
                }
                else
                {
                    symbol_type = ELF64_ST_TYPE(st_info_val);
                    symbol_binding = ELF64_ST_BIND(st_info_val);
                }
                sym->char_type = '\0';
                logic_deter_symbol(sym, shndx_val, symbol_type, symbol_binding);
                if (sym->char_type == '\0')
                {
                    if (symbol_binding == STB_LOCAL && shndx_val != SHN_UNDEF)
                        lower = true;
                    complete_type(sym, aux, symbol_type, shndx_val, lower); 
                }
                lower = false;
                sym = sym->next;
            }  
        }
        aux = aux->next;
    }
}
