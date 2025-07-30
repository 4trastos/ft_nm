#include "../incl/ft_nm.h"

bool    compare_symbols(t_symbol_info *a, t_symbol_info *b)
{
    int name_cmp;

    name_cmp = ft_strcmp(a->name, b->name);
    if (name_cmp < 0)
        return (true);
    else if (name_cmp > 0)
        return (false);

    if (a->value < b->value)
        return (true);
    else
        return (false);
}

t_symbol_info   *ft_merge(t_symbol_info *a, t_symbol_info *b)
{
    t_symbol_info *result;

    result = NULL;

    if (a == NULL)
        return (b);
    if (b == NULL)
        return (a);
    
    if (compare_symbols(a, b))
    {
        result = a;
        result->next = ft_merge(a->next, b);
    }
    else
    {
        result = b;
        if (ft_strcmp(a->name, b->name) == 0 && a->value == b->value)
            result = a;
        
        t_symbol_info *next_a;
        t_symbol_info *next_b;

        if (result == a)
            next_a = a->next;
        else
            next_a = a;

        if (result == b)
            next_b = b->next;
        else
            next_b = b;

        result->next = ft_merge(next_a, next_b);
    }
    return (result);
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