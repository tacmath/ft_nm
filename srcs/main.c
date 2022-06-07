#include "ft_nm.h"
#include <stdio.h>

int getFileData(t_fileData *fileData, char *name) {
    struct stat ret;
    t_magic *magic;

    if ((fileData->fd = open(name, O_RDONLY)) <= 0)
        return (fileErrors(": '", name, ":' No such file\n"));
    if (fstat(fileData->fd, &ret))
        return (fileErrors(": '", name, ":' No such file\n"));
    if (!S_ISREG(ret.st_mode))
        return (fileErrors(": Warning: '", name, "' is not an ordinary file\n"));
    fileData->fileSize = ret.st_size;
    if (!(fileData->head = mmap(0, fileData->fileSize, PROT_READ, MAP_PRIVATE, fileData->fd, 0)))
        return (0);
    close(fileData->fd);
    magic = (t_magic*)fileData->head;
    if (magic->magic_number != MAGIC_NUMBER)
        return (fileErrors(": ", name, ": File format not recognized\n"));
    fileData->type = (magic->support == ELF32);
    return (1);
}

int checkFileData64(t_fileData *fileData, char *name) {
    int n;

    if (fileData->head->e_shoff + fileData->head->e_shnum * fileData->head->e_shentsize > fileData->fileSize)
        return (fileErrors(": ", name, ": File format not recognized\n"));
    fileData->shead = (void*)fileData->head + fileData->head->e_shoff;
    fileData->ShStrTab = (void*)fileData->head + fileData->shead[fileData->head->e_shstrndx].sh_offset;
    fileData->strTab = (void*)fileData->head + fileData->shead[fileData->head->e_shstrndx - 1].sh_offset;
    fileData->symTab = (void*)fileData->head + fileData->shead[fileData->head->e_shstrndx - 2].sh_offset;
    fileData->symbols_nb = fileData->shead[fileData->head->e_shstrndx - 2].sh_size / sizeof(Elf64_Sym);
    return (1);
}

int getSymbols(t_fileData *fileData) {
    int n;
    int m;

    if (!(fileData->symbols = malloc(sizeof(t_sym) * fileData->symbols_nb)))
        return (0);
    n = -1;
    while (++n < fileData->symbols_nb) {
        fileData->symbols[n].name = &fileData->strTab[fileData->symTab[n].st_name];
        fileData->symbols[n].originalName = fileData->symbols[n].name;
        fileData->symbols[n].bind = ELF64_ST_BIND(fileData->symTab[n].st_info);
        fileData->symbols[n].type = ELF64_ST_TYPE(fileData->symTab[n].st_info);
        fileData->symbols[n].value = fileData->symTab[n].st_value;
        if (fileData->symTab[n].st_shndx < fileData->head->e_shnum)
            fileData->symbols[n].section = &fileData->ShStrTab[fileData->shead[fileData->symTab[n].st_shndx].sh_name];
        else
            fileData->symbols[n].section = &fileData->ShStrTab[fileData->shead[0].sh_name];
        if (!fileData->symbols[n].name[0])
            fileData->symbols[n].name = fileData->symbols[n].section;
    }
    return (1);
}

int nmFile(char *name, t_option option, char mode) {
    t_fileData fileData;

    ft_bzero(&fileData, sizeof(fileData));
    if (!getFileData(&fileData, name))
        return (0);
    if (fileData.type) {
        if (!checkFileData32(&fileData, name))
            return (0);
        getSymbols32(&fileData);
    }
    else {
        if (!checkFileData64(&fileData, name))
            return (0);
        getSymbols(&fileData);
    }
    if (mode) {
        write(1, "\n", 1);
        write(1, name, ft_strlen(name));
        write(1, ":\n", 2);
    }
    if (!option.p) {
        if (option.r)
            ft_rquicksort(fileData.symbols, fileData.symbols_nb);
        else
            ft_quicksort(fileData.symbols, fileData.symbols_nb);
    }
    printSymbols(&fileData, option);
    free(fileData.symbols);
    munmap(fileData.head, fileData.fileSize);
    return (1);
}

void getOption(t_option *option, int ac, char **av) {
    int n;
    int m;

    n = 0;
    while (++n < ac) {
        if (av[n][0] == '-') {
            m = 0;
            while (av[n][++m]) {
                if (av[n][m] == 'a')
                    option->a = 1;
                if (av[n][m] == 'g')
                    option->g = 1;
                if (av[n][m] == 'p')
                    option->p = 1;
                if (av[n][m] == 'r')
                    option->r = 1;
                if (av[n][m] == 'u')
                    option->u = 1;
            }
        }
    }
}

int getfilesNb(int ac, char **av) {
    int n;
    int nb;

    nb = 0;
    n = 0;
    while (++n < ac)
        if (av[n][0] != '-')
            nb++;
    return (nb);
}

int main(int ac, char **av) {
    int n;
    int filesNb;
    t_option option;

    ft_bzero(&option, sizeof(option));
    getOption(&option, ac, av);
    filesNb = getfilesNb(ac, av);
    if (!filesNb) {
        if (!nmFile("a.out", option, 0))
            return (1);
    }
    else {
        n = 0;
        while (++n < ac)
            if (av[n][0] != '-') 
                nmFile(av[n], option, (filesNb != 1));
    }
    return (0);
}
