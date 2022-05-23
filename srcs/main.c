#include "ft_nm.h"
#include <stdio.h>

size_t	ft_strlen(const	char *str)
{
	size_t nb;

	nb = 0;
	while (*(str++) != '\0')
		nb++;
	return (nb);
}

int getFileData(t_fileData *fileData, char *name) {
    struct stat ret;

    if ((fileData->fd = open(name, O_RDONLY)) <= 0)
        return (0);
    if (fstat(fileData->fd, &ret))
        return (0);
    fileData->fileSize = ret.st_size;
    if (!(fileData->head = mmap(0, fileData->fileSize, PROT_READ, MAP_PRIVATE, fileData->fd, 0)))
        return (0);
    if (fileData->head->e_shoff + fileData->head->e_shnum * fileData->head->e_shentsize > fileData->fileSize)
        return (0);
    fileData->shead = (void*)fileData->head + fileData->head->e_shoff;
    fileData->ShStrTab = (void*)fileData->head + fileData->shead[fileData->head->e_shstrndx].sh_offset;
    close(fileData->fd);
    return (1);
}

int printSymbols(t_fileData *fileData) {
    char *strtab;
    Elf64_Sym *symbols;
    Elf64_Shdr section;
    int n;
    int size;

    section = getShHead(fileData, ".strtab");
    strtab = (void*)fileData->head + section.sh_offset;
    section = getShHead(fileData, ".symtab");
    symbols = (void*)fileData->head + section.sh_offset;
    size = section.sh_size / sizeof(Elf64_Sym);
    n = -1;
    while (++n < size) {
        if (symbols[n].st_name) {
            write(1, &strtab[symbols[n].st_name], ft_strlen(&strtab[symbols[n].st_name]));
            write(1, "\n", 1);
        }
    }
    return (1);
}

int main(int ac, char **av) {
    t_fileData fileData;

    if (ac != 2) {
        write(2, "Wrong numbers of arguments\n", 27);
        return (1);
    }
    if (!getFileData(&fileData, av[1]))
        return (1);
    printSymbols(&fileData);
    return (0);
}