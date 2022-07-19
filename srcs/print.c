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

char checkSymbolType(t_sym symbol, char **sections, char global, char local) {
    int n;
    
    n = -1;
    while (sections[++n]) {
        if (!ft_strcmp(symbol.section, sections[n])) {
            if (symbol.bind == STB_GLOBAL)
                return (global);
            return (local);
        }
    }
    return (0);
}

char getSymbolChar(t_sym symbol) {
    char ret;

    if (symbol.bind == STB_WEAK) {
        if (symbol.value)
            return ('W');
        return ('w');
    }
    if (symbol.type == STT_FILE || (symbol.type == STT_NOTYPE && !symbol.section[0])) {
        if (symbol.bind == STB_GLOBAL)
            return ('A');
        return ('a');
    }
    if (symbol.section_type == SHT_NOBITS) {
        if (symbol.bind == STB_GLOBAL)
            return ('B');
        return ('b');
    }
    if (symbol.section_type == SHT_NOTE) {
        if (symbol.bind == STB_GLOBAL)
            return ('N');
        return ('n');
    }
    if ((symbol.section_flags & SHF_EXECINSTR) || symbol.section_type == SHT_INIT_ARRAY
        || symbol.section_type == SHT_FINI_ARRAY || symbol.section_type == SHT_PREINIT_ARRAY) {
        if (symbol.bind == STB_GLOBAL)
            return ('T');
        return ('t');
    }
    if (symbol.section_flags == (SHF_WRITE | SHF_ALLOC) /* && (symbol.section_type == SHT_PROGBITS || symbol.section_type == SHT_DYNAMIC)*/) {
        if (symbol.bind == STB_GLOBAL)
            return ('D');
        return ('d');
    }
    if (symbol.section_flags == SHF_ALLOC) {
        if (symbol.bind == STB_GLOBAL)
            return ('R');
        return ('r');
    }
    if (symbol.type == STT_COMMON)
        return ('C');
    if (!symbol.section[0]) {
        if (symbol.bind == STB_GLOBAL)
            return ('U');
        return ('u');
    }
    return ('?');
}

void printSymbols(t_fileData *fileData, t_option option) {
    int n;
    static char type[] = "   ";
    static char zeros[] = "0000000000000000";

    n = 0;
    while (++n < fileData->symbols_nb) {
        if ((option.u && (fileData->symbols[n].section[0] || fileData->symbols[n].type == STT_FILE || (fileData->symbols[n].type == STT_NOTYPE && fileData->symbols[n].bind < STB_WEAK)))
            || (!option.a && !option.u && (!fileData->symbols[n].originalName[0] || fileData->symbols[n].type == STT_FILE))
            || option.g && fileData->symbols[n].bind != STB_GLOBAL && fileData->symbols[n].bind != STB_WEAK)
            continue;
        if (fileData->symbols[n].section[0] || fileData->symbols[n].type == STT_FILE || (fileData->symbols[n].type == STT_NOTYPE
            && fileData->symbols[n].bind < STB_WEAK && !fileData->symbols[n].section[0])) {
            write(1, zeros, 8 + (!fileData->type) * 8 - hexNbrLen(fileData->symbols[n].value));
            printHexNbr(fileData->symbols[n].value);
        }
        else
            write(1, "                ", 8 + (!fileData->type) * 8);
        type[1] = getSymbolChar(fileData->symbols[n]);
        write(1, type, 3);
        write(1, fileData->symbols[n].name, ft_strlen(fileData->symbols[n].name));   
        write(1, "\n", 1);
   //     dprintf(1, "%d\n", fileData->symbols[n].shndx);
    }
}