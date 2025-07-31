#include "../incl/ft_nm.h"
#include <sys/mman.h>

void    get_flags(char *file, t_nmflags *flag)
{
 
    if (ft_strcmp(file, "-a") == 0)
        *flag = NM_FLAG_A;
    else if (ft_strcmp(file, "-g") == 0)
        *flag = NM_FLAG_G;
    else if (ft_strcmp(file, "-u") == 0)
        *flag = NM_FLAG_U;
    else if (ft_strcmp(file, "-r") == 0)
        *flag = NM_FLAG_R;
    else
        *flag = NM_FLAG_P;
}

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

void    fileFormat_id(t_stack_file **sfile, int flag)
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
                handle_file_error("./ft_nm", aux->file, errno);
                flag = 1;    
            }
            if (flag == 0)
            {
                success = fstat(fd, &my_file_info);
                if (success == -1)
                    handle_file_error("./ft_nm", aux->file, errno);
                else
                {
                    elf = mmap(NULL, my_file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                    if (elf == MAP_FAILED)
                        handle_file_error("mmap failed:", aux->file, errno);
                    else if (my_file_info.st_size < 4 || elf[0] != 0x7F || elf[1] != 'E' || elf[2] != 'L' || elf[3] != 'F')
                        aux->elf = 0;
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
        else if (aux->validity == 1 && aux->type == ARG_TYPE_FLAG)
            get_flags(aux->file, &aux->flag);
        aux = aux->next;
    } 
}