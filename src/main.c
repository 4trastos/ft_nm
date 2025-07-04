#include "../incl/ft_nm.h"

int     ft_checkarg(char *file_name, char *program_name)
{
    struct stat my_file_info;
    int         fd;
    int         success;
    
    if (ft_findflags(file_name))
        return (1);
    fd = open(file_name, O_RDONLY); // Si se puede abrir y si se puede leer
    if (fd == -1)
    {
        ft_handle_file_error(program_name, file_name, errno);
        return (0);
    }
    success = fstat(fd, &my_file_info); //(file status) es una llamada al sistema POSIX que se utiliza para obtener información detallada sobre un archivo.
    if (success == -1)
    {
        ft_handle_file_error(program_name, file_name, errno);
        close(fd);
        return (0);
    }
    if (S_ISDIR(my_file_info.st_mode))
    {
        ft_putstr_stderr(program_name);
        ft_putstr_stderr(": warning: ");
        ft_putstr_stderr(file_name);
        ft_putstr_stderr(": is a directory\n");
        close(fd);
        return (0);
    }
    
    close(fd);
    return (1);
}

void    ft_create_list(t_stack_file **sfile, char **str, int *flag, char *program_name)
{
    int i = 0;
    int validity = 1;

    while (str[i] != NULL)
    {   
        validity = ft_checkarg(str[i], program_name);
        if (!validity)
            *flag += 1;
        ft_stack_node(sfile, ft_create_node(str[i], i, validity));
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
        ft_putstr_stderr("ft_nm: a.out: No such file or directory\n");
        return (1);
    }
    argv++;
    ft_create_list(&sfile, argv, &flag, program_name_str);
    if (flag && argc == 2)
        return(1);
    ft_print_stack_files(sfile);
    flag = 0;
    ft_fileFormat_id(&sfile, flag); // (formato ELF);
    write(1, "Formato de archivo identificado\n", 32);
    //ft_nmap(argv);
    //ft_file_Format_Id();
    //ft_parsing_header();
    //ft_location_headings();
    //ft_location_names();
    //ft_location_symbol();
    //ft_parsing_symbol_ent();
    //ft_extr_detc_symbol_type();
    //ft_tilter_collecting();
    //ft_ordering_symbols();
    //ft_output();
    //ft_clear_closing();
    write(1, "OK\n", 3);

    return (0);
}