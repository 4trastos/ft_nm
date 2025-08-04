#include "../incl/ft_nm.h"

int     checkarg(char *file_name)
{
    struct stat my_file_info;
    int         fd;
    int         success;
    
    if (findflags(file_name))
        return (1);
    fd = open(file_name, O_RDONLY);
    if (fd == -1)
        return (errno == EISDIR ? -2 : -1);
    success = fstat(fd, &my_file_info); //(file status) Llamada al sistema POSIX para obtener los metadatos de un archivo.
    if (success == -1)
    {
        close(fd);
        return (-3);
    }
    if (S_ISDIR(my_file_info.st_mode))
    {
        close(fd);
        return (-2);
    }
    
    close(fd);
    return (1);
}

void    create_list(t_stack_file **sfile, char **str, int *flag)
{
    int i = 0;
    int validity = 1;

    while (str[i] != NULL)
    {   
        validity = checkarg(str[i]);
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

    sfile = NULL;
    flag = 0;
    if (argc == 1)
    {
        putstr_stderr("ft_nm: «a.out»: No such file\n");
        return (1);
    }
    init_host_endianness();
    argv++;
    create_list(&sfile, argv, &flag);
    flag = 0;
    fileFormat_id(&sfile, flag);        // Identifica 32/64 bits y endianness
    process_file_list(&sfile); 
    ft_output(&sfile, argc);
    
    clear_closing(&sfile);
    
    return (0);
}