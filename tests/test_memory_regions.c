#include <assert.h>
#include <stdint.h>
#include "../memory_browser/memory_regions.h"
static void set_nor(uint8_t h[24], uint32_t len)
{
    static const char tag[] = "CASIODICS";
    unsigned i;
    for (i = 0; i < 24u; ++i) h[i] = 0u;
    for (i = 0; i < 9u; ++i) h[i] = (uint8_t)tag[i];
    h[20] = (uint8_t)(len >> 24);
    h[21] = (uint8_t)(len >> 16);
    h[22] = (uint8_t)(len >> 8);
    h[23] = (uint8_t)len;
}
int main(void)
{
    MBRegions r;
    uint8_t h[24];
    uint32_t size=0;
    mb_regions_init(&r, UINT32_C(0x8C800200), 512u);
    assert(mb_regions_can_read(&r, MB_VIEW_SCRATCH, UINT32_C(0x8C800200)));
    assert(mb_regions_can_read(&r, MB_VIEW_SCRATCH, UINT32_C(0x8C8003FF)));
    assert(!mb_regions_can_read(&r, MB_VIEW_SCRATCH, UINT32_C(0x8C800400)));
    assert(!mb_regions_can_read(&r, MB_VIEW_NOR, MB_NOR_BASE));
    assert(mb_regions_can_read(&r, MB_VIEW_RAW, UINT32_C(0xFFFFFFFF)));
    set_nor(h, UINT32_C(0x02000000));
    assert(mb_nor_header_parse(h, &size));
    assert(size == UINT32_C(0x02000000));
    r.nor_size=size;
    assert(mb_regions_can_read(&r, MB_VIEW_NOR, MB_NOR_BASE));
    assert(mb_regions_can_read(&r, MB_VIEW_NOR, MB_NOR_HEADER_1));
    assert(mb_regions_can_read(&r, MB_VIEW_NOR, UINT32_C(0x81FFFFFF)));
    assert(!mb_regions_can_read(&r, MB_VIEW_NOR, UINT32_C(0x82000000)));
    assert(!mb_regions_can_read(&r, MB_VIEW_NOR, UINT32_C(0x8C800200)));
    assert(!mb_regions_can_read(&r, MB_VIEW_NOR, 0));
    h[0] = 0;
    assert(!mb_nor_header_parse(h, &size));
    set_nor(h, 0xffffffffu);
    assert(!mb_nor_header_parse(h, &size));
    set_nor(h, 0x20080u);
    assert(!mb_nor_header_parse(h, &size));
    mb_regions_init(&r, UINT32_C(0xFFFFFF00), 256u);
    assert(mb_regions_can_read(&r, MB_VIEW_SCRATCH, UINT32_MAX));
    assert(!mb_regions_can_read(&r, MB_VIEW_SCRATCH, 0));
    return 0;
}
