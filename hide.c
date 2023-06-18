#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structs.h"

void hide(FILE* file, char* filename2, BMP_Header read_bmp_header, DIB_Header read_dib_header) {
    // read in the text from filename2 into a string
    char* hidden_text; // string to store the hidden message from filename2
    long length; // length for the string hidden_text

    FILE* textfile = fopen(filename2, "r+");
    if (textfile == NULL) {
        fprintf(stderr, "Error opening text file\n");
        exit(1);
    }

    // getting the length of the hidden message to allocate the appropriate amount of space
    fseek(textfile, 0, SEEK_END);
    length = ftell(textfile);
    fseek(textfile, 0, SEEK_SET);

    hidden_text = malloc(length);
    if (hidden_text == NULL) {
        fprintf(stderr, "Error allocating memory for text\n");
    }

    fread (hidden_text, 1, length, textfile);
    fclose (textfile);

    // if the text is too large to fit in filename1
    if (length > read_dib_header.image_size) {
        fprintf(stderr, "ERROR: text too large\n");
    }

    // moving file pointer to the start of the pixel array
    fseek(file, read_bmp_header.offset, SEEK_SET);

    bool flag = 0; // flag to break out of the nested loop

    // loop to read each row/col
    int k = 0;
    for (int i = 0; i < read_dib_header.height; i++) { // rows
        if (flag) {
            break;
        }
        for (int j = 0; j < read_dib_header.width; j++) { // cols
            if (k < length) {
                char c = hidden_text[k]; // hidden char 
                char c1 = c & 0xF0; // 4 MSB of c followed by four 0s
                char c2 = c & 0x0F; // 4 LSB of c preceded by four 0s

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

                k++;
            } else {
                /*
                // writing a 0 to the 4 LSB of green and red to denote the text is over 
                Pixel_Array pixel_array;
                fread(&pixel_array, sizeof(pixel_array), 1, file);

                pixel_array.green = pixel_array.green & 0xF0;
                pixel_array.red = pixel_array.red & 0xF0;
                
                // to get back to the correct pixel for writing
                fseek(file, -sizeof(pixel_array), SEEK_CUR); 

                fwrite(&pixel_array, sizeof(pixel_array), 1, file); */

                // writing a 0 to the image to denote the text is over 
                Pixel_Array pixel_array;

                pixel_array.blue = '\0';
                pixel_array.green = '\0';
                pixel_array.red = '\0';

                fwrite(&pixel_array, sizeof(pixel_array), 1, file); 

                flag = 1;
                break;
            }
            
        }

        // checking for extra padding at end of row and skipping them if they exist
        if ((3 * read_dib_header.width) % 4 != 0) { // not a multiple of 4
            fseek(file, 4 - ((3 * read_dib_header.width) % 4), SEEK_CUR); // advance by the remainder of the way to 4
        } 
    }

    free(hidden_text);
}