#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "structs.h"

void reveal(FILE* file, BMP_Header read_bmp_header, DIB_Header read_dib_header) {
    // moving file pointer to the start of the pixel array
    fseek(file, read_bmp_header.offset, SEEK_SET);
    
    int padding = (4 - ((3 * read_dib_header.width) % 4)) % 4; // Calculate the padding size

    bool flag = 0;
    // loop to read each row/col
    for (int i = 0; i < read_dib_header.height; i++) { // rows
        if (flag) {
            break;
        }
        for (int j = 0; j < read_dib_header.width; j++) { // cols

            // reading a pixel
            Pixel_Array pixel_array;
            fread(&pixel_array, sizeof(pixel_array), 1, file);

            // transforming it
            char g_lsb = pixel_array.green & 0x0F; // 4 LSB of green preceded by 0000
            char r_lsb = pixel_array.red & 0x0F; // 4 LSB of red preceded by 0000
            char b_lsb = pixel_array.blue & 0x0F; // 4 LSB of blue preceded by 0000

            // revealing the secret 4 LSB of green
            g_lsb = g_lsb ^ b_lsb;
            // revealing the secret 4 LSB of red
            r_lsb = r_lsb ^ b_lsb;

            // green lsb is nybble1 and red lsb is nybble2 for the hidden char
            char c = (g_lsb << 4) | r_lsb;

            // if theres an encoded 0 for the char then its the end of the text so break
            if (c == 0x00) {
                flag = 1;
                break;
            }

            printf("%c", c);

        }

        // checking for extra padding at end of row and skipping them if they exist
        // Skipping the padding if it exists
        if (padding > 0) {
            fseek(file, padding, SEEK_CUR);
        }
    }
    printf("\n");
}
