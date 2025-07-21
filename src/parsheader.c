#include "../incl/ft_nm.h"

void    location_names(t_stack_file **files)
{
    t_stack_file    *aux;
    Elf32_Shdr      *shstrtab_shdr;
    Elf64_Shdr      *shstrtab_shdr64;
    uint16_t        shstrtab_idx;       // index de la sección
    uint64_t        sh_offset;          // Offset de la sección
    size_t          sh_size;            // Tamaño de la sección

    aux = *files;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            if (aux->bits == BITS_32)
            {
                shstrtab_idx = get_elf_u16(aux->elf32_header->e_shstrndx, aux->endianness);
                shstrtab_shdr = &aux->elf32_sh_table[shstrtab_idx];
                sh_offset = get_elf_u32(shstrtab_shdr->sh_offset, aux->endianness);
                sh_size = get_elf_u32(shstrtab_shdr->sh_size, aux->endianness);
            }
            else
            {
                shstrtab_idx = get_elf_u16(aux->elf64_header->e_shstrndx, aux->endianness);
                shstrtab_shdr64 = &aux->elf64_sh_table[shstrtab_idx];
                sh_offset = get_elf_u64(shstrtab_shdr64->sh_offset, aux->endianness);
                sh_size = get_elf_u64(shstrtab_shdr64->sh_size, aux->endianness);
            }
            if (sh_offset >= aux->file_size || (sh_offset + sh_size) > aux->file_size || sh_size == 0)
            {
                handle_file_error_two("./ft_nm", aux->file, "Error: Invalid .shstrtab section offset or size");
                aux->validity = 0;
            }
            else
            {
                aux->shstrtag_ptr = aux->file_content_ptr + sh_offset;
                aux->shstrtab_size = sh_size;
            }
        }
        aux = aux->next;
    }
}

void    location_headings(t_stack_file **files) //Localizar Tabla de Cabeceras de Sección (SHT)
{
    t_stack_file    *aux;
    uint64_t        sh_offset;
    uint16_t        sh_num;
    uint16_t        sh_entsize;

    aux = *files;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            sh_offset = 0;
            sh_num = 0;
            sh_entsize = 0;
            if (aux->bits == BITS_32)
            {
                sh_offset = get_elf_u32(aux->elf32_header->e_shoff, aux->endianness);
                sh_num = get_elf_u16(aux->elf32_header->e_shnum, aux->endianness);
                sh_entsize = get_elf_u16(aux->elf32_header->e_shentsize, aux->endianness);
            }
            else
            {
                sh_offset = get_elf_u64(aux->elf64_header->e_shoff, aux->endianness);
                sh_num = get_elf_u16(aux->elf64_header->e_shnum, aux->endianness);
                sh_entsize = get_elf_u16(aux->elf64_header->e_shentsize, aux->endianness);
            }
            if (sh_num == 0 || sh_entsize == 0 || sh_offset == 0 || sh_offset >= aux->file_size ||
                (sh_offset + sh_num * sh_entsize) > aux->file_size)
            {
                handle_file_error_two("./ft_nm", aux->file, "Error: Section headers on ELF file");
                aux->validity = 0;
            }
            if (aux->validity)
            {
                if (aux->bits == BITS_32)
                    aux->elf32_sh_table = (Elf32_Shdr*)(aux->file_content_ptr + sh_offset);
                else
                    aux->elf64_sh_table = (Elf64_Shdr*)(aux->file_content_ptr + sh_offset);
            }
        }
        aux = aux->next;
    }
}

void    parsing_header(t_stack_file **files)
{
    t_stack_file    *aux;

    aux = *files;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->file_content_ptr != NULL)
        {
            if (aux->bits == BITS_32)
                aux->elf32_header = (Elf32_Ehdr*)aux->file_content_ptr;
            else if (aux->bits == BITS_64)
                aux->elf64_header = (Elf64_Ehdr*)aux->file_content_ptr;
            else
            {
                aux->validity = 0;
                printf("ft_nm: %s: Error: Unknown bitness for file \n", aux->file);
                return;
            }
        }
        aux = aux->next;
    }
}