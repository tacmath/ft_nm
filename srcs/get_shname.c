#include "ft_nm.h"


int	ft_strcmp(const char *str1, const char *str2)
{
	int					n;

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

/*
** Elf64_Shdr getShHead(t_fileData *fileData, char *name)
**
** permet de recuperer le section header a partir de son nom
** si il n'existe par renvoie un Elf64_Shdr header vide
*/

Elf64_Shdr getShHead(t_fileData *fileData, char *name) {
    int n;
    Elf64_Shdr shead;

    ft_bzero(&shead, sizeof(shead));
    n = -1;
    while (++n < fileData->head->e_shnum && ft_strcmp(&fileData->ShStrTab[fileData->shead[n].sh_name], name))
        ;
    if (!ft_strcmp(&fileData->ShStrTab[fileData->shead[n].sh_name], name))
        shead = fileData->shead[n];
    return(shead);
}