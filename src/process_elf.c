#include "../incl/ft_nm.h"

char    *clean_ar_name(const char *raw_name)
{
    char    *name;
    size_t  i;

    i = 0;
    while (i < 16 && raw_name[i] && raw_name[i] != '/' && !ft_isspace(raw_name[i]))
        i++;
    
    name = (char *)malloc(sizeof(char) * (i + 1));
    if (!name)
        return (NULL); // Manejo de error de malloc
    
    ft_memcpy(name, raw_name, i);
    name[i] = '\0';
    
    return (name);
}
void    process_file_list(t_stack_file **sfile)
{
    t_stack_file    *aux;

    aux = *sfile;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1)
            process_elf_file(aux);
        else if (aux->validity == 1 && aux->elf == 2)
            process_static_archive(aux);
        aux = aux->next;
    }
}

void    process_elf_file(t_stack_file *sfile)
{
    parsing_header(&sfile);             // Mapea la cabecera ELF del archivo en RAM
    location_headings(&sfile);          // Mapea la tabla de cabeceras de sección (SHT)
    location_names(&sfile);             // Mapea la tabla de nombres de sección (.shstrtab)
    iterytable(&sfile);                 // Busca y mapea las tablas de símbolos (.symtab) y strings (.strtab)
    parsing_symbol_ent(&sfile);         // Parsea los símbolos del archivo y los guarda en la lista de símbolos 
    extr_detc_symbol_type(&sfile);      // Asigna el carácter de tipo (ej., 'T', 'D', 'U') a cada símbolo
    tilter_collecting(&sfile);          // Filtra símbolos
    ordering_symbols(&sfile);           // Ordena símbolos alfabéticamente

}

void    process_static_archive(t_stack_file *sfile)
{
    t_ar_hdr        *ar_header;
    t_stack_file    *temp_node;
    t_stack_file    *current_member_node;
    char            *member_ptr;
    size_t          offset;
    size_t          member_size;

    offset = SARMAG;
    sfile->dinamic_bin = NULL;

    while (offset < sfile->file_size)
    {
        ar_header = (t_ar_hdr*)(sfile->file_content_ptr + offset);
        member_size = ft_atoi_base(ar_header->ar_size, 10);
        member_ptr = (char *)(sfile->file_content_ptr + offset + sizeof(t_ar_hdr));
        
        // CORRECCIÓN: Ignorar el miembro especial de la tabla de símbolos
        if (ft_strncmp(ar_header->ar_name, "//", 2) == 0 || ft_strncmp(ar_header->ar_name, "/\x00", 2) == 0)
        {
            offset += sizeof(t_ar_hdr) + member_size;
            if (member_size % 2 != 0)
                offset++;
            continue;
        }

        temp_node = (t_stack_file*)malloc(sizeof(t_stack_file));
        if (!temp_node)
        {
            save_file_error(sfile, "Memory allocation failed");
            return;
        }
        
        ft_memset(temp_node, 0, sizeof(t_stack_file));
        
        // CORRECCIÓN: Usar la función corregida
        temp_node->file = clean_ar_name(ar_header->ar_name);
        if (!temp_node->file) {
            free(temp_node);
            save_file_error(sfile, "Memory allocation failed");
            return;
        }

        temp_node->file_content_ptr = (unsigned char*)member_ptr;
        temp_node->file_size = member_size;
        temp_node->elf = 1;
        temp_node->validity = 1;

        if (member_size >= 4 && member_ptr[0] == 0x7F && member_ptr[1] == 'E' && member_ptr[2] == 'L' && member_ptr[3] == 'F')    
        {
            find_bits((unsigned char*)member_ptr, temp_node);
            find_endianness((unsigned char*)member_ptr, temp_node);
            process_elf_file(temp_node);
        } 
        else
        {
            temp_node->elf = 0;
            save_file_error(temp_node, "file format not recognized");
        }

        if (sfile->dinamic_bin == NULL)
            sfile->dinamic_bin = temp_node;
        else
        {
            current_member_node = sfile->dinamic_bin;
            while (current_member_node->next)
                current_member_node = current_member_node->next;
            current_member_node->next = temp_node;
        }

        offset += sizeof(t_ar_hdr) + member_size;
        if (member_size % 2 != 0)
            offset++;
    }
}