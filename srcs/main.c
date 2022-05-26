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
    Elf64_Shdr *section;
    int n;

    if (fileData->head->e_shoff + fileData->head->e_shnum * fileData->head->e_shentsize > fileData->fileSize)
        return (fileErrors(": ", name, ": File format not recognized\n"));
    fileData->shead = (void*)fileData->head + fileData->head->e_shoff;
    fileData->ShStrTab = (void*)fileData->head + fileData->shead[fileData->head->e_shstrndx].sh_offset;
    section = getShHead(fileData, ".strtab");
    fileData->strTab = (void*)fileData->head + section->sh_offset;
    section = getShHead(fileData, ".symtab");
    fileData->symTab = (void*)fileData->head + section->sh_offset;
    fileData->symTabSize = section->sh_size / sizeof(Elf64_Sym);
    fileData->symbols_nb = 0;
    n = -1;
    while (++n < fileData->symTabSize)
        if (fileData->symTab[n].st_name && ELF64_ST_TYPE(fileData->symTab[n].st_info) != STT_FILE)
            fileData->symbols_nb++;
    return (1);
}

int getSymbols(t_fileData *fileData) {
    int n;
    int m;

    if (!(fileData->symbols = malloc(sizeof(t_sym) * fileData->symbols_nb)))
        return (0);
    m = 0;
    n = -1;
    while (++n < fileData->symTabSize) {
        if (fileData->symTab[n].st_name && ELF64_ST_TYPE(fileData->symTab[n].st_info) != STT_FILE) {
            fileData->symbols[m].name = &fileData->strTab[fileData->symTab[n].st_name];
            fileData->symbols[m].bind = ELF64_ST_BIND(fileData->symTab[n].st_info);
            fileData->symbols[m].type = ELF64_ST_TYPE(fileData->symTab[n].st_info);
            fileData->symbols[m].value = fileData->symTab[n].st_value;
            fileData->symbols[m].section = &fileData->ShStrTab[fileData->shead[fileData->symTab[n].st_shndx].sh_name];
            m++;
        }
    }
    return (1);
}

int nmFile(char *name, char mode) {
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
    ft_quicksort(fileData.symbols, fileData.symbols_nb);
    printSymbols(&fileData);
    free(fileData.symbols);
    munmap(fileData.head, fileData.fileSize);
    return (1);
}

int main(int ac, char **av) {
    int n;

    if (ac == 1) {
        write(2, "Wrong numbers of arguments\n", 27);
        return (1);
    }
    if (ac == 2 && !nmFile(av[1], 0))
        return (1);
    if (ac > 2) {
        n = 0;
        while (++n < ac)
            nmFile(av[n], 1);
    }
    return (0);
}
