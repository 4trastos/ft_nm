#include "../incl/ft_nm.h"

int findflags(char *str)
{
    int i;

    i = 0;
    if (str[0] != '-')
        return (0);
    i++;
    if (str[i + 1] != '\0')
        return (0);
    if (str[i] != 'a' && str[i] != 'g' && str[i] != 'u' && str[i] != 'r' && str[i] != 'p')
        return (0);
    return (1);
}

void    stack_node(t_stack_file **sfile, t_stack_file *new)
{
    t_stack_file    *aux;

    aux = *sfile;
    if (*sfile == NULL)
    {
        *sfile = new;
        return ;   
    }
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    aux->next = new;
    new->next = NULL;
}

t_stack_file    *create_node(char *str, int pos, int status)
{
    t_stack_file    *new;

    new = (t_stack_file *)malloc(sizeof(t_stack_file));
    if (!new)
        return (NULL);
    new->file = str;
    new->position = pos + 1;
    new->validity = status;
    if (findflags(str))
        new->type = ARG_TYPE_FLAG;
    else
        new->type = ARG_TYPE_FILE;
    new->next = NULL;
    return (new);
}
