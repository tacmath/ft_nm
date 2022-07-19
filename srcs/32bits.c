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
    ft_bzero(fileData->symbols, sizeof(t_sym) * fileData->symbols_nb);
    n = -1;
    while (++n < fileData->symbols_nb) {
        fileData->symbols[n].name = &fileData->strTab[symTab[n].st_name];
        fileData->symbols[n].originalName = fileData->symbols[n].name;
        fileData->symbols[n].bind = ELF32_ST_BIND(symTab[n].st_info);
        fileData->symbols[n].type = ELF32_ST_TYPE(symTab[n].st_info);
        fileData->symbols[n].value = symTab[n].st_value;
        if (symTab[n].st_shndx < head->e_shnum) {
            fileData->symbols[n].section = &fileData->ShStrTab[shead[symTab[n].st_shndx].sh_name];
            fileData->symbols[n].section_type = shead[symTab[n].st_shndx].sh_type;
            fileData->symbols[n].section_flags = shead[symTab[n].st_shndx].sh_flags;
        }
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
    int n;

	head = (Elf32_Ehdr *)fileData->head;
    if (head->e_shoff + head->e_shnum * head->e_shentsize > fileData->fileSize)
        return (fileErrors(": ", name, ": File format not recognized\n"));
    fileData->shead = (void*)head + head->e_shoff;
    shead = (Elf32_Shdr *)fileData->shead;
    fileData->ShStrTab = (void*)head + shead[head->e_shstrndx].sh_offset;
    fileData->ShStrTab = (void*)fileData->head + shead[head->e_shstrndx].sh_offset;
    fileData->strTab = (void*)fileData->head + shead[head->e_shstrndx - 1].sh_offset;
    fileData->symTab = (void*)fileData->head + shead[head->e_shstrndx - 2].sh_offset;
    if (ft_strcmp(&fileData->ShStrTab[shead[head->e_shstrndx - 2].sh_name], ".symtab"))
        return (fileErrors(": ", name, ": no symbols\n"));
    fileData->symbols_nb = shead[head->e_shstrndx - 2].sh_size / sizeof(Elf32_Sym);
    return (1);
}