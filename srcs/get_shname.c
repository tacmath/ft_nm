#include "ft_nm.h"

/*
** Elf64_Shdr getShHead(t_fileData *fileData, char *name)
**
** permet de recuperer le section header a partir de son nom
** si il n'existe par renvoie un Elf64_Shdr header vide
*/

Elf64_Shdr *getShHead(t_fileData *fileData, char *name) {
    int n;

    n = -1;
    while (++n < fileData->head->e_shnum)
        if (!ft_strcmp(&fileData->ShStrTab[fileData->shead[n].sh_name], name))
        	return (&fileData->shead[n]);
    return(0);
}

Elf32_Shdr *getShHead32(t_fileData *fileData, char *name) {
    int n;
    Elf32_Shdr *shead;
	Elf32_Ehdr *head;

	head = (Elf32_Ehdr *)fileData->head;
	shead = (Elf32_Shdr *)fileData->shead;
    n = -1;
    while (++n < head->e_shnum)
        if (!ft_strcmp(&fileData->ShStrTab[shead[n].sh_name], name))
        	return (&shead[n]);
    return(0);
}