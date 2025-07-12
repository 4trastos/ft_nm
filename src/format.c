#include "../incl/ft_nm.h"
#include <sys/mman.h>

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
                ft_handle_file_error("./ft_nm:", aux->file, errno);
                flag = 1;    
            }
            if (!flag)
            {
                success = fstat(fd, &my_file_info);
                if (success == -1)
                    ft_handle_file_error("./ft_nm:", aux->file, errno);
                else
                {
                    elf = mmap(NULL, my_file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0); // nmap: carga el contenido del ELF en memoria RAM.
                    if (elf == MAP_FAILED)
                        ft_handle_file_error("mmap failed:", aux->file, errno);
                    if (my_file_info.st_size < 4 || elf[0] != 0x7F || elf[1] != 'E' || elf[2] != 'L' || elf[3] != 'F')
                    {
                        ft_handle_file_error("Not an ELF file:  \n", aux->file, errno);
                        flag = 1;
                    }
                    else
                    {
                        aux->elf = 1;
                        aux->file_size = my_file_info.st_size;
                        aux->file_content_ptr = elf;
                        printf("\nES UN ARCHIVO ELF VÁLIDO: ====>>> %02X, %02X, %02X, %02X\n", elf[0], elf[1], elf[2], elf[3]);
                        printf("La dirección de ELF en la RAM: %p  ==>>  Tamaño del archivo: %zu\n", aux->file_content_ptr, aux->file_size);
                        printf("si es un ELF válido, entonces leer más del header (usando la estructura ElfXX_Ehdr y el puntero elf) para determinar 32/64 bits, endianness, etc.\n");
                        printf("\n");
                    }
                }
            }
            flag = 0;
            close(fd);
        }
        aux = aux->next;
    } 
}