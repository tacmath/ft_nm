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

char getGlobalSymbol(t_sym symbol, char c) {
    if (symbol.bind == STB_GLOBAL)
        return (c - ('a' - 'A'));
    return (c);
}

char getSymbolChar(t_sym symbol) {
    if (symbol.bind == STB_WEAK && symbol.type == STT_OBJECT) {
        if (symbol.shndx)
            return ('V');
        return ('v');
    }
    if (symbol.bind == STB_WEAK) {
        if (symbol.shndx)
            return ('W');
        return ('w');
    }
    if (symbol.type == STT_GNU_IFUNC)
        return ('i');
    if (symbol.type == STT_FILE || !symbol.section[0] && symbol.shndx)
        return (getGlobalSymbol(symbol, 'a'));
    if (symbol.section_type == SHT_NOBITS)
        return (getGlobalSymbol(symbol, 'b'));
    if (symbol.section_flags == (SHF_MERGE | SHF_STRINGS) || (!symbol.section_flags && symbol.shndx))
        return (getGlobalSymbol(symbol, 'n'));
    if ((symbol.section_flags & SHF_EXECINSTR))
        return (getGlobalSymbol(symbol, 't'));
    if (symbol.section_flags == (SHF_WRITE | SHF_ALLOC))
        return (getGlobalSymbol(symbol, 'd'));
    if ((symbol.section_flags & SHF_ALLOC))
        return (getGlobalSymbol(symbol, 'r'));
    if (symbol.type == STT_COMMON)
        return ('C');
    if (!symbol.shndx)
        return (getGlobalSymbol(symbol, 'u'));
    return ('?');
}

void printSymbols(t_fileData *fileData, t_option option) {
    int n;
    static char type[] = "   ";
    static char zeros[] = "0000000000000000";

    n = 0;
    while (++n < fileData->symbols_nb) {
        if ((option.u && (fileData->symbols[n].shndx || fileData->symbols[n].type == STT_FILE
            || (fileData->symbols[n].type == STT_NOTYPE && fileData->symbols[n].bind < STB_WEAK)))
            || (!option.a && !option.u && (!fileData->symbols[n].originalName[0] || fileData->symbols[n].type == STT_FILE))
            || option.g && fileData->symbols[n].bind != STB_GLOBAL && fileData->symbols[n].bind != STB_WEAK)
            continue;
        if (fileData->symbols[n].shndx || fileData->symbols[n].type == STT_FILE) {
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