#include "../incl/ft_nm.h"

int	ft_isspace(char c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}

int	ft_isvalid(char c, int base)
{
	char	*lower;
	char	*upper;

	lower = "0123456789abcdef";
	upper = "0123456789ABCDEF";
	while (base--)
	{
		if (lower[base] == c || upper[base] == c)
			return (1);
	}
	return (0);
}

int	ft_value_of(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return (0);
}

int ft_atoi_base(const char *str, int base)
{
	int	sign;
	int	result;

	sign = 1;
	result = 0;
	if (base < 2 || base > 16)
		return (0);
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (ft_isvalid(*str, base))
	{
		result = result * base + ft_value_of(*str);
		str++;
	}
	return (result * sign);
}