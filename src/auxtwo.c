#include "../incl/ft_nm.h"

size_t  ft_strlen(char *str)
{
    size_t  i;

    i = 0;
    while (str[i] != '\0')
        i++;
    return (i);
}

char    *ft_strdup(char *str)
{
    char    *new;
    size_t  i;
    
    if (!str)
        return (NULL);
    new = (char *)malloc(ft_strlen(str) + 1);
    if (!new)
        return (NULL);
    i = 0;
    while (str[i] != '\0')
    {
        new[i] = str[i];
        i++;
    }
    new[i] = '\0';
    return (new);
}

void    clear_symbol_list(t_symbol_info **list)
{
    t_symbol_info   *current;
    t_symbol_info   *next;

    current = *list;
    while (current != NULL)
    {
        next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    *list = NULL;
}

char    ft_tolower(char c)
{
    if (c >= 'A' && c <= 'Z')
        c = c + 32;
    return (c);   
}