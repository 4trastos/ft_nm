#include "../incl/ft_nm.h"

void    clear_closing(t_stack_file **files)
{
    t_stack_file *aux;
    
    aux = *files;
    while (aux)
    {
        *files = aux->next;
        free(aux);
        aux = *files;
    }
    *files = NULL;
}

void    putstr_stderr(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        write(2, &str[i], 1);
        i++;
    }
    return;
}

void    handle_file_error(char *program_name, char *file_name, int errnum)
{
    putstr_stderr(program_name);
    putstr_stderr(": ");

    putstr_stderr(file_name);
    putstr_stderr(": ");

    putstr_stderr(strerror(errnum));
    putstr_stderr("\n");
    return;
}

void    handle_file_error_two(char *program_name, char *file_name, char *str)
{
    putstr_stderr(program_name);
    putstr_stderr(": ");

    putstr_stderr(file_name);
    putstr_stderr(": ");

    putstr_stderr(str);
    putstr_stderr("\n");
    return;
}
