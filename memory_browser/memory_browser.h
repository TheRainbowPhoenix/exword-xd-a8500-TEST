#ifndef EXWORD_MEMORY_BROWSER_H
#define EXWORD_MEMORY_BROWSER_H

#include <stdint.h>

/* Entire 32-bit address space is addressable by the UI; there is deliberately
 * no whitelist, privilege elevation or automatic scan. Invalid reads can fault.
 * This structure stores addresses and UI state only, not cached device memory.
 */
#define MB_COLUMNS 8u
#define MB_ROWS 16u
#define MB_PAGE_BYTES (MB_COLUMNS * MB_ROWS)

typedef struct {
    uint32_t address;
    uint32_t page_base;
    uint32_t jump_target;
    uint8_t jump_digit;
    uint8_t jumping;
} MemoryBrowser;

void mb_init(MemoryBrowser *b, uint32_t initial_address);
void mb_move(MemoryBrowser *b, int32_t difference);
void mb_open_jump(MemoryBrowser *b);
void mb_cancel_jump(MemoryBrowser *b);
void mb_jump_digit_move(MemoryBrowser *b, int direction);
void mb_jump_digit_step(MemoryBrowser *b, int step);
void mb_jump_digit_set(MemoryBrowser *b, uint8_t value);
void mb_jump_confirm(MemoryBrowser *b);

#endif
