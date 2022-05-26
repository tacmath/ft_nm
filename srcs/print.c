#include "ft_nm.h"

int fileErrors(char *preMessage, char *name, char *afterMessage) {
    write(1, FT_NM_NAME, sizeof(FT_NM_NAME));
    write(1, preMessage, ft_strlen(preMessage));
    write(1, name, ft_strlen(name));
    write(1, afterMessage, ft_strlen(afterMessage));
    return (0);
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

void printSymbols(t_fileData *fileData) {
    int n;
    static char type[] = "   ";
    static char zeros[] = "0000000000000000";

    n = -1;
    while (++n < fileData->symbols_nb) {
        if (fileData->symbols[n].value) {
            write(1, zeros, 8 + (!fileData->type) * 8 - hexNbrLen(fileData->symbols[n].value));
            printHexNbr(fileData->symbols[n].value);
        }
        else
            write(1, "                ", 8 + (!fileData->type) * 8);
        type[1] = getSymbolChar(fileData->symbols[n]);
        write(1, type, 3);
        write(1, fileData->symbols[n].name, ft_strlen(fileData->symbols[n].name));
        write(1, "\n", 1);
    }
}