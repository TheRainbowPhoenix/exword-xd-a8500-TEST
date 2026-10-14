#ifndef EXWORD_HEXED_H
#define EXWORD_HEXED_H

#include <stdint.h>

/* Deliberately limited to this application's own RAM. No device address
 * dereferences, hardware register writes, or filesystem operations. */
#define HEXED_SIZE 512u
#define HEXED_COLUMNS 8u
#define HEXED_ROWS 16u
#define HEXED_PAGE_BYTES (HEXED_COLUMNS * HEXED_ROWS)

typedef struct {
    uint8_t bytes[HEXED_SIZE];
    uint16_t cursor;
} HexEditor;

void hexed_init(HexEditor *editor);
int hexed_move(HexEditor *editor, int delta);
void hexed_change(HexEditor *editor, int amount);
uint16_t hexed_page_start(const HexEditor *editor);

#endif
