#ifndef FT_NM_H
# define FT_NM_H

#include <elf.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct magic_num {
    uint32_t    magic_number;
    uint8_t     support;
    uint8_t     endian;
    uint8_t     version;
    uint8_t     abi;
}              t_magic;

typedef struct s_data t_data;

struct s_sym
{
    char *name;
    char *section;
    char   bind;
    char   type;
    size_t value;
};

typedef struct s_sym t_sym;

struct s_fileData
{
    int         fd;
    size_t      fileSize;
    Elf64_Ehdr  *head;
    Elf64_Shdr  *shead;
    t_sym       *symbols;
    int         symbols_nb;
    char        *ShStrTab;
};

typedef struct s_fileData t_fileData;

Elf64_Shdr  getShHead(t_fileData *fileData, char *name);
void        ft_bzero(void *s, size_t n);
void		ft_quicksort(t_sym *tab, int len);
int         ft_strcmp(const char *str1, const char *str2);

#endif