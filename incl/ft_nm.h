#ifndef FT_NM_H
# define FT_NM_H

# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <limits.h>
# include <stdbool.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <errno.h>
# include <stdint.h>
# include <elf.h>
# include <byteswap.h>

typedef enum e_host_endianness
{
    HOST_ENDIAN_UNKNOWN = 0,
    HOST_ENDIAN_LSB,
    HOST_ENDIAN_MSB,
    
} t_host_endianness;

extern t_host_endianness g_host_endianness;

typedef enum e_endianness
{
    ENDIAN_UNKNOWN = 0,
    ENDIAN_LSB,             // Little-endian (menos significativo primero)
    ENDIAN_MSB,             // Big-endian (más significativo primero)
}   t_endianness;

typedef enum e_bits
{
    BITS_32 = 32,
    BITS_64 = 64,
}   t_bits;

typedef enum e_argtype
{
    ARG_TYPE_UNKNOWN = 0,
    ARG_TYPE_FILE,
    ARG_TYPE_FLAG,
}   t_argtype;

typedef enum e_nmflags
{
    NM_ARG_FILE = 0,        //00000000 = 0 decimal (no hay transistores encendidos)
    NM_FLAG_A = 1 << 0,     //00000001 = 1 decimal (el bit 0 está encendido)
    NM_FLAG_G = 1 << 1,     //00000010 = 2 decinal (el bit 1 está encendido)
    NM_FLAG_U = 1 << 2,     //00000100 = 4 decimal (el bit 2 está encendido) 
    NM_FLAG_R = 1 << 3,     //00001000 = 8 decimal (el bit 3 está encendido) 
    NM_FLAG_P = 1 << 4,     //00010000 = 16 decimal (el bit 4 está encendido)
}   t_nmflags;

typedef struct s_stack_file
{
    char                *file;
    int                 validity;
    int                 position;
    int                 elf;
    unsigned char       *file_content_ptr;
    size_t              file_size;
    t_nmflags           flag;
    t_argtype           type;
    t_bits              bits;
    t_endianness        endianness;
    Elf32_Ehdr          *elf32_header;
    Elf64_Ehdr          *elf64_header;
    Elf32_Shdr          *elf32_sh_table;
    Elf64_Shdr          *elf64_sh_table;
    void                *shstrtag_ptr;
    size_t              shstrtab_size;
    void                *symtab_ptr;
    size_t              symtab_size;
    void                *strtab_ptr;
    size_t              strtab_size;
    struct s_stack_file *next;
}   t_stack_file;

//*** Functions to reverse the order of bytes ***
uint16_t    swap16(uint16_t val);
uint32_t    swap32(uint32_t val);
uint64_t     swap64(uint64_t val);

//*** Generic functions to obtain ELF values applying swap if necessary ***
uint16_t    get_elf_u16(uint16_t val_from_elf, t_endianness file_endianness);
uint32_t    get_elf_u32(uint32_t val_from_elf, t_endianness file_endianness);
uint64_t    get_elf_u64(uint64_t val_from_elf, t_endianness file_endianness);

//*** Function to determine the endianness of the host ***

void        init_host_endianness(void);

//*** auxiliary functions ***

void            putstr_stderr(char *str);
void            handle_file_error(char *program_name, char *file_name, int errnum);
void            handle_file_error_two(char *program_name, char *file_name, char *str);
char            *ft_split(char **str, char c);
int             findflags(char *str);
int             ft_strcmp(char *s1, char *s2);

//*** strcut functions ***

void            stack_node(t_stack_file **sfile, t_stack_file *new);
t_stack_file    *create_node(char *str, int pos, int status);
void            print_stack_files(t_stack_file *sfile);
void            clear_closing(t_stack_file **files);


//*** explicit functions ***

void            fileFormat_id(t_stack_file **sfile, int flag);
void            parsing_header(t_stack_file **files);
void            location_headings(t_stack_file **files);
void            location_names(t_stack_file **files);

#endif