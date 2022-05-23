#ifndef FT_NM_H
# define FT_NM_H

#include <elf.h>
#include <unistd.h>
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

struct s_fileData
{
    int         fd;
    size_t      fileSize;
    Elf64_Ehdr  *head;
    Elf64_Shdr  *shead;
    char        *ShStrTab;
};

typedef struct s_fileData t_fileData;

Elf64_Shdr getShHead(t_fileData *fileData, char *name);

#endif