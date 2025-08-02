#include "../incl/ft_nm.h"

void    save_file_error(t_stack_file *file, char *msg)
{
    if (file == NULL)
        return;

    //file->validity = 0;
    if (file->error_msg)
        free (file->error_msg);
    file->error_msg = ft_strdup(msg);
}

void    ft_puthex(unsigned long n, int width)
{
    char hex_digits[] = "0123456789abcdef";
    char buffer[width + 1];
    int i = width;

    buffer[i] = '\0';
    if (n == 0)
    {
        while (i > 0)
            buffer[--i] = '0';
    }
    else
    {
        while (i > 0)
        {
            buffer[--i] = hex_digits[n % 16];
            n /= 16;
        }
    }
    write(1, buffer, width);
}