#include "ft_nm.h"

int getSymbols32(t_fileData *fileData) {
    Elf32_Ehdr *head;
    Elf32_Shdr *shead;
    Elf32_Sym *symTab;
    int n;
    int m;

    symTab = (Elf32_Sym*)fileData->symTab;
    head = (Elf32_Ehdr*)fileData->head;
    shead = (Elf32_Shdr*)fileData->shead;
    if (!(fileData->symbols = malloc(sizeof(t_sym) * fileData->symbols_nb)))
        return (0);
    n = -1;
    while (++n < fileData->symbols_nb) {
        fileData->symbols[n].name = &fileData->strTab[symTab[n].st_name];
        fileData->symbols[n].originalName = fileData->symbols[n].name;
        fileData->symbols[n].bind = ELF32_ST_BIND(symTab[n].st_info);
        fileData->symbols[n].type = ELF32_ST_TYPE(symTab[n].st_info);
        fileData->symbols[n].value = symTab[n].st_value;
        if (symTab[n].st_shndx < head->e_shnum)
            fileData->symbols[n].section = &fileData->ShStrTab[shead[symTab[n].st_shndx].sh_name];
        else
            fileData->symbols[n].section = &fileData->ShStrTab[shead[0].sh_name];
        if (!fileData->symbols[n].name[0])
            fileData->symbols[n].name = fileData->symbols[n].section;
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
    fileData->symbols_nb = section->sh_size / sizeof(Elf32_Sym);
    return (1);
}