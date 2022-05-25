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

int fileErrors(char *preMessage, char *name, char *afterMessage) {
    write(1, FT_NM_NAME, sizeof(FT_NM_NAME));
    write(1, preMessage, ft_strlen(preMessage));
    write(1, name, ft_strlen(name));
    write(1, afterMessage, ft_strlen(afterMessage));
    return (0);
}

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
    if (fileData->head->e_shoff + fileData->head->e_shnum * fileData->head->e_shentsize > fileData->fileSize)
        return (fileErrors(": ", name, ": File format not recognized\n"));
    fileData->shead = (void*)fileData->head + fileData->head->e_shoff;
    fileData->ShStrTab = (void*)fileData->head + fileData->shead[fileData->head->e_shstrndx].sh_offset;
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
            fileData->symbols[m].bind = ELF64_ST_BIND(symbols[n].st_info);
            fileData->symbols[m].type = ELF64_ST_TYPE(symbols[n].st_info);
            fileData->symbols[m].value = symbols[n].st_value;
            fileData->symbols[m].section = &fileData->ShStrTab[fileData->shead[symbols[n].st_shndx].sh_name];
            m++;
        }
    }
    return (1);
}

char getSymbolChar(t_sym symbol) {
  //  printf("section = %5s, bind = %d, type = %d\n", symbol.section, symbol.bind, symbol.type);
    if (symbol.bind == STB_WEAK) {
        if (symbol.value)
            return ('W');
        return ('w');
    }
    if (!ft_strcmp(symbol.section, ".bss")) {
        if (symbol.bind == STB_GLOBAL)
            return ('B');
        return ('b');
    }
    if (!ft_strcmp(symbol.section, ".data") || !ft_strcmp(symbol.section, ".jcr")) {
        if (symbol.bind == STB_GLOBAL)
            return ('D');
        return ('d');
    }
    if (!ft_strcmp(symbol.section, ".rodata") || !ft_strcmp(symbol.section, ".eh_frame") || !ft_strcmp(symbol.section, ".eh_frame_hdr")) {
        if (symbol.bind == STB_GLOBAL)
            return ('R');
        return ('r');
    }
    if (!ft_strcmp(symbol.section, ".dynamic") || !ft_strcmp(symbol.section, ".got.plt")) {
        if (symbol.bind == STB_GLOBAL)
            return ('D');
        return ('d');
    }
    if (!ft_strcmp(symbol.section, ".text") || !ft_strcmp(symbol.section, ".init_array") || !ft_strcmp(symbol.section, ".fini_array") || !ft_strcmp(symbol.section, ".init") || !ft_strcmp(symbol.section, ".fini")) {
        if (symbol.bind == STB_GLOBAL)
            return ('T');
        return ('t');
    }
    if (symbol.type == STT_COMMON)
        return ('C');
    if (symbol.type == STT_NOTYPE || symbol.type < STT_NUM) {
        if (symbol.bind == STB_GLOBAL)
            return ('U');
        return ('u');
    }
    return ('?');
}

void printHexNbr(size_t nb) {
    static char hex[] = "0123456789abcdef";

    if (nb >= 16) {
        printHexNbr(nb / 16);
        printHexNbr(nb % 16);
    }
    else
        write(1, hex + nb, 1);
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

void printSymbols(t_fileData *fileData) {
    int n;
    static char type[] = "   ";
    static char zeros[] = "0000000000000000";

    n = -1;
    while (++n < fileData->symbols_nb) {
        if (fileData->symbols[n].value) {
            write(1, zeros, 16 - hexNbrLen(fileData->symbols[n].value));
            printHexNbr(fileData->symbols[n].value);
        }
        else
            write(1, "                ", 16);
        type[1] = getSymbolChar(fileData->symbols[n]);
        write(1, type, 3);
        write(1, fileData->symbols[n].name, ft_strlen(fileData->symbols[n].name));
        write(1, "\n", 1);
    }
}

int nmFile(char *name, char mode) {
    t_fileData fileData;

    ft_bzero(&fileData, sizeof(fileData));
    if (!getFileData(&fileData, name))
        return (0);
    if (fileData.type) {
        write(1, "32 bits\n", 9);
        return (0);
    }
    if (!checkFileData64(&fileData, name))
        return (0);
    if (mode) {
        write(1, "\n", 1);
        write(1, name, ft_strlen(name));
        write(1, ":\n", 2);
    }
    getSymbols(&fileData);
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
