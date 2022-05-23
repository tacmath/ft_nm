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

int getSymbols(t_fileData *fileData) {
    char *strtab;
    Elf64_Sym *symbols;
    Elf64_Shdr section;
    int n;
    int size;
    int m;

    section = getShHead(fileData, ".strtab");
    strtab = (void*)fileData->head + section.sh_offset;
    section = getShHead(fileData, ".symtab");
    symbols = (void*)fileData->head + section.sh_offset;
    size = section.sh_size / sizeof(Elf64_Sym);
    m = 0;
    n = -1;
    while (++n < size)
        if (symbols[n].st_name && symbols[n].st_info != 4)
            m++;
    fileData->symbols_nb = m;
    if (!(fileData->symbols = malloc(sizeof(t_sym) * fileData->symbols_nb)))
        return (0);
    m = 0;
    n = -1;
    while (++n < size) {
        if (symbols[n].st_name && symbols[n].st_info != 4) {
            fileData->symbols[m].name = &strtab[symbols[n].st_name];
            fileData->symbols[m].info = symbols[n].st_info;
            fileData->symbols[m].value = symbols[n].st_value;
            m++;
        }
    }
    return (1);
}

void printSymbols(t_fileData *fileData) {
    int n;

    n = -1;
    while (++n < fileData->symbols_nb) {
        if (fileData->symbols[n].value)
            printf("%016x %2d %s\n", fileData->symbols[n].value, fileData->symbols[n].info, fileData->symbols[n].name);
        else
            printf("                 %2d %s\n", fileData->symbols[n].info, fileData->symbols[n].name);
    }
}

int main(int ac, char **av) {
    t_fileData fileData;

    if (ac != 2) {
        write(2, "Wrong numbers of arguments\n", 27);
        return (1);
    }
    ft_bzero(&fileData, sizeof(fileData));
    if (!getFileData(&fileData, av[1]))
        return (1);
    getSymbols(&fileData);
    ft_quicksort(fileData.symbols, fileData.symbols_nb);
    printSymbols(&fileData);
    free(fileData.symbols);
    munmap(fileData.head, fileData.fileSize);
    return (0);
}