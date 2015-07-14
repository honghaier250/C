/*
 * linux/fs/nls/nls_cp869.c
 *
 * Charset cp869 translation tables.
 * Generated automatically from the Unicode and charset
 * tables from the Unicode Organization (www.unicode.org).
 * The Unicode to charset table has only exact mappings.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/nls.h>
#include <linux/errno.h>

static const wchar_t charset2uni[256] = {
    /* 0x00*/
    0x0000, 0x0001, 0x0002, 0x0003,
    0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000a, 0x000b,
    0x000c, 0x000d, 0x000e, 0x000f,
    /* 0x10*/
    0x0010, 0x0011, 0x0012, 0x0013,
    0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001a, 0x001b,
    0x001c, 0x001d, 0x001e, 0x001f,
    /* 0x20*/
    0x0020, 0x0021, 0x0022, 0x0023,
    0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002a, 0x002b,
    0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30*/
    0x0030, 0x0031, 0x0032, 0x0033,
    0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b,
    0x003c, 0x003d, 0x003e, 0x003f,
    /* 0x40*/
    0x0040, 0x0041, 0x0042, 0x0043,
    0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b,
    0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50*/
    0x0050, 0x0051, 0x0052, 0x0053,
    0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b,
    0x005c, 0x005d, 0x005e, 0x005f,
    /* 0x60*/
    0x0060, 0x0061, 0x0062, 0x0063,
    0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b,
    0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70*/
    0x0070, 0x0071, 0x0072, 0x0073,
    0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b,
    0x007c, 0x007d, 0x007e, 0x007f,
    /* 0x80*/
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0386, 0x0000,
    0x00b7, 0x00ac, 0x00a6, 0x2018,
    0x2019, 0x0388, 0x2015, 0x0389,
    /* 0x90*/
    0x038a, 0x03aa, 0x038c, 0x0000,
    0x0000, 0x038e, 0x03ab, 0x00a9,
    0x038f, 0x00b2, 0x00b3, 0x03ac,
    0x00a3, 0x03ad, 0x03ae, 0x03af,
    /* 0xa0*/
    0x03ca, 0x0390, 0x03cc, 0x03cd,
    0x0391, 0x0392, 0x0393, 0x0394,
    0x0395, 0x0396, 0x0397, 0x00bd,
    0x0398, 0x0399, 0x00ab, 0x00bb,
    /* 0xb0*/
    0x2591, 0x2592, 0x2593, 0x2502,
    0x2524, 0x039a, 0x039b, 0x039c,
    0x039d, 0x2563, 0x2551, 0x2557,
    0x255d, 0x039e, 0x039f, 0x2510,
    /* 0xc0*/
    0x2514, 0x2534, 0x252c, 0x251c,
    0x2500, 0x253c, 0x03a0, 0x03a1,
    0x255a, 0x2554, 0x2569, 0x2566,
    0x2560, 0x2550, 0x256c, 0x03a3,
    /* 0xd0*/
    0x03a4, 0x03a5, 0x03a6, 0x03a7,
    0x03a8, 0x03a9, 0x03b1, 0x03b2,
    0x03b3, 0x2518, 0x250c, 0x2588,
    0x2584, 0x03b4, 0x03b5, 0x2580,
    /* 0xe0*/
    0x03b6, 0x03b7, 0x03b8, 0x03b9,
    0x03ba, 0x03bb, 0x03bc, 0x03bd,
    0x03be, 0x03bf, 0x03c0, 0x03c1,
    0x03c3, 0x03c2, 0x03c4, 0x0384,
    /* 0xf0*/
    0x00ad, 0x00b1, 0x03c5, 0x03c6,
    0x03c7, 0x00a7, 0x03c8, 0x0385,
    0x00b0, 0x00a8, 0x03c9, 0x03cb,
    0x03b0, 0x03ce, 0x25a0, 0x00a0,
};

static const unsigned char page00[256] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* 0x00-0x07 */
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* 0x08-0x0f */
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* 0x10-0x17 */
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* 0x18-0x1f */
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /* 0x20-0x27 */
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* 0x28-0x2f */
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 0x30-0x37 */
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 0x38-0x3f */
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* 0x40-0x47 */
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* 0x48-0x4f */
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* 0x50-0x57 */
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* 0x58-0x5f */
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* 0x60-0x67 */
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* 0x68-0x6f */
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* 0x70-0x77 */
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* 0x78-0x7f */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x80-0x87 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x88-0x8f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x90-0x97 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x98-0x9f */
    0xff, 0x00, 0x00, 0x9c, 0x00, 0x00, 0x8a, 0xf5, /* 0xa0-0xa7 */
    0xf9, 0x97, 0x00, 0xae, 0x89, 0xf0, 0x00, 0x00, /* 0xa8-0xaf */
    0xf8, 0xf1, 0x99, 0x9a, 0x00, 0x00, 0x00, 0x88, /* 0xb0-0xb7 */
    0x00, 0x00, 0x00, 0xaf, 0x00, 0xab, 0x00, 0x00, /* 0xb8-0xbf */
};

static const unsigned char page03[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x00-0x07 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x08-0x0f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x10-0x17 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x18-0x1f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x20-0x27 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x28-0x2f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x30-0x37 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x38-0x3f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x40-0x47 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x48-0x4f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x50-0x57 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x58-0x5f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x60-0x67 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x68-0x6f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x70-0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x78-0x7f */

    0x00, 0x00, 0x00, 0x00, 0xef, 0xf7, 0x86, 0x00, /* 0x80-0x87 */
    0x8d, 0x8f, 0x90, 0x00, 0x92, 0x00, 0x95, 0x98, /* 0x88-0x8f */
    0xa1, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, /* 0x90-0x97 */
    0xac, 0xad, 0xb5, 0xb6, 0xb7, 0xb8, 0xbd, 0xbe, /* 0x98-0x9f */
    0xc6, 0xc7, 0x00, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, /* 0xa0-0xa7 */
    0xd4, 0xd5, 0x91, 0x96, 0x9b, 0x9d, 0x9e, 0x9f, /* 0xa8-0xaf */
    0xfc, 0xd6, 0xd7, 0xd8, 0xdd, 0xde, 0xe0, 0xe1, /* 0xb0-0xb7 */
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, /* 0xb8-0xbf */
    0xea, 0xeb, 0xed, 0xec, 0xee, 0xf2, 0xf3, 0xf4, /* 0xc0-0xc7 */
    0xf6, 0xfa, 0xa0, 0xfb, 0xa2, 0xa3, 0xfd, 0x00, /* 0xc8-0xcf */
};

static const unsigned char page20[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x00-0x07 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x08-0x0f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x00, 0x00, /* 0x10-0x17 */
    0x8b, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x18-0x1f */
};

static const unsigned char page25[256] = {
    0xc4, 0x00, 0xb3, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x00-0x07 */
    0x00, 0x00, 0x00, 0x00, 0xda, 0x00, 0x00, 0x00, /* 0x08-0x0f */
    0xbf, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, /* 0x10-0x17 */
    0xd9, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, /* 0x18-0x1f */
    0x00, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, /* 0x20-0x27 */
    0x00, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x00, /* 0x28-0x2f */
    0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, /* 0x30-0x37 */
    0x00, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, /* 0x38-0x3f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x40-0x47 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x48-0x4f */
    0xcd, 0xba, 0x00, 0x00, 0xc9, 0x00, 0x00, 0xbb, /* 0x50-0x57 */
    0x00, 0x00, 0xc8, 0x00, 0x00, 0xbc, 0x00, 0x00, /* 0x58-0x5f */
    0xcc, 0x00, 0x00, 0xb9, 0x00, 0x00, 0xcb, 0x00, /* 0x60-0x67 */
    0x00, 0xca, 0x00, 0x00, 0xce, 0x00, 0x00, 0x00, /* 0x68-0x6f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x70-0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x78-0x7f */

    0xdf, 0x00, 0x00, 0x00, 0xdc, 0x00, 0x00, 0x00, /* 0x80-0x87 */
    0xdb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x88-0x8f */
    0x00, 0xb0, 0xb1, 0xb2, 0x00, 0x00, 0x00, 0x00, /* 0x90-0x97 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x98-0x9f */
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xa0-0xa7 */
};

static const unsigned char *const page_uni2charset[256] = {
    page00, NULL,   NULL,   page03, NULL,   NULL,   NULL,   NULL,   
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   
    page20, NULL,   NULL,   NULL,   NULL,   page25, NULL,   NULL,   
};

static const unsigned char charset2lower[256] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* 0x00-0x07 */
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* 0x08-0x0f */
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* 0x10-0x17 */
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* 0x18-0x1f */
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /* 0x20-0x27 */
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* 0x28-0x2f */
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 0x30-0x37 */
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 0x38-0x3f */
    0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* 0x40-0x47 */
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* 0x48-0x4f */
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* 0x50-0x57 */
    0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* 0x58-0x5f */
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* 0x60-0x67 */
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* 0x68-0x6f */
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* 0x70-0x77 */
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* 0x78-0x7f */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9b, 0x00, /* 0x80-0x87 */
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x9d, 0x8e, 0x9e, /* 0x88-0x8f */
    0x9f, 0xa0, 0xa2, 0x00, 0x00, 0xa3, 0xfb, 0x97, /* 0x90-0x97 */
    0xfd, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, /* 0x98-0x9f */
    0xa0, 0xa1, 0xa2, 0xa3, 0xd6, 0xd7, 0xd8, 0xdd, /* 0xa0-0xa7 */
    0xde, 0xe0, 0xe1, 0xab, 0xe2, 0xe3, 0xae, 0xaf, /* 0xa8-0xaf */
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xe4, 0xe5, 0xe6, /* 0xb0-0xb7 */
    0xe7, 0xb9, 0xba, 0xbb, 0xbc, 0xe8, 0xe9, 0xbf, /* 0xb8-0xbf */
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xea, 0xeb, /* 0xc0-0xc7 */
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xec, /* 0xc8-0xcf */
    0xee, 0xf2, 0xf3, 0xf4, 0xf6, 0xfa, 0xd6, 0xd7, /* 0xd0-0xd7 */
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* 0xd8-0xdf */
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* 0xe0-0xe7 */
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* 0xe8-0xef */
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* 0xf0-0xf7 */
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, /* 0xf8-0xff */
};

static const unsigned char charset2upper[256] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* 0x00-0x07 */
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* 0x08-0x0f */
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* 0x10-0x17 */
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* 0x18-0x1f */
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /* 0x20-0x27 */
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* 0x28-0x2f */
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 0x30-0x37 */
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 0x38-0x3f */
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* 0x40-0x47 */
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* 0x48-0x4f */
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* 0x50-0x57 */
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* 0x58-0x5f */
    0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* 0x60-0x67 */
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* 0x68-0x6f */
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* 0x70-0x77 */
    0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* 0x78-0x7f */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x00, /* 0x80-0x87 */
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* 0x88-0x8f */
    0x90, 0x91, 0x92, 0x00, 0x00, 0x95, 0x96, 0x97, /* 0x90-0x97 */
    0x98, 0x99, 0x9a, 0x86, 0x9c, 0x8d, 0x8f, 0x90, /* 0x98-0x9f */
    0x91, 0xa1, 0x92, 0x95, 0xa4, 0xa5, 0xa6, 0xa7, /* 0xa0-0xa7 */
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* 0xa8-0xaf */
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* 0xb0-0xb7 */
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* 0xb8-0xbf */
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* 0xc0-0xc7 */
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* 0xc8-0xcf */
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xa4, 0xa5, /* 0xd0-0xd7 */
    0xa6, 0xd9, 0xda, 0xdb, 0xdc, 0xa7, 0xa8, 0xdf, /* 0xd8-0xdf */
    0xa9, 0xaa, 0xac, 0xad, 0xb5, 0xb6, 0xb7, 0xb8, /* 0xe0-0xe7 */
    0xbd, 0xbe, 0xc6, 0xc7, 0xcf, 0xcf, 0xd0, 0xef, /* 0xe8-0xef */
    0xf0, 0xf1, 0xd1, 0xd2, 0xd3, 0xf5, 0xd4, 0xf7, /* 0xf0-0xf7 */
    0xf8, 0xf9, 0xd5, 0x96, 0xfc, 0x98, 0xfe, 0xff, /* 0xf8-0xff */
};

static int uni2char(wchar_t uni, unsigned char *out, int boundlen)
{
    const unsigned char *uni2charset;
    unsigned char cl = uni & 0x00ff;
    unsigned char ch = (uni & 0xff00) >> 8;

    if (boundlen <= 0)
        return -ENAMETOOLONG;

    uni2charset = page_uni2charset[ch];
    if (uni2charset && uni2charset[cl])
        out[0] = uni2charset[cl];
    else
        return -EINVAL;
    return 1;
}

static int char2uni(const unsigned char *rawstring, int boundlen, wchar_t *uni)
{
    *uni = charset2uni[*rawstring];
    if (*uni == 0x0000)
        return -EINVAL;
    return 1;
}

static struct nls_table table = {
    .charset    = "cp869",
    .uni2char    = uni2char,
    .char2uni    = char2uni,
    .charset2lower    = charset2lower,
    .charset2upper    = charset2upper,
    .owner        = THIS_MODULE,
};

static int __init init_nls_cp869(void)
{
    return register_nls(&table);
}

static void __exit exit_nls_cp869(void)
{
    unregister_nls(&table);
}

module_init(init_nls_cp869)
module_exit(exit_nls_cp869)

MODULE_LICENSE("Dual BSD/GPL");
