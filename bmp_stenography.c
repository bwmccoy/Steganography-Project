#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct _BMP_Header {
    char format_identifier1; // should be a short since a string will give it a sentinel which doesn't exist in the BMP header (I had it as a char* before)
    char format_identifier2;
    int size_in_bits;
    short reserved1;
    short reserved2;
    int offset;

} BMP_Header;

#pragma pack(1)
typedef struct _DIB_Header {
    int size;
    int width;
    int height;
    short color_panes;
    short bits_per_pixel;
    int compression_scheme;
    int image_size;
    int horizontal_res;
    int vertical_res;
    int num_of_colors;
    int num_of_important_colors;
} DIB_Header;

#pragma pack(1)
typedef struct _Pixel_Array {
    char blue;
    char green;
    char red;
} Pixel_Array;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "ERROR: need to provide --<type> and <filename> arguments");
        exit(1);
    }

    char* type = argv[1];
    char* filename = argv[2];

    // opening the <filename> argument to be read
    FILE* file = fopen(filename, "r+");
    if (file == NULL) {
        fprintf(stderr, "Error opening file");
        exit(1);

    }

    // reading the BMP header to read_bmp_header and reading the DIB header to read_dib_header 
    BMP_Header read_bmp_header;
    DIB_Header read_dib_header;
    fread(&read_bmp_header, sizeof(read_bmp_header), 1, file);
    fread(&read_dib_header, sizeof(read_dib_header), 1, file);


    // check to make sure format identifier is "BM":
    if (read_bmp_header.format_identifier1 != 'B' || read_bmp_header.format_identifier2 != 'M') {
        fprintf(stderr, "ERROR: file format not supported");
        exit(1);
    }

    // checking that the size of the DIB header is 40
    if (read_dib_header.size != 40) {
        fprintf(stderr, "ERROR: file format not supported");
        exit(1);
    }

    // checking that the pixel data is encoded in 24-bit RGB
    if (read_dib_header.bits_per_pixel != 24) {
        fprintf(stderr, "ERROR: file format not supported");
        exit(1);
    }




    if (!strcmp(type, "--info")) { // if the argument is --info
        // printing the contents of the BMP header and DIB header from the created structs
        printf("=== BMP Header ===\nType: %c%c\nSize: %d\nReserved 1: %d\nReserved 2: %d\nImage Offset: %d\n\n=== DIB Header ===\nSize: %d\nWidth: %d\nHeight: %d\n# color panes: %d\n# bits per pixel: %d\nCompression scheme: %d\nImage size: %d\nHorizontal resolution: %d\nVertical resolution: %d\n# colors in palette: %d\n# important colors: %d\n", read_bmp_header.format_identifier1, read_bmp_header.format_identifier2, read_bmp_header.size_in_bits, read_bmp_header.reserved1, read_bmp_header.reserved2, read_bmp_header.offset, read_dib_header.size, read_dib_header.width, read_dib_header.height, read_dib_header.color_panes, read_dib_header.bits_per_pixel, read_dib_header.compression_scheme, read_dib_header.image_size, read_dib_header.horizontal_res, read_dib_header.vertical_res, read_dib_header.num_of_colors, read_dib_header.num_of_important_colors);
        
        fclose(file);

    } if (!strcmp(type, "--reveal")) {
        // moving file pointer to the start of the pixel array
        fseek(file, read_bmp_header.offset, SEEK_SET);

        // loop to read each row/col
        for (int i = 0; i < read_dib_header.height; i++) { // rows
            for (int j = 0; j < read_dib_header.width; j++) { // cols
                // reading a pixel
                Pixel_Array pixel_array;
                fread(&pixel_array, sizeof(pixel_array), 1, file);

                // transforming it (the 4 LSB of green and red are the hidden chars (may have to XOR them with the 4 LSB of blue to get the correct c1 and c2))
                char c1 = pixel_array.green & 0x0F;
                char c2 = pixel_array.red & 0x0F;
                c2 = c2 << 4;
                char letter = c1 | c2;

                //fseek(file, sizeof(pixel_array), SEEK_CUR);

                // writing the transformed pixel back to the file
                printf("%c", letter);

            }

            // checking for extra padding at end of row and skipping them if they exist
            if ((3 * read_dib_header.width) % 4 != 0) { // not a multiple of 4
                fseek(file, 4 - (3 * read_dib_header.width) % 4, SEEK_CUR); // advance by the remainder of the way to 4
            } 
        }

        // closing the file
        fclose(file);
    }


    return 0;
}