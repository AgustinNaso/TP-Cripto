#include <stdint.h>
#define HEADER_SIZE 54

typedef struct bitmap_file_header {
    uint16_t    bf_type;
    uint32_t    bf_size;
    uint16_t    bf_reserved_1;
    uint16_t    bf_reserved_2;
    uint32_t    bf_offset_bits;
} bmp_file_header;

typedef struct bitmap_image_header {
    uint32_t    header_size;        // 4 bytes
    uint32_t    width;              // 4 bytes
    uint32_t    height;             // 4 bytes
    uint16_t    planes;             // 2 bytes
    uint16_t    bit_count;          // 2 bytes
    uint32_t    compression;        // 4 bytes
    uint32_t    image_size;         // 4 bytes
    uint32_t    ppm_x;              // 4 bytes
    uint32_t    ppm_y;              // 4 bytes
    uint32_t    clr_used;           // 4 bytes
    uint32_t    clr_important;      // 4 bytes
} bmp_image_header;

typedef struct rgb_data {
    unsigned char r, g, b;
} rgb_data;