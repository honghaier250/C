/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 *   This file is part of the Linux kernel, and is made available under
 *   the terms of the GNU General Public License version 2.
 *
 * ----------------------------------------------------------------------- */

/*
 * Select video mode
 */

#include "boot.h"
#include "video.h"
#include "vesa.h"

static void store_cursor_position(void)
{
    u16 curpos;
    u16 ax, bx;

    ax = 0x0300;
    bx = 0;
    asm(INT10
        : "=d" (curpos), "+a" (ax), "+b" (bx)
        : : "ecx", "esi", "edi");

    boot_params.screen_info.orig_x = curpos;
    boot_params.screen_info.orig_y = curpos >> 8;
}

static void store_video_mode(void)
{
    u16 ax, page;

    /* N.B.: the saving of the video page here is a bit silly,
       since we pretty much assume page 0 everywhere. */
    ax = 0x0f00;
    asm(INT10
        : "+a" (ax), "=b" (page)
        : : "ecx", "edx", "esi", "edi");

    /* Not all BIOSes are clean with respect to the top bit */
    boot_params.screen_info.orig_video_mode = ax & 0x7f;
    boot_params.screen_info.orig_video_page = page >> 8;
}

/*
 * Store the video mode parameters for later usage by the kernel.
 * This is done by asking the BIOS except for the rows/columns
 * parameters in the default 80x25 mode -- these are set directly,
 * because some very obscure BIOSes supply insane values.
 */
static void store_mode_params(void)
{
    u16 font_size;
    int x, y;

    /* For graphics mode, it is up to the mode-setting driver
       (currently only video-vesa.c) to store the parameters */
    if (graphic_mode)
        return;

    store_cursor_position();
    store_video_mode();

    if (boot_params.screen_info.orig_video_mode == 0x07) {
        /* MDA, HGC, or VGA in monochrome mode */
        video_segment = 0xb000;
    } else {
        /* CGA, EGA, VGA and so forth */
        video_segment = 0xb800;
    }

    set_fs(0);
    font_size = rdfs16(0x485); /* Font size, BIOS area */
    boot_params.screen_info.orig_video_points = font_size;

    x = rdfs16(0x44a);
    y = (adapter == ADAPTER_CGA) ? 25 : rdfs8(0x484)+1;

    if (force_x)
        x = force_x;
    if (force_y)
        y = force_y;

    boot_params.screen_info.orig_video_cols  = x;
    boot_params.screen_info.orig_video_lines = y;
}

static unsigned int get_entry(void)
{
    char entry_buf[4];
    int i, len = 0;
    int key;
    unsigned int v;

    do {
        key = getchar();

        if (key == '\b') {
            if (len > 0) {
                puts("\b \b");
                len--;
            }
        } else if ((key >= '0' && key <= '9') ||
               (key >= 'A' && key <= 'Z') ||
               (key >= 'a' && key <= 'z')) {
            if (len < sizeof entry_buf) {
                entry_buf[len++] = key;
                putchar(key);
            }
        }
    } while (key != '\r');
    putchar('\n');

    if (len == 0)
        return VIDEO_CURRENT_MODE; /* Default */

    v = 0;
    for (i = 0; i < len; i++) {
        v <<= 4;
        key = entry_buf[i] | 0x20;
        v += (key > '9') ? key-'a'+10 : key-'0';
    }

    return v;
}

static void display_menu(void)
{
    struct card_info *card;
    struct mode_info *mi;
    char ch;
    int i;
    int nmodes;
    int modes_per_line;
    int col;

    nmodes = 0;
    for (card = video_cards; card < video_cards_end; card++)
        nmodes += card->nmodes;

    modes_per_line = 1;
    if (nmodes >= 20)
        modes_per_line = 3;

    for (col = 0; col < modes_per_line; col++)
        puts("Mode: Resolution:  Type: ");
    putchar('\n');

    col = 0;
    ch = '0';
    for (card = video_cards; card < video_cards_end; card++) {
        mi = card->modes;
        for (i = 0; i < card->nmodes; i++, mi++) {
            char resbuf[32];
            int visible = mi->x && mi->y;
            u16 mode_id = mi->mode ? mi->mode :
                (mi->y << 8)+mi->x;

            if (!visible)
                continue; /* Hidden mode */

            if (mi->depth)
                sprintf(resbuf, "%dx%d", mi->y, mi->depth);
            else
                sprintf(resbuf, "%d", mi->y);

            printf("%c %03X %4dx%-7s %-6s",
                   ch, mode_id, mi->x, resbuf, card->card_name);
            col++;
            if (col >= modes_per_line) {
                putchar('\n');
                col = 0;
            }

            if (ch == '9')
                ch = 'a';
            else if (ch == 'z' || ch == ' ')
                ch = ' '; /* Out of keys... */
            else
                ch++;
        }
    }
    if (col)
        putchar('\n');
}

#define H(x)    ((x)-'a'+10)
#define SCAN    ((H('s')<<12)+(H('c')<<8)+(H('a')<<4)+H('n'))

static unsigned int mode_menu(void)
{
    int key;
    unsigned int sel;

    puts("Press <ENTER> to see video modes available, "
         "<SPACE> to continue, or wait 30 sec\n");

    kbd_flush();
    while (1) {
        key = getchar_timeout();
        if (key == ' ' || key == 0)
            return VIDEO_CURRENT_MODE; /* Default */
        if (key == '\r')
            break;
        putchar('\a');    /* Beep! */
    }


    for (;;) {
        display_menu();

        puts("Enter a video mode or \"scan\" to scan for "
             "additional modes: ");
        sel = get_entry();
        if (sel != SCAN)
            return sel;

        probe_cards(1);
    }
}

#ifdef CONFIG_VIDEO_RETAIN
/* Save screen content to the heap */
struct saved_screen {
    int x, y;
    int curx, cury;
    u16 *data;
} saved;

static void save_screen(void)
{
    /* Should be called after store_mode_params() */
    saved.x = boot_params.screen_info.orig_video_cols;
    saved.y = boot_params.screen_info.orig_video_lines;
    saved.curx = boot_params.screen_info.orig_x;
    saved.cury = boot_params.screen_info.orig_y;

    if (!heap_free(saved.x*saved.y*sizeof(u16)+512))
        return;        /* Not enough heap to save the screen */

    saved.data = GET_HEAP(u16, saved.x*saved.y);

    set_fs(video_segment);
    copy_from_fs(saved.data, 0, saved.x*saved.y*sizeof(u16));
}

static void restore_screen(void)
{
    /* Should be called after store_mode_params() */
    int xs = boot_params.screen_info.orig_video_cols;
    int ys = boot_params.screen_info.orig_video_lines;
    int y;
    addr_t dst = 0;
    u16 *src = saved.data;
    u16 ax, bx, dx;

    if (graphic_mode)
        return;        /* Can't restore onto a graphic mode */

    if (!src)
        return;        /* No saved screen contents */

    /* Restore screen contents */

    set_fs(video_segment);
    for (y = 0; y < ys; y++) {
        int npad;

        if (y < saved.y) {
            int copy = (xs < saved.x) ? xs : saved.x;
            copy_to_fs(dst, src, copy*sizeof(u16));
            dst += copy*sizeof(u16);
            src += saved.x;
            npad = (xs < saved.x) ? 0 : xs-saved.x;
        } else {
            npad = xs;
        }

        /* Writes "npad" blank characters to
           video_segment:dst and advances dst */
        asm volatile("pushw %%es ; "
                 "movw %2,%%es ; "
                 "shrw %%cx ; "
                 "jnc 1f ; "
                 "stosw \n\t"
                 "1: rep;stosl ; "
                 "popw %%es"
                 : "+D" (dst), "+c" (npad)
                 : "bdS" (video_segment),
                   "a" (0x07200720));
    }

    /* Restore cursor position */
    ax = 0x0200;        /* Set cursor position */
    bx = 0;            /* Page number (<< 8) */
    dx = (saved.cury << 8)+saved.curx;
    asm volatile(INT10
             : "+a" (ax), "+b" (bx), "+d" (dx)
             : : "ecx", "esi", "edi");
}
#else
#define save_screen()        ((void)0)
#define restore_screen()    ((void)0)
#endif

void set_video(void)
{
    u16 mode = boot_params.hdr.vid_mode;

    RESET_HEAP();

    store_mode_params();
    save_screen();
    probe_cards(0);

    for (;;) {
        if (mode == ASK_VGA)
            mode = mode_menu();

        if (!set_mode(mode))
            break;

        printf("Undefined video mode number: %x\n", mode);
        mode = ASK_VGA;
    }
    boot_params.hdr.vid_mode = mode;
    vesa_store_edid();
    store_mode_params();

    if (do_restore)
        restore_screen();
}
