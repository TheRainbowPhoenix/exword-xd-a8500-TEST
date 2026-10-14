#include <assert.h>
#include <stdint.h>
#include "../src/hexedit.h"

int main(void)
{
    HexEditor e;
    uint8_t untouched;
    hexed_init(&e);
    assert(e.cursor == 0);
    assert(e.bytes[0] == 0);
    assert(e.bytes[1] == 1);
    assert(e.bytes[32] == 'E');
    assert(hexed_page_start(&e) == 0);

    assert(hexed_move(&e, -1) == 0);
    assert(e.cursor == 0);
    assert(hexed_move(&e, 511) == 1);
    assert(e.cursor == 511);
    assert(hexed_page_start(&e) == 384);
    assert(hexed_move(&e, 200) == 0);
    assert(e.cursor == 511);

    untouched = e.bytes[510];
    e.bytes[e.cursor] = 255;
    hexed_change(&e, 1);
    assert(e.bytes[511] == 0);
    hexed_change(&e, -1);
    assert(e.bytes[511] == 255);
    hexed_change(&e, 16);
    assert(e.bytes[511] == 15);
    hexed_change(&e, -16);
    assert(e.bytes[511] == 255);
    assert(e.bytes[510] == untouched);

    assert(hexed_move(&e, -383) == 1);
    assert(e.cursor == 128);
    assert(hexed_page_start(&e) == 128);
    assert(hexed_move(&e, -128) == 1);
    assert(hexed_page_start(&e) == 0);
    return 0;
}
