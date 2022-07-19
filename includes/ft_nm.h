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

#define FT_NM_NAME      "ft_nm"
#define MAGIC_NUMBER    0x464c457f
#define ELF64           0
#define ELF32           1

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
    char *originalName;
    char *section;
    int   section_type;
    int   section_flags;
    char   bind;
    char   type;
    size_t value;
};

typedef struct s_sym t_sym;

struct s_option
{
    char a;
    char g;
    char u;
    char r;
    char p;
};

typedef struct s_option t_option;

struct s_fileData
{
    int         fd;
    size_t      fileSize;
    Elf64_Ehdr  *head;
    Elf64_Shdr  *shead;
    Elf64_Sym   *symTab;
    t_sym       *symbols;
    int         symbols_nb;
    char        *ShStrTab;
    char        *strTab;
    char        type;
};

typedef struct s_fileData t_fileData;

void        ft_bzero(void *s, size_t n);
void		ft_quicksort(t_sym *tab, int len);
void		ft_rquicksort(t_sym *tab, int len);
int         ft_strcmp(const char *str1, const char *str2);
size_t      ft_strlen(const	char *str);
int         fileErrors(char *preMessage, char *name, char *afterMessage);
size_t      hexNbrLen(size_t nb);
char        getSymbolChar(t_sym symbol);
void        printSymbols(t_fileData *fileData, t_option option);
int         getSymbols32(t_fileData *fileData);
int         checkFileData32(t_fileData *fileData, char *name);

#endif