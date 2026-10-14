#include "memory_browser.h"

static void sync_page(MemoryBrowser *b)
{
    b->page_base = b->address & ~((uint32_t)MB_PAGE_BYTES - 1u);
}

void mb_init(MemoryBrowser *b, uint32_t initial_address)
{
    b->address = initial_address;
    sync_page(b);
    b->jump_target = initial_address;
    b->jump_digit = 0u;
    b->jumping = 0u;
}

void mb_move(MemoryBrowser *b, int32_t difference)
{
    /* Explicit unsigned modulo-2^32 navigation, without a memory whitelist. */
    b->address += (uint32_t)difference;
    sync_page(b);
}

void mb_open_jump(MemoryBrowser *b)
{
    b->jump_target = b->address;
    b->jump_digit = 0u;
    b->jumping = 1u;
}

void mb_cancel_jump(MemoryBrowser *b)
{
    b->jumping = 0u;
}

void mb_jump_digit_move(MemoryBrowser *b, int direction)
{
    if (direction > 0 && b->jump_digit < 7u)
        ++b->jump_digit;
    else if (direction < 0 && b->jump_digit > 0u)
        --b->jump_digit;
}

void mb_jump_digit_set(MemoryBrowser *b, uint8_t value)
{
    uint32_t shift = (uint32_t)(7u - b->jump_digit) * 4u;
    uint32_t mask = 15u << shift;
    b->jump_target = (b->jump_target & ~mask) | (((uint32_t)value & 15u) << shift);
}

void mb_jump_digit_step(MemoryBrowser *b, int step)
{
    unsigned shift = (7u - b->jump_digit) * 4u;
    uint8_t old_nibble = (uint8_t)((b->jump_target >> shift) & 15u);
    mb_jump_digit_set(b, (uint8_t)((old_nibble + step) & 15));
}

void mb_jump_confirm(MemoryBrowser *b)
{
    b->address = b->jump_target;
    sync_page(b);
    b->jumping = 0u;
}
