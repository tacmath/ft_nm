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
    static char *dataSections[9] = {".data", ".jcr", ".dynamic", ".got", ".got.plt", 0};
    static char *rdataSections[19] = {".rodata", ".eh_frame", ".eh_frame_hdr", ".dynstr", ".dynsym", ".gnu.hash", ".gnu.version",
                                        ".gnu.version_r", ".rela.dyn", ".rela.plt", ".note.ABI-tag", ".note.gnu.build-id", ".interp", 
                                        ".rel.dyn", ".rel.plt", 0};
    static char *textSections[12] = {".text", ".init_array", ".fini_array", ".init", ".fini", ".plt", ".plt.got", 0};
    char ret;

    if (symbol.bind == STB_WEAK) {
        if (symbol.value)
            return ('W');
        return ('w');
    }
    if (symbol.type == STT_FILE) {
        if (symbol.bind == STB_GLOBAL)
            return ('A');
        return ('a');
    }
    if (!ft_strcmp(symbol.section, ".bss")) {
        if (symbol.bind == STB_GLOBAL)
            return ('B');
        return ('b');
    }
    if (!ft_strcmp(symbol.section, ".comment")) {
        if (symbol.bind == STB_GLOBAL)
            return ('N');
        return ('n');
    }
    if ((ret = checkSymbolType(symbol, dataSections, 'D', 'd'))) 
        return (ret);
    if ((ret = checkSymbolType(symbol, rdataSections, 'R', 'r'))) 
        return (ret);
    if ((ret = checkSymbolType(symbol, textSections, 'T', 't'))) 
        return (ret);
    if (symbol.type == STT_COMMON)
        return ('C');
    if (!symbol.section[0]/*symbol.type == STT_NOTYPE || symbol.type < STT_NUM*/) {
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
        if ((option.u && (fileData->symbols[n].section[0] || fileData->symbols[n].type == STT_FILE))
            || (!option.a && !option.u && (!fileData->symbols[n].originalName[0] || fileData->symbols[n].type == STT_FILE)))
            continue;
        if (fileData->symbols[n].section[0] || fileData->symbols[n].type == STT_FILE) {
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