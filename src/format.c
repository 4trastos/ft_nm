#include "../incl/ft_nm.h"
#include <sys/mman.h>

void    find_endianness(unsigned char *elf, t_stack_file *aux)
{
    if (elf[EI_DATA] == ELFDATA2LSB)
        aux->endianness = ENDIAN_LSB;
    else if (elf[EI_DATA] == ELFDATA2MSB)
        aux->endianness = ENDIAN_MSB;
    else
    {
        aux->endianness = ENDIAN_UNKNOWN;
        aux->validity = 0;
    }
}

void    find_bits(unsigned char *elf, t_stack_file *aux)
{
    if (elf[EI_CLASS] == ELFCLASS32)
        aux->bits = BITS_32;
    else if (elf[EI_CLASS] == ELFCLASS64)
        aux->bits = BITS_64;
    else
        aux->validity = 0;
}

void    ft_fileFormat_id(t_stack_file **sfile, int flag)
{
    struct stat     my_file_info;
    t_stack_file    *aux;
    unsigned char   *elf;
    int             success;
    int             fd;

    aux = *sfile;
    while (aux)
    {
        if (aux->validity == 1 && aux->type == ARG_TYPE_FILE)
        {
            fd = open(aux->file, O_RDONLY);
            if (fd == -1)
            {
                ft_handle_file_error("./ft_nm", aux->file, errno);
                flag = 1;    
            }
            if (!flag)
            {
                success = fstat(fd, &my_file_info);
                if (success == -1)
                    ft_handle_file_error("./ft_nm", aux->file, errno);
                else
                {
                    elf = mmap(NULL, my_file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0); // nmap: carga el contenido del ELF en memoria RAM.
                    if (elf == MAP_FAILED)
                        ft_handle_file_error("mmap failed:", aux->file, errno);
                    else if (my_file_info.st_size < 4 || elf[0] != 0x7F || elf[1] != 'E' || elf[2] != 'L' || elf[3] != 'F')
                        ft_handle_file_error_two("./ft_nm", aux->file, "file format not recognized");
                    else
                    {
                        aux->elf = 1;
                        aux->file_size = my_file_info.st_size;
                        aux->file_content_ptr = elf;
                        find_bits(elf, aux);
                        find_endianness(elf, aux);
                    }
                }
            }
            flag = 0;
            close(fd);
        }
        aux = aux->next;
    } 
}