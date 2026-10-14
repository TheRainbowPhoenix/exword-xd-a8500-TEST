/*
 * MEMBR: unrestricted read-only EX-word memory browser.
 *
 * Legacy devkitSH4 GCC + ORIGINAL libdataplus, because DVLLO and DVGRD
 * launched on real hardware, whereas Hollyhock GCC Hello rebooted.
 * No user-space exception handling exists here: dereferencing unmapped or
 * privileged addresses can HARD-FAULT, hang, or reboot the EX-word.
 * Never write through the memory pointer. No storage/network I/O.
 */
#include <stdint.h>
#include <graphics/color.h>
#include <graphics/drawing.h>
#include <graphics/lcdc.h>
#include <graphics/text.h>
#include <sh4a/input/keypad.h>
#include "memory_browser.h"

#define SCREEN_W 528u
#define SCREEN_H 320u
#define GRID_TOP 72u
#define GRID_LINE 13u
#define HEX_LEFT 120u
#define ASCII_LEFT 354u
#define HEX_STRIDE 24u
#define ASCII_STRIDE 8u

static const char hexdigits[] = "0123456789ABCDEF";
static MemoryBrowser browser;
/* First displayed page is a known, ordinary app-allocated RAM object.
 * 128-byte alignment prevents the first page from preceding the allocation. */
static uint8_t demonstration[512] __attribute__((aligned(128)));

/* Input uses only libdataplus's *known working* keypad_read/get_key_state.
 * The physical EX-word keyboard has no general KEY_0 ... KEY_9 constants.
 * Q W E R T Y U I O P map to hex digits 0 1 2 3 4 5 6 7 8 9 when jumping.
 * Letters A B C D E F enter those six hex digits.
 */
enum {
    B_LEFT, B_RIGHT, B_UP, B_DOWN, B_PGUP, B_PGDN, B_JUMP,
    B_ENTER, B_BACK, B_POWER, B_BKSP,
    B_HEX0, B_HEX1, B_HEX2, B_HEX3, B_HEX4,
    B_HEX5, B_HEX6, B_HEX7, B_HEX8, B_HEX9,
    B_HEXA, B_HEXB, B_HEXC, B_HEXD, B_HEXE, B_HEXF,
    B_COUNT
};

static const uint8_t key_codes[B_COUNT] = {
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_PAGE_UP, KEY_PAGE_DOWN,
    KEY_JUMP, KEY_ENTER, KEY_BACK, KEY_POWER, KEY_BACKSPACE,
    KEY_CHAR_Q, KEY_CHAR_W, KEY_CHAR_E, KEY_CHAR_R, KEY_CHAR_T,
    KEY_CHAR_Y, KEY_CHAR_U, KEY_CHAR_I, KEY_CHAR_O, KEY_CHAR_P,
    KEY_CHAR_A, KEY_CHAR_B, KEY_CHAR_C, KEY_CHAR_D, KEY_CHAR_E, KEY_CHAR_F
};

static uint32_t key_snapshot(void)
{
    uint32_t bits = 0;
    unsigned i;
    for (i = 0u; i < B_COUNT; ++i)
        if (get_key_state(key_codes[i]))
            bits |= (uint32_t)1u << i;
    return bits;
}

static void text_at(unsigned x, unsigned y, const char *text)
{
    render_text((uint16_t)x, (uint16_t)y, (char *)text);
}

static void format_hex(char *dst, uint32_t num, unsigned count)
{
    unsigned i;
    for (i = 0; i < count; ++i)
        dst[i] = hexdigits[(num >> ((count - i - 1u) * 4u)) & 15u];
    dst[count] = 0;
}

static void fill_demo(void)
{
    static const char label[] = "EXWORD MEMORY BROWSER (OWN RAM)";
    unsigned i;
    for (i = 0u; i < sizeof(demonstration); ++i)
        demonstration[i] = (uint8_t)i;
    for (i = 0u; label[i] && i < 120u; ++i)
        demonstration[16u + i] = (uint8_t)label[i];
}

/* Every grid access is a byte read at the exact requested 32-bit address.
 * There is NO whitelist, caching, range check, probing, or fault recovery.
 * This is intentional per the experimental use case. */
static uint8_t read_volatile_byte(uint32_t address)
{
    const volatile uint8_t *p = (const volatile uint8_t *)(uintptr_t)address;
    return *p;
}

static void draw_page(void)
{
    uint16_t background = create_rgb16(255, 255, 255);
    uint16_t ink = create_rgb16(18, 36, 55);
    uint16_t soft = create_rgb16(80, 99, 116);
    uint16_t rule = create_rgb16(215, 224, 232);
    uint16_t selection = create_rgb16(182, 220, 253);
    unsigned row, col;
    char haddr[9], hvalue[3];

    set_pen(background);
    draw_rect(0, 0, SCREEN_W, SCREEN_H);

    set_pen(ink);
    text_at(12, 7, "MEMBR  /  RAW MEMORY VIEW  /  READ ONLY");
    set_pen(soft);
    text_at(12, 24, "ANY 32-BIT ADDRESS - INVALID READ MAY REBOOT");
    set_pen(rule);
    draw_rect(10, 44, SCREEN_W - 20u, 1u);

    set_pen(soft);
    text_at(12, 54, "ADDRESS");
    text_at(HEX_LEFT, 54, "00 01 02 03 04 05 06 07");
    text_at(ASCII_LEFT, 54, "ASCII");

    for (row = 0; row < MB_ROWS; ++row) {
        uint32_t row_addr = browser.page_base + (uint32_t)row * MB_COLUMNS;
        unsigned y = GRID_TOP + row * GRID_LINE;
        char hexrow[MB_COLUMNS * 3u + 1u];
        char asciirow[MB_COLUMNS + 1u];

        for (col = 0; col < MB_COLUMNS; ++col) {
            uint32_t addr = row_addr + col;
            uint8_t value = read_volatile_byte(addr);
            hexrow[col * 3u] = hexdigits[(value >> 4) & 15u];
            hexrow[col * 3u + 1u] = hexdigits[value & 15u];
            hexrow[col * 3u + 2u] = ' ';
            asciirow[col] = (value >= 32u && value <= 126u) ? (char)value : '.';
        }
        hexrow[MB_COLUMNS * 3u] = 0;
        asciirow[MB_COLUMNS] = 0;

        if (browser.address >= row_addr && browser.address < row_addr + MB_COLUMNS) {
            unsigned selected = browser.address - row_addr;
            set_pen(selection);
            draw_rect((uint16_t)(HEX_LEFT - 1u + selected * HEX_STRIDE), (uint16_t)(y - 1u), 19u, 13u);
            draw_rect((uint16_t)(ASCII_LEFT - 1u + selected * ASCII_STRIDE), (uint16_t)(y - 1u), 9u, 13u);
        }

        format_hex(haddr, row_addr, 8u);
        set_pen(soft);
        text_at(12, y, haddr);
        set_pen(ink);
        text_at(HEX_LEFT, y, hexrow);
        text_at(ASCII_LEFT, y, asciirow);
    }

    set_pen(rule);
    draw_rect(10, 282, SCREEN_W - 20u, 1u);

    if (browser.jumping) {
        char target[9];
        format_hex(target, browser.jump_target, 8u);
        set_pen(selection);
        draw_rect((uint16_t)(76u + (unsigned)browser.jump_digit * 8u), 288u, 9u, 13u);
        set_pen(ink);
        text_at(12, 288, "JUMP: 0x");
        text_at(76, 288, target);
        set_pen(soft);
        text_at(12, 304, "Q-P=0-9 A-F=HEX  LR DIGIT  ENTER GO  BACK NO");
    } else {
        char status[48] = "AT 0x00000000  BYTE=00   JUMP KEY TO GOTO";
        uint8_t current = read_volatile_byte(browser.address);
        format_hex(haddr, browser.address, 8u);
        format_hex(hvalue, current, 2u);
        {
            unsigned i;
            for (i = 0; i < 8u; ++i) status[5u + i] = haddr[i];
            for (i = 0; i < 2u; ++i) status[20u + i] = hvalue[i];
        }
        set_pen(ink);
        text_at(12, 288, status);
        set_pen(soft);
        text_at(12, 304, "ARROWS MOVE  PAGE UP/DOWN  JUMP  BACK EXIT");
    }

    lcdc_copy_vram();
}

static int first_typed_hex(uint32_t pressed)
{
    unsigned k;
    for (k = B_HEX0; k <= B_HEXF; ++k)
        if (pressed & ((uint32_t)1u << k))
            return (int)(k - B_HEX0);
    return -1;
}

static int handle_key_press(uint32_t pressed)
{
    if (pressed & ((uint32_t)1u << B_POWER))
        return -1;

    if (browser.jumping) {
        int digit;
        if (pressed & ((uint32_t)1u << B_BACK)) {
            mb_cancel_jump(&browser);
        } else if (pressed & ((uint32_t)1u << B_ENTER)) {
            mb_jump_confirm(&browser);
        } else if (pressed & ((uint32_t)1u << B_LEFT)) {
            mb_jump_digit_move(&browser, -1);
        } else if (pressed & ((uint32_t)1u << B_RIGHT)) {
            mb_jump_digit_move(&browser, 1);
        } else if (pressed & ((uint32_t)1u << B_UP)) {
            mb_jump_digit_step(&browser, 1);
        } else if (pressed & ((uint32_t)1u << B_DOWN)) {
            mb_jump_digit_step(&browser, -1);
        } else if (pressed & ((uint32_t)1u << B_BKSP)) {
            mb_jump_digit_set(&browser, 0);
        } else if ((digit = first_typed_hex(pressed)) >= 0) {
            mb_jump_digit_set(&browser, (uint8_t)digit);
            mb_jump_digit_move(&browser, 1);
        }
        return 1;
    }

    if (pressed & ((uint32_t)1u << B_BACK))
        return -1;
    if (pressed & ((uint32_t)1u << B_JUMP))
        mb_open_jump(&browser);
    else if (pressed & ((uint32_t)1u << B_LEFT))
        mb_move(&browser, -1);
    else if (pressed & ((uint32_t)1u << B_RIGHT))
        mb_move(&browser, 1);
    else if (pressed & ((uint32_t)1u << B_UP))
        mb_move(&browser, -(int32_t)MB_COLUMNS);
    else if (pressed & ((uint32_t)1u << B_DOWN))
        mb_move(&browser, (int32_t)MB_COLUMNS);
    else if (pressed & ((uint32_t)1u << B_PGUP))
        mb_move(&browser, -(int32_t)MB_PAGE_BYTES);
    else if (pressed & ((uint32_t)1u << B_PGDN))
        mb_move(&browser, (int32_t)MB_PAGE_BYTES);
    return 1;
}

int main(void)
{
    struct font *font = get_font();
    uint32_t last_keys = 0u;
    (void)font;

    fill_demo();
    mb_init(&browser, (uint32_t)(uintptr_t)&demonstration[0]);
    draw_page();

    for (;;) {
        uint32_t snapshot;
        uint32_t pressed;
        int action;
        volatile unsigned pause;

        keypad_read();
        snapshot = key_snapshot();
        pressed = snapshot & ~last_keys;
        last_keys = snapshot;
        if (pressed != 0u) {
            action = handle_key_press(pressed);
            if (action < 0)
                return -2;
            if (action > 0)
                draw_page();
        }

        for (pause = 0u; pause < 3500u; ++pause) { }
    }
}
