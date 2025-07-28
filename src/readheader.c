#include "../incl/ft_nm.h"

const char  *get_section_name(uint32_t offset_name, void *shstrtab_ptr, size_t shstrtab_size)
{
    if (offset_name >= shstrtab_size)
        return (NULL);
    return ((const char*)(shstrtab_ptr + offset_name));
}

void    iterytable(t_stack_file **file)
{
    t_stack_file    *aux;
    uint16_t        sh_num;
    uint16_t        i;
    Elf32_Shdr      *shdr;
    Elf64_Shdr      *shdrr;
    const char      *section_name;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            aux->symtab_ptr = NULL;
            aux->strtab_ptr = NULL;
            aux->symtab_size = 0;
            aux->strtab_size = 0;

            if (aux->bits == BITS_32)
            {
                sh_num = get_elf_u16(aux->elf32_header->e_shnum, aux->endianness);
                for (i = 0; i < sh_num; i++)
                {
                    shdr = &aux->elf32_sh_table[i];
                    section_name = get_section_name(get_elf_u32(shdr->sh_name, aux->endianness), aux->shstrtab_ptr, aux->shstrtab_size);
                    if (get_elf_u32(shdr->sh_type, aux->endianness == SHT_SYMTAB))
                    {
                        if (aux->symtab_ptr == NULL)
                        {
                            aux->symtab_ptr = aux->file_content_ptr + get_elf_u32(shdr->sh_offset, aux->endianness);
                            aux->symtab_size = get_elf_u32(shdr->sh_size, aux->endianness);
                            aux->symtab_link = get_elf_u32(shdr->sh_link, aux->endianness);
                        }
                    }
                    else if (get_elf_u32(shdr->sh_type, aux->endianness) == SHT_STRTAB)
                    {
                        if (section_name && ft_strcmp(section_name, ".strtab") == 0)
                        {
                            if (aux->shstrtab_ptr == NULL)
                            {
                                aux->shstrtab_ptr = aux->file_content_ptr + get_elf_u32(shdr->sh_offset, aux->endianness);
                                aux->shstrtab_size = get_elf_u32(shdr->sh_size, aux->endianness);
                            }
                        }
                    }
                }
            }
            else
            {
                sh_num = get_elf_u16(aux->elf64_header->e_shnum, aux->endianness);
                for (i = 0; i < sh_num; i++)
                {
                    shdrr = &aux->elf64_sh_table[i];
                    section_name = get_section_name(get_elf_u32(shdrr->sh_name, aux->endianness), aux->shstrtab_ptr, aux->shstrtab_size);
                    if (section_name)
                        printf("  Section[%d]: %s (Type: %u)\n", i, section_name, get_elf_u32(shdrr->sh_type, aux->endianness));
                    else
                        printf("  Section[%d]: <Invalid Name Offset> (Type: %u)\n", i, get_elf_u32(shdrr->sh_name, aux->endianness));
                    if (get_elf_u32(shdrr->sh_type, aux->endianness) == SHT_SYMTAB)
                    {
                        if (aux->symtab_ptr == NULL)
                        {
                            aux->symtab_ptr = aux->file_content_ptr + get_elf_u64(shdrr->sh_offset, aux->endianness);
                            aux->symtab_size = get_elf_u64(shdrr->sh_size, aux->endianness);
                            aux->symtab_link = get_elf_u64(shdrr->sh_link, aux->endianness);
                        }
                    }
                    else if (get_elf_u32(shdrr->sh_type, aux->endianness) == SHT_STRTAB)
                    {
                        if (section_name && ft_strcmp(section_name, ".strtab") == 0)
                        {
                            if (aux->strtab_ptr == NULL)
                            {
                                aux->strtab_ptr = aux->file_content_ptr + get_elf_u64(shdrr->sh_offset, aux->endianness);
                                aux->strtab_size = get_elf_u64(shdrr->sh_size, aux->endianness);
                            }
                        }
                    }
                }
            }
            if (!aux->shstrtab_ptr || !aux->symtab_link)
            {
                handle_file_error_two("./ft_nm:", aux->file, "Error: Required .symtab or .strtab section not found.");
                aux->validity = 0;
            }
        }
        aux = aux->next;
    }
}