#include "../incl/ft_nm.h"

bool compare_symbols(t_symbol_info *a, t_symbol_info *b)
{
    int cmp = ignore_underscores(a->name, b->name);

    if (cmp < 0)
        return (true);
    if (cmp > 0)
        return (false);

    return (a->value < b->value);
}

t_symbol_info   *ft_merge(t_symbol_info *a, t_symbol_info *b)
{
    if (a == NULL)
        return (b);
    if (b == NULL)
        return (a);
    
    if (compare_symbols(a, b))
    {
        a->next = ft_merge(a->next, b);
        return (a);
    }
    else
    {
        b->next = ft_merge(a, b->next);
        return (b);
    }
}

void    ft_split_list(t_symbol_info *head, t_symbol_info **front, t_symbol_info **back)
{
    t_symbol_info *slow;
    t_symbol_info *fast;

    slow = head;
    fast = head->next;

    // fast avanza dos pasos, slow un paso
    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = head;              // La primera mitad va desde la cabeza
    *back = slow->next;         // La segunda mitad empieza después de slow
    slow->next = NULL;    
}

void    merge_sort(t_symbol_info **list)
{
    t_symbol_info   *head;
    t_symbol_info   *a;
    t_symbol_info   *b;

    head = *list;

    if ((head == NULL) || (head->next == NULL))
        return;
    
    ft_split_list(head, &a ,&b);

    merge_sort(&a);
    merge_sort(&b);

    *list = ft_merge(a, b);
}

void    ordering_symbols(t_stack_file **file)
{
    t_stack_file    *aux;

    aux = *file;
    while (aux)
    {
        if (aux->validity == 1 && aux->elf == 1 && aux->symbol_list != NULL)
            merge_sort(&aux->symbol_list);
        aux = aux->next;
    }
}