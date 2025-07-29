#include "../incl/ft_nm.h"

int     checkarg(char *file_name, char *program_name)
{
    struct stat my_file_info;
    int         fd;
    int         success;
    
    if (findflags(file_name))
        return (1);
    fd = open(file_name, O_RDONLY); // Si se puede abrir y si se puede leer
    if (fd == -1)
    {
        handle_file_error(program_name, file_name, errno);
        return (0);
    }
    success = fstat(fd, &my_file_info); //(file status) Llamada al sistema POSIX para obtener los metadatos de un archivo.
    if (success == -1)
    {
        handle_file_error(program_name, file_name, errno);
        close(fd);
        return (0);
    }
    if (S_ISDIR(my_file_info.st_mode))
    {
        putstr_stderr(program_name);
        putstr_stderr(": warning: ");
        putstr_stderr(file_name);
        putstr_stderr(": is a directory\n");
        close(fd);
        return (0);
    }
    
    close(fd);
    return (1);
}

void    create_list(t_stack_file **sfile, char **str, int *flag, char *program_name)
{
    int i = 0;
    int validity = 1;

    while (str[i] != NULL)
    {   
        validity = checkarg(str[i], program_name);
        if (!validity)
            *flag += 1;
        stack_node(sfile, create_node(str[i], i, validity));
        validity = 1;
        i++;
    }
    return;
}

int main(int argc, char **argv)
{
    t_stack_file    *sfile;
    int             flag;
    char            *program_name_str;

    sfile = NULL;
    flag = 0;
    program_name_str = argv[0];
    if (argc == 1)
    {
        putstr_stderr("ft_nm: a.out: No such file\n");
        return (1);
    }
    init_host_endianness();
    argv++;
    create_list(&sfile, argv, &flag, program_name_str);
    if (flag && argc == 2)
        return(1);
    flag = 0;
    fileFormat_id(&sfile, flag);        // Identifica 32/64 bits y endianness
    parsing_header(&sfile);             // Mapea la cabecera ELF del archivo en RAM
    location_headings(&sfile);          // Mapea la tabla de cabeceras de sección (SHT)
    location_names(&sfile);             // Mapea la tabla de nombres de sección (.shstrtab)
    iterytable(&sfile);                 // Busca y mapea las tablas de símbolos (.symtab) y strings (.strtab)
    parsing_symbol_ent(&sfile);         // Parsea los símbolos del archivo y los guarda en la lista de símbolos 
    extr_detc_symbol_type(&sfile);      // Asigna el carácter de tipo (ej., 'T', 'D', 'U') a cada símbolo
    tilter_collecting(&sfile);          // Filtra símbolos
    ordering_symbols(&sfile);           // Ordena símbolos alfabéticamente
    ft_output(&sfile, argc);               // Imprime la salida final de nm
    
    //print_stack_files(sfile);
    
    clear_closing(&sfile);

    return (0);
}