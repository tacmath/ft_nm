#include "ft_nm.h"

size_t	ft_strlen(const	char *str)
{
	size_t nb;

	nb = 0;
	while (*(str++) != '\0')
		nb++;
	return (nb);
}

int	ft_strcmp(const char *str1, const char *str2)
{
	int					n;

	if (!str1 || !str2)
		return (-1);
	n = 0;
	while (str1[n] == str2[n] && (str1[n] != '\0' || str2[n] != '\0'))
		n++;
	return (str1[n] - str2[n]);
}

void	ft_bzero(void *s, size_t n)
{
	int		i;
	char	*str;

	i = -1;
	str = s;
	while (++i < n)
		str[i] = '\0';
}

void		ft_quicksort(t_sym *tab, int len)
{
	char	*compa;
	t_sym	tmp;
	int		n;
	int		m;

	if (len < 2)
		return ;
	compa = tab[(len - 1)].name;
	m = 0;
	n = -1;
	while (++n < len)
		if (ft_strcmp(tab[n].name, compa) <= 0)
		{
			if (m != n) {
				tmp = tab[m];
				tab[m] = tab[n];
				tab[n] = tmp;
			}
			m++;
		}
	ft_quicksort(tab, --m);
	ft_quicksort(&tab[m], len - m);
}

void		ft_rquicksort(t_sym *tab, int len)
{
	char	*compa;
	t_sym	tmp;
	int		n;
	int		m;

	if (len < 2)
		return ;
	compa = tab[(len - 1)].name;
	m = 0;
	n = -1;
	while (++n < len)
		if (ft_strcmp(tab[n].name, compa) >= 0)
		{
			if (m != n) {
				tmp = tab[m];
				tab[m] = tab[n];
				tab[n] = tmp;
			}
			m++;
		}
	ft_rquicksort(tab, --m);
	ft_rquicksort(&tab[m], len - m);
}

size_t hexNbrLen(size_t nb) {
    size_t len;

    len = 1;
    nb /= 16;
    while(nb) {
        nb /= 16;
        len++;
    }
    return (len);
}