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
# include <sys/mman.h>

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

typedef struct s_symbol_info
{
    char                    *name;
    char                    char_type;
    unsigned char           st_info;
    uint64_t                value;
    uint16_t                shndx;
    bool                    visible;
    struct s_symbol_info    *next;
}   t_symbol_info;

typedef struct s_stack_file
{
    char                    *file;
    int                     validity;
    int                     position;
    int                     elf;
    unsigned char           *file_content_ptr;
    size_t                  file_size;
    t_nmflags               flag;
    t_argtype               type;
    t_bits                  bits;
    t_endianness            endianness;
    t_symbol_info           *symbol_list;
    Elf32_Ehdr              *elf32_header;
    Elf64_Ehdr              *elf64_header;
    Elf32_Shdr              *elf32_sh_table;
    Elf64_Shdr              *elf64_sh_table;
    void                    *shstrtab_ptr;
    void                    *strtab_ptr;
    void                    *symtab_ptr;
    size_t                  shstrtab_size;
    size_t                  strtab_size;
    size_t                  symtab_size;
    uint32_t                symtab_link;
    struct s_stack_file     *next;
}   t_stack_file;

//*** Functions to reverse the order of bytes ***
uint16_t        swap16(uint16_t val);
uint32_t        swap32(uint32_t val);
uint64_t        swap64(uint64_t val);

//*** Generic functions to obtain ELF values applying swap if necessary ***
uint16_t        get_elf_u16(uint16_t val_from_elf, t_endianness file_endianness);
uint32_t        get_elf_u32(uint32_t val_from_elf, t_endianness file_endianness);
uint64_t        get_elf_u64(uint64_t val_from_elf, t_endianness file_endianness);

//*** Function to determine the endianness of the host ***

void            init_host_endianness(void);

//*** auxiliary functions ***

void            putstr_stderr(char *str);
void            handle_file_error(char *program_name, char *file_name, int errnum);
void            handle_file_error_two(char *program_name, char *file_name, char *str);
char            *ft_split(char **str, char c);
char            *ft_strdup(char *str);
int             findflags(char *str);
int             ft_strcmp(const char *s1, char *s2);
size_t          ft_strlen(char *str);
char            *get_symbol_name(uint32_t offset_name, void *strtab_ptr, size_t strtab_size);
char            ft_tolower(char c);
const char      *get_section_name(uint32_t offset_name, void *shstrtab_ptr, size_t shstrtab_size);

//*** struct functions ***

t_stack_file    *create_node(char *str, int pos, int status);
t_symbol_info   *create_symbnode(char *name, uint64_t value, unsigned char st_info, uint16_t shndx);
void            stack_node(t_stack_file **sfile, t_stack_file *new);
void            stack_symbnode(t_symbol_info **list, t_symbol_info *new);
void            print_stack_files(t_stack_file *sfile);
void            clear_closing(t_stack_file **sfiles);
void            clear_symbol_list(t_symbol_info **list);

//*** explicit functions ***

void            fileFormat_id(t_stack_file **sfile, int flag);
void            parsing_header(t_stack_file **files);
void            location_headings(t_stack_file **files);
void            location_names(t_stack_file **files);
void            iterytable(t_stack_file **file);
void            parsing_symbol_ent(t_stack_file **file);
void            extr_detc_symbol_type(t_stack_file **file);
void            logic_deter_symbol(t_symbol_info *sym, uint16_t shndx, unsigned char type, unsigned char binding);
void            logic_deterc_type(bool lower, t_symbol_info *sym, unsigned char type, unsigned char shndx);
void            tilter_collecting(t_stack_file **file);
void            ordering_symbols(t_stack_file **file);
void            merge_sort(t_symbol_info **list);
void            ft_split_list(t_symbol_info *head, t_symbol_info **front, t_symbol_info **back);
t_symbol_info   *ft_merge(t_symbol_info *a, t_symbol_info *b);
bool            compare_symbols(t_symbol_info *a, t_symbol_info *b);
void            ft_output(t_stack_file **file, int argc);

#endif