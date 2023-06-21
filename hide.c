#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structs.h"

void hide(FILE* file, char* filename2, BMP_Header read_bmp_header, DIB_Header read_dib_header) {
    // reading in the text from filename2 to store in image
    FILE* textfile = fopen(filename2, "r+");
    if (textfile == NULL) {
        fprintf(stderr, "Error opening text file\n");
        exit(1);
    }

    // moving file pointer to the start of the pixel array
    fseek(file, read_bmp_header.offset, SEEK_SET);

    // variables to hold the char in hidden message
    char current_char;
    char c1;
    char c2;

    int padding = (4 - ((3 * read_dib_header.width) % 4)) % 4; // Calculate the padding size

    bool flag = 0; // flag to break out of the nested loop

    // loop to read each row/col
    for (int i = 0; i < read_dib_header.height; i++) { // rows
        if (flag) {
            break;
        }
        for (int j = 0; j < read_dib_header.width; j++) { // cols
            current_char = fgetc(textfile);
            if (current_char != EOF) {
                c1 = current_char & 0xF0; // 4 MSB of c followed by four 0s
                c2 = current_char & 0x0F; // 4 LSB of c preceded by four 0s

                // to get c1 in correct format
                c1 = c1 >> 4;
                c1 = c1 & 0x0F; 

                // reading a pixel
                Pixel_Array pixel_array;
                fread(&pixel_array, sizeof(pixel_array), 1, file);

                // transforming it according to the current char of hidden_text
                char g_lsb = pixel_array.green & 0x0F; // 4 LSB of green preceded by 0000
                char g_msb = pixel_array.green & 0xF0;
                char r_lsb = pixel_array.red & 0x0F; // 4 LSB of red preceded by 0000
                char r_msb = pixel_array.red & 0xF0;
                char b_lsb = pixel_array.blue & 0x0F; // 4 LSB of blue preceded by 0000
                

                // encoding the 4 lsb of green based on the first half of the hidden char
                g_lsb = c1 ^ b_lsb; 
                // encoding the 4 lsb of red based on the first half of the hidden char
                r_lsb = c2 ^ b_lsb;

                // updating the pixel_array struct with encoded green and red 
                pixel_array.green = g_msb | g_lsb; 
                pixel_array.red = r_msb | r_lsb;

                // to get back to the correct pixel for writing
                fseek(file, -sizeof(pixel_array), SEEK_CUR); 

                // writing the transformed pixel back to the file
                fwrite(&pixel_array, sizeof(pixel_array), 1, file);
            } else {
                // writing a 0 to the image to denote the text is over 
                Pixel_Array pixel_array;
                fread(&pixel_array, sizeof(pixel_array), 1, file);

                char g_lsb = pixel_array.green & 0x0F;
                char g_msb = pixel_array.green & 0xF0;
                char r_lsb = pixel_array.red & 0x0F;
                char r_msb = pixel_array.red & 0xF0;
                char b_lsb = pixel_array.blue & 0x0F;

                g_lsb = 0x00 ^ b_lsb;
                r_lsb = 0x00 ^ b_lsb;

                // updating the pixel_array struct with encoded green and red 
                pixel_array.green = g_msb | g_lsb; 
                pixel_array.red = r_msb | r_lsb;

                // to get back to the correct pixel for writing
                fseek(file, -sizeof(pixel_array), SEEK_CUR); 

                fwrite(&pixel_array, sizeof(pixel_array), 1, file); 

                flag = 1;
                break;
            }
            
        }

        // checking for extra padding at end of row and skipping them if they exist
        // Skipping the padding if it exists
        if (padding > 0) {
            fseek(file, padding, SEEK_CUR);
        }
    }

    // if the textfile is too large for image
    if (fgetc(textfile) != EOF) {
        fprintf(stderr, "ERROR: text too large\n");
    }

    fclose(textfile);
}