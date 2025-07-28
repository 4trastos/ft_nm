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
        aux = aux->next;
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

void    stack_symbnode(t_symbol_info **list, t_symbol_info *new)
{
    t_symbol_info   *aux;

    aux = *list;
    if (*list == NULL)
    {
        *list = new;
        return;
    }
    while (aux->next != NULL)
        aux = aux->next;
    aux->next = new;
    new->next = NULL;
}

t_symbol_info   *create_symbnode(char *name, uint64_t value, unsigned char st_info, uint16_t shndx)
{
    t_symbol_info   *new;

    new = (t_symbol_info *)malloc(sizeof(t_symbol_info));
    if (!new)
        return (NULL);
    new->name = ft_strdup(name);
    if (name && !new->name)
    {
        free(new);
        return (NULL);
    }
    new->value = value;
    new->shndx = shndx;
    new->st_info = st_info;
    new->char_type = '\0';
    new->next = NULL;
    return (new);
}