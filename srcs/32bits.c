#include "ft_nm.h"

int getSymbols32(t_fileData *fileData) {
    Elf32_Shdr *shead;
    Elf32_Sym *symTab;
    int n;
    int m;

    symTab = (Elf32_Sym*)fileData->symTab;
    shead = (Elf32_Shdr*)fileData->shead;
    if (!(fileData->symbols = malloc(sizeof(t_sym) * fileData->symbols_nb)))
        return (0);
    m = 0;
    n = -1;
    while (++n < fileData->symTabSize) {
        if (symTab[n].st_name && ELF32_ST_TYPE(symTab[n].st_info) != STT_FILE) {
            fileData->symbols[m].name = &fileData->strTab[symTab[n].st_name];
            fileData->symbols[m].bind = ELF32_ST_BIND(symTab[n].st_info);
            fileData->symbols[m].type = ELF32_ST_TYPE(symTab[n].st_info);
            fileData->symbols[m].value = symTab[n].st_value;
            fileData->symbols[m].section = &fileData->ShStrTab[shead[symTab[n].st_shndx].sh_name];
            m++;
        }
    }
    return (1);
}

int checkFileData32(t_fileData *fileData, char *name) {
	Elf32_Ehdr *head;
    Elf32_Shdr *shead;
    Elf32_Shdr *section;
    Elf32_Sym *symTab;
    int n;

	head = (Elf32_Ehdr *)fileData->head;
    if (head->e_shoff + head->e_shnum * head->e_shentsize > fileData->fileSize)
        return (fileErrors(": ", name, ": File format not recognized\n"));
    fileData->shead = (void*)head + head->e_shoff;
    shead = (Elf32_Shdr *)fileData->shead;
    fileData->ShStrTab = (void*)head + shead[head->e_shstrndx].sh_offset;
    section = getShHead32(fileData, ".strtab");
    fileData->strTab = (void*)head + section->sh_offset;
    section = getShHead32(fileData, ".symtab");
    fileData->symTab = (void*)head + section->sh_offset;
    fileData->symTabSize = section->sh_size / sizeof(Elf32_Sym);
    fileData->symbols_nb = 0;
    symTab = (void*)fileData->symTab;
    n = -1;
    while (++n < fileData->symTabSize)
        if (symTab[n].st_name && ELF32_ST_TYPE(symTab[n].st_info) != STT_FILE)
            fileData->symbols_nb++;
    return (1);
}