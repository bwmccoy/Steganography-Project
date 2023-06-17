#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structs.h"
#include "info.h"
#include "reveal.h"
#include "hide.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: need to provide --<type> and up to two <filename> arguments\n");
        exit(1);
    }

    // opening the <filename> argument to be read
    FILE* file = fopen(argv[2], "r+");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);

    }

    // reading the BMP header to read_bmp_header and reading the DIB header to read_dib_header 
    BMP_Header read_bmp_header;
    DIB_Header read_dib_header;
    fread(&read_bmp_header, sizeof(read_bmp_header), 1, file);
    fread(&read_dib_header, sizeof(read_dib_header), 1, file);


    // check to make sure format identifier is "BM":
    if (read_bmp_header.format_identifier1 != 'B' || read_bmp_header.format_identifier2 != 'M') {
        fprintf(stderr, "The format is not supported.\n");
        exit(1);
    }

    // checking that the size of the DIB header is 40
    if (read_dib_header.size != 40) {
        fprintf(stderr, "The format is not supported.\n");
        exit(1);
    }

    // checking that the pixel data is encoded in 24-bit RGB
    if (read_dib_header.bits_per_pixel != 24) {
        fprintf(stderr, "The format is not supported.\n");
        exit(1);
    }

    if (!strcmp(argv[1], "--info")) { // if the argument is --info
        info(read_bmp_header, read_dib_header);
    } else if (!strcmp(argv[1], "--reveal")) { // if the argument is --reveal
        reveal(file, read_bmp_header, read_dib_header);
    } else if (!strcmp(argv[1], "--hide")) { // if the argument is --hide
        hide(file, argv[3], read_bmp_header, read_dib_header);
    }
    fclose(file);

    return 0;
}