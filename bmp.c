#include <stdio.h>
#include <string.h>
#include "include/bmp.h"


void init_file_header(bmp_file_header* bfh, int file_size)
{
    memcpy(&(bfh->bf_type), "BM", 2);
    bfh->bf_size = file_size;
    bfh->bf_reserved_1 = 0;
    bfh->bf_reserved_2 = 0;
    bfh->bf_offset_bits = 0;
}

void init_img_header(bmp_image_header* bih, int ppm, int file_size, int width, int height)
{
    bih->header_size = sizeof(*bih);
    bih->width = width;
    bih->height = height;
    bih->planes = 1;
    bih->bit_count = 24;
    bih->compression = 0;
    bih->image_size = file_size;
    bih->ppm_x = ppm;
    bih->ppm_y = ppm;
    bih->clr_used = 0;
    bih->clr_important = 0;
}


void create_bmp_file(char* file_name, int height, int width, rgb_data* pixels)
{
    bmp_image_header bih;
    bmp_file_header bfh;
    // dpi a ppm https://www.wikiwand.com/en/Dots_per_inch
    int ppm = 96 * 39.375;
    int img_size = height * width;
    int file_size = HEADER_SIZE + 8 * img_size;
    init_file_header(&bfh, file_size);
    init_img_header(&bih, ppm, file_size, width, height);
    printf("%d\n", bih.header_size);

    FILE *fp = fopen(file_name, "wb");
    fwrite(&bfh, 1, 14, fp);
    fwrite(&bih, 1, sizeof(bih), fp);

    for (int i = 0; i < img_size; i++)
    {
        rgb_data BGR = pixels[i];
        unsigned char color[3] = {BGR.b, BGR.g, BGR.r};
        fwrite(color, 1, sizeof(color), fp);
    }
    return;
}

int main(int argc, char *argv[])
{
    int height = 400;
    int width = 400;
    rgb_data pixels[height * width];

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int a = y * width + x;

            if ((x > 50 && x < 350) && (y > 50 && y < 350))
            {
                pixels[a].r = 255;
                pixels[a].g = 255;
                pixels[a].b = 5;
            }
            else
            {
                pixels[a].r = 0;
                pixels[a].g = 0;
                pixels[a].b = 155;
            }
        }
    }

    create_bmp_file("test_file.bmp", height, width, pixels);

    return 0;
}