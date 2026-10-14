#include <assert.h>
#include <stdint.h>
#include "../memory_browser/memory_browser.h"

int main(void)
{
    MemoryBrowser b;
    mb_init(&b, UINT32_C(0x8C801234));
    assert(b.address == UINT32_C(0x8C801234));
    assert(b.page_base == UINT32_C(0x8C801200));
    mb_move(&b, 8);
    assert(b.address == UINT32_C(0x8C80123C));
    mb_move(&b, -128);
    assert(b.page_base == UINT32_C(0x8C801180));
    mb_open_jump(&b);
    assert(b.jumping);
    mb_jump_digit_set(&b, 15);
    assert((b.jump_target >> 28) == 15);
    mb_jump_digit_move(&b, 1);
    mb_jump_digit_set(&b, 0);
    assert(((b.jump_target >> 24) & 15) == 0);
    mb_jump_digit_step(&b, -1);
    assert(((b.jump_target >> 24) & 15) == 15);
    mb_cancel_jump(&b);
    assert(!b.jumping);
    assert(b.address == UINT32_C(0x8C8011BC));
    mb_open_jump(&b);
    mb_jump_digit_set(&b, 0);
    mb_jump_confirm(&b);
    assert(b.page_base == (b.address & UINT32_C(0xFFFFFF80)));
    assert(!b.jumping);
    mb_init(&b, UINT32_C(0xFFFFFFFF));
    mb_move(&b, 1);
    assert(b.address == 0u);
    assert(b.page_base == 0u);
    mb_move(&b, -1);
    assert(b.address == UINT32_MAX);
    assert(b.page_base == UINT32_C(0xFFFFFF80));
    return 0;
}
