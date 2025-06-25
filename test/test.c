#include <stdio.h>

void func()
{
    static int static_var;
    printf("Hellow\n");
}

int main()
{
    func();
    return (0);
}