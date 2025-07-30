#include "../incl/ft_nm.h"

void    complete_type(t_symbol_info *sym, t_stack_file *aux,
    unsigned char type, unsigned char shndx, bool lower)
{
    Elf64_Shdr  *sect64 = NULL;
    Elf32_Shdr  *sect32 = NULL;   
    char        *sect_name = NULL;
    //Elf64_Word  sh_flags = 0;
    //Elf64_Word  sh_type = 0;

    if (shndx == SHN_UNDEF)
    {
        sym->char_type = 'U';
        return;
    }

    if (type == STT_OBJECT)
    {
        if ((aux->bits == BITS_32 && (shndx == 0 || shndx >= aux->elf32_header->e_shnum)) ||
            (aux->bits == BITS_64 && (shndx == 0 || shndx >= aux->elf64_header->e_shnum)))
        {
            sym->char_type = '?';
            return;
        }
        if (aux->bits == BITS_32)
        {
            sect32 = &aux->elf32_sh_table[shndx];
            sect_name = (char *)(aux->shstrtab_ptr + sect32->sh_name);
            //sh_flags = sect32->sh_flags;
            //sh_type = sect32->sh_type;
        }
        else
        {
            sect64 = &aux->elf64_sh_table[shndx];
            sect_name = (char *)(aux->shstrtab_ptr + sect64->sh_name);
            //sh_flags = sect64->sh_flags;
            //sh_type = sect64->sh_type;
        }

        // // Sección de código ejecutable (.text)
        // if (sh_flags & SHF_EXECINSTR)
        //     sym->char_type = (lower) ? 't' : 'T';
        // // Sección de datos inicializados (.data, .rodata, .got)
        // else if ((sh_flags & SHF_ALLOC) && (sh_flags & SHF_WRITE) && (sh_type == SHT_PROGBITS))
        //     sym->char_type = (lower) ? 'd' : 'D';
        // // Sección de datos de solo lectura (.rodata)
        // else if ((sh_flags & SHF_ALLOC) && !(sh_flags & SHF_WRITE) && (sh_type == SHT_PROGBITS))
        //     sym->char_type = (lower) ? 'r' : 'R';
        // // Sección BSS (datos no inicializados)
        // else if ((sh_flags & SHF_ALLOC) && (sh_flags & SHF_WRITE) && (sh_type == SHT_NOBITS))
        //     sym->char_type = (lower) ? 'b' : 'B';
        // else if (sh_flags & SHF_ALLOC)
        //     sym->char_type = (lower) ? 'd' : 'D';
        // else
        //     sym->char_type = '?';
        if (ft_strcmp(sect_name, ".text") == 0)
            sym->char_type = (lower) ? 't' : 'T';
        else if (ft_strcmp(sect_name, ".data") == 0 || ft_strcmp(sect_name, ".data.rel.ro") == 0)
            sym->char_type = (lower) ? 'd' : 'D';
        else if (ft_strcmp(sect_name, ".rodata") == 0)
            sym->char_type = (lower) ? 'r' : 'R';
        else if (ft_strcmp(sect_name, ".bss") == 0 || ft_strcmp(sect_name, ".tbss") == 0)
            sym->char_type = (lower) ? 'b' : 'B';
        else if (ft_strcmp(sect_name, ".got") == 0)
            sym->char_type = (lower) ? 'd' : 'D';
        else
            sym->char_type = '?';
    }
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
    unsigned char type, unsigned char binding)
{
    bool    lower = false;

    if (shndx == SHN_ABS)
    {
        sym->char_type = 'A';
        return;
    }
    else if (shndx == SHN_COMMON)
    {
        sym->char_type = (binding == STB_LOCAL) ? 'c' : 'C';
        return;
    }
    else if (binding == STB_WEAK)
    {
        if (shndx == SHN_UNDEF)
            sym->char_type = (type == STT_OBJECT) ? 'v' : 'V';
        else
            sym->char_type = (type == STT_OBJECT) ? 'w' : 'W';
        return;
    }
    else if (binding == STB_LOCAL)
        lower = true;
    
    if (type == STT_SECTION || type == STT_FUNC || type == STT_COMMON || type == STT_NOTYPE)
        logic_deterc_type(lower, sym, type, shndx);

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
                if (sym->char_type == '\0' || sym->char_type == '?')
                {
                    bool lower = (symbol_binding == STB_LOCAL);
                        complete_type(sym, aux, symbol_type, shndx_val, lower);
                }
                sym = sym->next;
            }  
        }
        aux = aux->next;
    }
}
