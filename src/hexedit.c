#include "hexedit.h"

void hexed_init(HexEditor *editor)
{
    static const char banner[] = "EX-WORD HEX EDITOR";
    unsigned i;
    editor->cursor = 0;

    /* Repeatable demonstration content; no OS memory is read. */
    for (i = 0; i < HEXED_SIZE; ++i)
        editor->bytes[i] = (uint8_t)i;
    for (i = 0; banner[i] != '\0' && (32u + i) < HEXED_SIZE; ++i)
        editor->bytes[32u + i] = (uint8_t)banner[i];
}

int hexed_move(HexEditor *editor, int delta)
{
    int next = (int)editor->cursor + delta;
    if (next < 0) next = 0;
    if (next >= (int)HEXED_SIZE) next = (int)HEXED_SIZE - 1;
    if (next == (int)editor->cursor) return 0;
    editor->cursor = (uint16_t)next;
    return 1;
}

void hexed_change(HexEditor *editor, int amount)
{
    unsigned value = (unsigned)editor->bytes[editor->cursor];
    /* Explicit modulo-256 arithmetic for +1, -1, +16 and -16. */
    editor->bytes[editor->cursor] = (uint8_t)((int)value + amount);
}

uint16_t hexed_page_start(const HexEditor *editor)
{
    return (uint16_t)((editor->cursor / HEXED_PAGE_BYTES) * HEXED_PAGE_BYTES);
}
