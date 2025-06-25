#include "../incl/ft_nm.h"

void    ft_putstr_stderr(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        write(2, &str[i], 1);
        i++;
    }
    return;
}

void    ft_handle_file_error(char *program_name, char *file_name, int errnum)
{
    ft_putstr_stderr(program_name);
    ft_putstr_stderr(": ");

    ft_putstr_stderr(file_name);
    ft_putstr_stderr(": ");

    ft_putstr_stderr(strerror(errnum));
    ft_putstr_stderr("\n");
    return;
}
