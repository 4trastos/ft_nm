#include "../incl/ft_nm.h"

char  *get_symbol_name(uint32_t offset_name, void *strtab_ptr, size_t strtab_size)
{
    if (offset_name >= strtab_size)
        return NULL;
    return (strtab_ptr + offset_name);
}

void    parsing_symbol_ent(t_stack_file **file)
{
    t_stack_file    *aux;
    uint32_t        num_symbol;
    uint32_t        i;
    char            *symbol_name;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->strtab_ptr != NULL &&
            aux->symtab_ptr != NULL && aux->symtab_size > 0)
        {
            clear_symbol_list(&aux->symbol_list);
            if (aux->bits == BITS_32)
            {
                num_symbol = aux->symtab_size / sizeof(Elf32_Sym);
                Elf32_Sym *sym_table_ptr = (Elf32_Sym *)aux->symtab_ptr;
                for (i = 0; i < num_symbol; i++)
                {
                    Elf32_Sym *sym = &sym_table_ptr[i];

                    uint32_t st_name_offset = get_elf_u32(*(uint32_t*)&sym->st_name, aux->endianness);
                    uint32_t st_value = get_elf_u32(*(uint32_t*)&sym->st_value, aux->endianness);
                    uint16_t st_shndx = get_elf_u16(*(uint16_t*)&sym->st_shndx, aux->endianness);
                    unsigned char st_info = sym->st_info;

                    symbol_name = get_symbol_name(st_name_offset, aux->strtab_ptr, aux->strtab_size);
                    stack_symbnode(&aux->symbol_list, create_symbnode(symbol_name, st_value, st_info, st_shndx));
                }
            }
            else
            {
                num_symbol = aux->symtab_size / sizeof(Elf64_Sym);
                Elf64_Sym *sym_table_ptr = (Elf64_Sym *)aux->symtab_ptr;
                for (i = 0; i < num_symbol; i++)
                {
                    Elf64_Sym *sym = &sym_table_ptr[i];

                    uint32_t st_name_offset = get_elf_u32(sym->st_name, aux->endianness);
                    uint64_t st_value = get_elf_u64(sym->st_value, aux->endianness);
                    uint16_t st_shndx = get_elf_u16(sym->st_shndx, aux->endianness);
                    unsigned char st_info = sym->st_info;

                    symbol_name = get_symbol_name(st_name_offset, aux->strtab_ptr, aux->strtab_size);
                    stack_symbnode(&aux->symbol_list, create_symbnode(symbol_name, st_value, st_info, st_shndx));
                }
            }
        }
        else if (aux->validity == 1 && aux->elf == 1 && (aux->strtab_ptr == NULL || aux->symtab_ptr == NULL))
            save_file_error(aux, "no symbols");
        aux = aux->next;
    }
}