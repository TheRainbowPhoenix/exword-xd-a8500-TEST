#ifndef EXWORD_MEMORY_REGIONS_H
#define EXWORD_MEMORY_REGIONS_H
#include <stdint.h>

/* The browser cannot catch SH4 address/bus faults. The known *application*
 * range is its own allocated byte array. The NOR region becomes available
 * only after an opt-in signature probe on hardware known to support it. */
#define MB_NOR_BASE UINT32_C(0x80000000)
#define MB_NOR_HEADER_1 UINT32_C(0x8000FF80)
#define MB_NOR_HEADER_2 UINT32_C(0x8001FF80)

typedef enum {
    MB_VIEW_SCRATCH = 0,
    MB_VIEW_NOR = 1,
    MB_VIEW_RAW = 2
} MBView;

typedef struct {
    uint32_t scratch_start;
    uint32_t scratch_size;
    uint32_t nor_size;        /* zero until CASIODICS header validated */
} MBRegions;

void mb_regions_init(MBRegions *regions, uint32_t scratch_start, uint32_t scratch_size);
int mb_nor_header_parse(const uint8_t hdr[24], uint32_t *nor_size);
int mb_regions_can_read(const MBRegions *regions, MBView view, uint32_t addr);
#endif
