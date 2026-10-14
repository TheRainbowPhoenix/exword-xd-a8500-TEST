#include "memory_regions.h"

void mb_regions_init(MBRegions *regions, uint32_t start, uint32_t size)
{
    regions->scratch_start = start;
    regions->scratch_size = size;
    regions->nor_size = 0u;
}

int mb_nor_header_parse(const uint8_t hdr[24], uint32_t *size)
{
    static const char sig[9] = { 'C','A','S','I','O','D','I','C','S' };
    uint32_t sz;
    unsigned i;
    for (i = 0; i < 9u; ++i)
        if (hdr[i] != (uint8_t)sig[i])
            return 0;

    /* The machine is big-endian, but parse explicitly to make unit tests
     * portable. Dictscript's r_hdr has nor_size at offset 20. */
    sz = ((uint32_t)hdr[20] << 24) |
         ((uint32_t)hdr[21] << 16) |
         ((uint32_t)hdr[22] << 8)  |
          (uint32_t)hdr[23];
    /* Reject obviously damaged values. Validity of a header still doesn't
     * prove any unrelated memory mapping is readable on a given device. */
    if (sz < UINT32_C(0x00020000) ||
        sz > UINT32_C(0x04000000) ||
        (sz & UINT32_C(0xFFFF)) != 0u)
        return 0;
    *size = sz;
    return 1;
}

static int inside(uint32_t addr, uint32_t start, uint32_t length)
{
    /* Subtraction avoids a 32-bit overflow in start + length. */
    return length != 0u && addr >= start && (addr - start) < length;
}

int mb_regions_can_read(const MBRegions *regions, MBView view, uint32_t addr)
{
    if (view == MB_VIEW_RAW)
        return 1;
    if (view == MB_VIEW_NOR)
        return inside(addr, MB_NOR_BASE, regions->nor_size);
    return inside(addr, regions->scratch_start, regions->scratch_size);
}
