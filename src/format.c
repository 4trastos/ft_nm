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
                success = fstat(fd, &my_file_info); //optenemos el tamaño del archivo en memoria
                if (success == -1)
                    ft_handle_file_error("./ft_nm:", aux->file, errno);
                else
                {
                    elf = mmap(NULL, my_file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0); // Mapear el ARCHIVO COMPLETO (my_file_info.st_size) en memoria usando mmap.
                    if (elf == MAP_FAILED)
                        ft_handle_file_error("mmap failed:", aux->file, errno);
                    aux->file_size = my_file_info.st_size;
                    aux->file_content_ptr = elf;
                    printf("Este es el puntero de nmap: %p  ==>>  Tamaño del archivo: %zu\n", aux->file_content_ptr, aux->file_size);
                    printf("Verificar si los bytes iniciales (elf[0] a elf[3]) corresponden a los 'magic bytes' de ELF.\n");
                    printf("si es un ELF válido, entonces leer más del encabezado (usando la estructura ElfXX_Ehdr y el puntero elf) para determinar 32/64 bits, endianness, etc.\n");
                    printf("Almacenar este puntero elf y el tamaño del mapeo (my_file_info.st_size) en el nodo t_stack_file correspondiente. \n");
                    printf("\n");
                }
            }
            flag = 0;
            close(fd);
        }
        aux = aux->next;
    } 
}