#include <stdio.h>
#include "structs.h"

void info(BMP_Header read_bmp_header, DIB_Header read_dib_header) {
    
    // printing the contents of the BMP header and DIB header from the created structs
    printf("=== BMP Header ===\nType: %c%c\nSize: %d\nReserved 1: %d\nReserved 2: %d\nImage offset: %d\n\n=== DIB Header ===\nSize: %d\nWidth: %d\nHeight: %d\n# color planes: %d\n# bits per pixel: %d\nCompression scheme: %d\nImage size: %d\nHorizontal resolution: %d\nVertical resolution: %d\n# colors in palette: %d\n# important colors: %d\n\n", read_bmp_header.format_identifier1, read_bmp_header.format_identifier2, read_bmp_header.size_in_bits, read_bmp_header.reserved1, read_bmp_header.reserved2, read_bmp_header.offset, read_dib_header.size, read_dib_header.width, read_dib_header.height, read_dib_header.color_panes, read_dib_header.bits_per_pixel, read_dib_header.compression_scheme, read_dib_header.image_size, read_dib_header.horizontal_res, read_dib_header.vertical_res, read_dib_header.num_of_colors, read_dib_header.num_of_important_colors);

}