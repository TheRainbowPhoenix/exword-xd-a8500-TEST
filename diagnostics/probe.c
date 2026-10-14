/*
 * EX-word runtime diagnosis: no arbitrary memory reads or writes.
 * HXHEL = minimum EX-word display/keypad test, close to established examples.
 * HXGRD = 16-row text workload, without HEXED editing or get_key_pressed().
 * Do not confuse passing CI with proving hardware safety.
 */
#include <stdint.h>
#include <graphics/drawing.h>
#include <graphics/color.h>
#include <graphics/text.h>
#include <graphics/lcdc.h>
#include <sh4a/input/keypad.h>

#ifndef HEXED_PROBE_STAGE
#define HEXED_PROBE_STAGE 1
#endif

static void text_at(uint16_t x, uint16_t y, const char *text)
{
    render_text(x, y, (char *)text);
}

static void screen_base(void)
{
    set_pen(create_rgb16(0, 0, 0));
    draw_rect(0, 0, 528, 320);
    set_pen(create_rgb16(255, 255, 255));
}

#if HEXED_PROBE_STAGE == 2
static const char hex_digits[] = "0123456789ABCDEF";

static void render_grid(void)
{
    unsigned r, c;
    char row_text[4 + 2 + 8*3 + 1 + 8 + 1];
    for (r = 0; r < 16; ++r) {
        unsigned n = r * 8;
        row_text[0] = hex_digits[(n >> 12) & 15];
        row_text[1] = hex_digits[(n >> 8) & 15];
        row_text[2] = hex_digits[(n >> 4) & 15];
        row_text[3] = hex_digits[n & 15];
        row_text[4] = ':';
        row_text[5] = ' ';
        for (c = 0; c < 8; ++c) {
            unsigned b = n + c;
            row_text[6 + c*3] = hex_digits[(b >> 4) & 15];
            row_text[7 + c*3] = hex_digits[b & 15];
            row_text[8 + c*3] = ' ';
            row_text[31 + c] = b >= 32 && b <= 126 ? (char)b : '.';
        }
        row_text[30] = '|';
        row_text[39] = 0;
        text_at(12, (uint16_t)(54 + 14*r), row_text);
    }
}
#endif

int main(void)
{
    /* An established EX-word hello-world app also calls get_font() first. */
    struct font *font = get_font();
    (void)font;
    screen_base();
#if HEXED_PROBE_STAGE == 1
    text_at(16, 45, "HXHEL - MINIMAL DISPLAY PROBE");
    text_at(16, 75, "Same drawing APIs as known EX-word examples");
    text_at(16, 105, "No hex editor logic or RAM scanning");
#elif HEXED_PROBE_STAGE == 2
    text_at(12, 10, "HXGRD - TEXT GRID PROBE");
    text_at(12, 31, "NO MEMORY EDIT / NO FILE ACCESS");
    render_grid();
#else
#error HEXED_PROBE_STAGE must be 1 (minimal) or 2 (grid)
#endif
    text_at(12, 294, "BACK or POWER to exit");
    lcdc_copy_vram();
    for (;;) {
        keypad_read();
        if (get_key_state(KEY_BACK) || get_key_state(KEY_POWER))
            return -2;
    }
}
