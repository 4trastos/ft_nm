#include "../incl/ft_nm.h"

int     ft_strcmp(const char *s1, char *s2)
{
    int i = 0;

    if (!s1 || !s2)
        return (-1);
    while (s1[i] != '\0' && s2[i] != '\0')
    {
        if (s1[i] != s2[i])
            return (s1[i] - s2[i]);
        i++;
    }
    return (0);
}

void    clear_closing(t_stack_file **sfiles)
{
    t_stack_file    *current;
    t_stack_file    *next_file;

    if (!sfiles || !*sfiles)
        return;
    
    current = *sfiles;
    while (current != NULL)
    {
        next_file = current->next;
        if (current->file_content_ptr)
        {
            if (munmap(current->file_content_ptr, current->file_size) == -1)
                perror("munmap failed");
        }
        clear_symbol_list(&current->symbol_list);
        free(current->error_msg);
        free(current);
        current = next_file;
    }
    *sfiles = NULL;
}

void    putstr_stderr(char *str)
{
    int i = 0;

    while (str[i] != '\0')
    {
        write(2, &str[i], 1);
        i++;
    }
}

void    handle_file_error(char *program_name, char *file_name, int errnum)
{
    putstr_stderr(program_name);
    putstr_stderr(": '");

    putstr_stderr(file_name);
    putstr_stderr("': ");

    putstr_stderr(strerror(errnum));
    write(2, "\n", 1);
}

void    handle_file_error_two(char *program_name, char *file_name, char *str)
{
    putstr_stderr(program_name);
    putstr_stderr(": ");

    if (ft_strcmp("Is a directory", str) == 0)
    {
        putstr_stderr("Warning: «");
        putstr_stderr(file_name);
        putstr_stderr("» : ");

        putstr_stderr(str);
        write(2, "\n", 1);
    }
    else
    {
        putstr_stderr(file_name);
        putstr_stderr(": ");
    
        putstr_stderr(str);
        write(2, "\n", 1);
    }
}
