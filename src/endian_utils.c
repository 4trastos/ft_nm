#include "../incl/ft_nm.h"

t_host_endianness g_host_endianness = HOST_ENDIAN_UNKNOWN;

void    init_host_endianness(void)
{
    uint16_t        test_val = 0x0001;
    unsigned char   *byte_ptr = (unsigned  char *)&test_val;

    if (byte_ptr[0] == 0x01)
        g_host_endianness = HOST_ENDIAN_LSB;
    else
        g_host_endianness = HOST_ENDIAN_MSB;
}

uint16_t    swap16(uint16_t val)
{
    return ((val >> 8) | (val << 8));
}

uint32_t    swap32(uint32_t val)
{
    return ((val >> 24) & 0x000000FF) |
           ((val >> 8)  & 0x0000FF00) |
           ((val << 8)  & 0x00FF0000) |
           ((val << 24) & 0xFF000000);;
}

uint64_t    swap64(uint64_t val)
{
    return ((val >> 56) & 0x00000000000000FFULL) |
           ((val >> 40) & 0x000000000000FF00ULL) |
           ((val >> 24) & 0x0000000000FF0000ULL) |
           ((val >> 8)  & 0x00000000FF000000ULL) |
           ((val << 8)  & 0x000000FF00000000ULL) |
           ((val << 24) & 0x0000FF0000000000ULL) |
           ((val << 40) & 0x00FF000000000000ULL) |
           ((val << 56) & 0xFF00000000000000ULL);
}

static bool needs_swap(t_endianness file_endianness)
{
    if ((file_endianness == ENDIAN_LSB && g_host_endianness == HOST_ENDIAN_LSB) || 
        (file_endianness == ENDIAN_MSB && g_host_endianness == HOST_ENDIAN_MSB))
        return (false);
    return (true);
}

uint16_t get_elf_u16(uint16_t val_from_elf, t_endianness file_endianness)
{
    if (needs_swap(file_endianness))
        return swap16(val_from_elf);
    return val_from_elf;
}

uint32_t get_elf_u32(uint32_t val_from_elf, t_endianness file_endianness)
{
    if (needs_swap(file_endianness))
        return swap32(val_from_elf);
    return val_from_elf;
}

uint64_t get_elf_u64(uint64_t val_from_elf, t_endianness file_endianness)
{
    if (needs_swap(file_endianness))
        return swap64(val_from_elf);
    return val_from_elf;
}