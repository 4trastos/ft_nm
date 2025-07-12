#include "../incl/ft_nm.h"

int ft_findflags(char *str)
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

void    ft_stack_node(t_stack_file **sfile, t_stack_file *new)
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

t_stack_file    *ft_create_node(char *str, int pos, int status)
{
    t_stack_file    *new;

    new = (t_stack_file *)malloc(sizeof(t_stack_file));
    if (!new)
        return (NULL);
    new->file = str;
    new->position = pos + 1;
    new->validity = status;
    if (ft_findflags(str))
        new->type = ARG_TYPE_FLAG;
    else
        new->type = ARG_TYPE_FILE;
    new->next = NULL;
    return (new);
}

void    ft_print_stack_files(t_stack_file *sfile)
{
    t_stack_file *current = sfile;
    int node_count = 0;

    printf("\n--- Contenido del Stack de Archivos ---\n");
    if (current == NULL)
    {
        printf("El stack está vacío.\n");
        return;
    }

    while (current != NULL)
    {
        printf("\nNodo %d: \nArchivo '%s', \nPosición original: %d, \nVálido: %s, \nTipo: %d, \nFlag: %d, \nELF: %s\n",
               node_count, current->file, current->position, current->validity ? "Sí" : "No", current->type, current->flag, 
               current->elf ? "Sí" : "No");
        current = current->next;
        node_count++;
    }
    printf("--- Fin del Stack (Total: %d nodos) ---\n\n", node_count);
}