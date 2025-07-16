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
    Elf32_Ehdr          *elf32_header;
    Elf64_Ehdr          *elf64_header;
    unsigned char       *file_content_ptr;      // ¡Campo para el puntero del ELF en la RAM!
    size_t              file_size;
    t_nmflags           flag;
    t_argtype           type;
    t_bits              bits;
    t_endianness        endianness;
    struct s_stack_file *next;
}   t_stack_file;

//*** auxiliary functions ***

void            ft_putstr_stderr(char *str);
void            ft_handle_file_error(char *program_name, char *file_name, int errnum);
void            ft_handle_file_error_two(char *program_name, char *file_name, char *str);
char            *ft_split(char **str, char c);
int             ft_findflags(char *str);

//*** strcut functions ***

void            ft_stack_node(t_stack_file **sfile, t_stack_file *new);
t_stack_file    *ft_create_node(char *str, int pos, int status);
void            ft_print_stack_files(t_stack_file *sfile);
void            ft_clear_closing(t_stack_file **files);


//*** explicit functions ***

void            ft_fileFormat_id(t_stack_file **sfile, int flag);
void            ft_parsing_header(t_stack_file **files);
void            ft_location_headings(t_stack_file **files);

#endif



/*typedef struct {
    Elf64_Addr    st_value;  // Dirección del símbolo
    Elf64_Xword   st_info;   // Tipo y binding
    char          st_name[]; // Nombre (en .strtab)
} Elf64_Sym;*/
