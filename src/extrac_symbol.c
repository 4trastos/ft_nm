#include "../incl/ft_nm.h"

char    assign_type_from_section(char *name, uint64_t flags, bool lower)
{
    if (ft_strcmp(name, ".text") == 0 || (flags & SHF_EXECINSTR))
        return (lower ? 't' : 'T');
    if (ft_strcmp(name, ".data") == 0 || ft_strcmp(name, ".got") == 0 || (flags & SHF_WRITE))
        return (lower ? 'd' : 'D');
    if (ft_strcmp(name, ".rodata") == 0 || (flags & SHF_MERGE) || ((flags & SHF_ALLOC) && !(flags & SHF_WRITE)))
        return (lower ? 'r' : 'R');
    if (ft_strcmp(name, ".bss") == 0 || ft_strcmp(name, ".tbss") == 0)
        return (lower ? 'b' : 'B');
    if (flags & SHF_TLS)
        return (lower ? 'g' : 'G');
    return (lower ? '?' : '?'); // Si llega aquí, es un caso no cubierto.
}

void    analyze_section(t_symbol_info *sym, t_stack_file *aux, uint8_t bind, uint16_t shndx)
{
    char        *name = NULL;
    uint64_t    flags = 0;
    bool        lower = (bind == STB_LOCAL);

    if (aux->bits == BITS_32 && shndx < aux->elf32_header->e_shnum)
    {
        Elf32_Shdr *shdr = &aux->elf32_sh_table[shndx];
        name = (char *)(aux->shstrtab_ptr + shdr->sh_name);
        flags = shdr->sh_flags;
    }
    else if(aux->bits == BITS_64 && shndx < aux->elf64_header->e_shnum)
    {
        Elf64_Shdr *shdr = &aux->elf64_sh_table[shndx];
        name = (char *)(aux->shstrtab_ptr + shdr->sh_name);
        flags = shdr->sh_flags; 
    }

    if (name)
        sym->char_type = assign_type_from_section(name, flags, lower);
    else
        sym->char_type = '?';
}

void    determine_symbol_type(t_symbol_info *sym, t_stack_file *aux, uint8_t type, uint8_t bind, uint16_t shndx)
{
    bool    lower = (bind == STB_LOCAL);

    // Prioridad 1: Símbolos Indefinidos (SHN_UNDEF)
    if (shndx == SHN_UNDEF)
    {
        if (bind == STB_WEAK)
        {
            //sym->char_type = (lower) ? 'w' : 'W';
            sym->char_type = (type == STT_OBJECT) ? 'v' : 'w';
        }
        else
            sym->char_type = 'U';
        return;
    }

    // Prioridad 2: Símbolos Absolutos (SHN_ABS) y Símbolos Common (SHN_COMMON)
    if (shndx == SHN_ABS)
    {
        sym->char_type = (lower) ? 'a' : 'A';
        return;
    }
    if (shndx == SHN_COMMON)
    {
        sym->char_type = (lower) ? 'c' : 'C';
        return;
    }

    // Prioridad 3: Símbolos Débiles Definidos (STB_WEAK, y ya no es UNDEF/ABS/COMMON)
    if (bind == STB_WEAK)
    {
        if (type == STT_OBJECT)
            sym->char_type = (lower) ? 'v' : 'V';
        else
            sym->char_type = (lower) ? 'w' : 'W';
        return;
    }

    //Prioridad 4: Símbolos GNU Unique
    if (bind == STB_GNU_UNIQUE)
    {
        sym->char_type = 'u';
        return;
    }

    // Prioridad 5: Otros tipos especiales de símbolo
    if (type == STT_GNU_IFUNC)
    {
        sym->char_type = (lower) ? 'i' : 'I';
        return;
    }

    //5. SECTION / FILE
    if (type == STT_SECTION)
    {
        sym->char_type = (lower) ? 's' : 'S';
        return;
    }

    analyze_section(sym, aux, bind, shndx);
}

void    extr_detc_symbol_type(t_stack_file **file)
{
    t_stack_file    *aux;
    t_symbol_info   *sym;
    uint8_t         type;
    uint8_t         bind;
    uint16_t        shndx;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->symbol_list != NULL)
        {
            sym = aux->symbol_list;
            while (sym)
            {
                if (aux->bits == BITS_32)
                {
                    type = ELF32_ST_TYPE(sym->st_info);
                    bind = ELF32_ST_BIND(sym->st_info);
                }
                else
                {
                    type = ELF64_ST_TYPE(sym->st_info);
                    bind = ELF64_ST_BIND(sym->st_info);
                }
                shndx = sym->shndx;
                sym->char_type = '\0';
                determine_symbol_type(sym, aux, type, bind, shndx);
                sym = sym->next;
            }
        }
        aux = aux->next;
    }  
}