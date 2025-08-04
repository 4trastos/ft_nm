#include "../incl/ft_nm.h"

void    *ft_memcpy(void *dst, const void *src, size_t n)
{
    char *d;
    const char *s;
    size_t i;
	
	d = dst;
	s = src;
	i = 0;

    while (i < n) {
        d[i] = s[i];
        i++;
    }
    return (dst);
}

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*j;
	size_t			i;

	j = b;
	i = 0;
	while (i < len)
	{
		j[i] = (unsigned char)c;
		i++;
	}
	return (b);
}

void    save_file_error(t_stack_file *file, char *msg)
{
    if (file == NULL)
        return;

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

int ft_memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *x;
    const unsigned char *y;
    size_t              i;

    x = s1;
    y = s2;
    i = 0;
    while (i < n)
    {
        if (x[i] != y[i])
            return (x[i] - y[i]);
        i++;
    }
    return (0);
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while ((s1[i] != '\0' || s2[i] != '\0') && i < n)
	{
		if (s1[i] != s2[i])
			return ((const unsigned char) s1[i] - (const unsigned char)s2[i]);
		i ++;
	}
	return (0);
}