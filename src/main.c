/*
 * EX-word HEXED: tiny scratch-RAM hex editor inspired by
 * PyCSharp/CPHexEditor-For-HHK3's hex+ASCII display concept.
 *
 * Uses ONLY libdataplus-ex graphics and keypad calls.
 * No Hollyhock symbols, arbitrary memory reads/writes, or file I/O.
 */
#include <stdint.h>
#include <graphics/drawing.h>
#include <graphics/color.h>
#include <graphics/text.h>
#include <graphics/lcdc.h>
#include <sh4a/input/keypad.h>
#include "hexedit.h"

#define WIDTH 528u
#define HEIGHT 320u
#define GRID_Y 74u
#define GRID_STEP 13u
#define HEX_X 118u
#define ASCII_X 351u
#define HEX_STEP 24u
#define ASCII_STEP 8u

static const char digits[] = "0123456789ABCDEF";
static HexEditor editor;

static void draw_text(unsigned x, unsigned y, const char *str)
{
    render_text((uint16_t)x, (uint16_t)y, (char *)str);
}

static void hex_text(char *out, unsigned value, unsigned count)
{
    unsigned i;
    for (i = 0; i < count; ++i) {
        unsigned shift = (count - i - 1u) * 4u;
        out[i] = digits[(value >> shift) & 0x0fu];
    }
    out[count] = '\0';
}

static void draw_screen(void)
{
    uint16_t page = hexed_page_start(&editor);
    uint16_t background = create_rgb16(8, 14, 25);
    uint16_t foreground = create_rgb16(230, 239, 255);
    uint16_t accent = create_rgb16(70, 195, 232);
    uint16_t muted = create_rgb16(146, 171, 193);
    uint16_t selection = create_rgb16(33, 75, 120);
    unsigned row, col;

    set_pen(background);
    draw_rect(0, 0, WIDTH, HEIGHT);

    set_pen(accent);
    draw_text(14, 7, "EX-WORD  HEX EDITOR");
    set_pen(muted);
    draw_text(14, 28, "512-BYTE SCRATCH RAM  /  NO FILE ACCESS");
    set_pen(accent);
    draw_rect(12, 47, WIDTH - 24u, 1);

    set_pen(muted);
    draw_text(14, 55, "OFFSET");
    draw_text(HEX_X, 55, "00 01 02 03 04 05 06 07");
    draw_text(ASCII_X, 55, "ASCII");

    for (row = 0; row < HEXED_ROWS; ++row) {
        uint16_t offset = (uint16_t)(page + row * HEXED_COLUMNS);
        unsigned y = GRID_Y + GRID_STEP * row;
        char address[5];
        char hexline[HEXED_COLUMNS * 3u + 1u];
        char asciiline[HEXED_COLUMNS + 1u];

        hex_text(address, offset, 4);
        for (col = 0; col < HEXED_COLUMNS; ++col) {
            unsigned b = editor.bytes[offset + col];
            hexline[col * 3u] = digits[(b >> 4) & 0x0fu];
            hexline[col * 3u + 1u] = digits[b & 0x0fu];
            hexline[col * 3u + 2u] = ' ';
            asciiline[col] = (b >= 32u && b <= 126u) ? (char)b : '.';
        }
        hexline[HEXED_COLUMNS * 3u] = '\0';
        asciiline[HEXED_COLUMNS] = '\0';

        if (editor.cursor >= offset &&
            editor.cursor < offset + HEXED_COLUMNS) {
            unsigned selected_col = editor.cursor - offset;
            set_pen(selection);
            draw_rect(HEX_X - 2u + HEX_STEP * selected_col, y - 1u, 20u, 13u);
            draw_rect(ASCII_X - 1u + ASCII_STEP * selected_col, y - 1u, 9u, 13u);
        }

        set_pen(muted);
        draw_text(14, y, address);
        set_pen(foreground);
        draw_text(HEX_X, y, hexline);
        draw_text(ASCII_X, y, asciiline);
    }

    {
        char info[] = "OFFSET: 0000    VALUE: 00    PAGE: 0000";
        hex_text(&info[8], editor.cursor, 4);
        hex_text(&info[23], editor.bytes[editor.cursor], 2);
        hex_text(&info[35], page, 4);
        set_pen(accent);
        draw_rect(12, 287, WIDTH - 24u, 1);
        draw_text(14, 291, info);
    }

    set_pen(muted);
    draw_text(14, 307, "ARROWS MOVE  PG PAGE  ENTER +/- BACKSP  SHIFT X16");
    lcdc_copy_vram();
}

int main(void)
{
    hexed_init(&editor);
    draw_screen();

    for (;;) {
        int dirty = 0;
        unsigned step;
        volatile unsigned spin;

        keypad_read();
        if (get_key_pressed(KEY_POWER) || get_key_pressed(KEY_BACK))
            return -2;

        if (get_key_pressed(KEY_LEFT))
            dirty |= hexed_move(&editor, -1);
        if (get_key_pressed(KEY_RIGHT))
            dirty |= hexed_move(&editor, 1);
        if (get_key_pressed(KEY_UP))
            dirty |= hexed_move(&editor, -(int)HEXED_COLUMNS);
        if (get_key_pressed(KEY_DOWN))
            dirty |= hexed_move(&editor, (int)HEXED_COLUMNS);
        if (get_key_pressed(KEY_PAGE_UP))
            dirty |= hexed_move(&editor, -(int)HEXED_PAGE_BYTES);
        if (get_key_pressed(KEY_PAGE_DOWN))
            dirty |= hexed_move(&editor, (int)HEXED_PAGE_BYTES);

        step = get_key_state(KEY_SHIFT) ? 16u : 1u;
        if (get_key_pressed(KEY_ENTER)) {
            hexed_change(&editor, (int)step);
            dirty = 1;
        }
        if (get_key_pressed(KEY_BACKSPACE)) {
            hexed_change(&editor, -(int)step);
            dirty = 1;
        }
        if (dirty)
            draw_screen();

        /* Avoid a fully busy input polling loop. */
        for (spin = 0; spin < 12000u; ++spin) { }
    }
}
