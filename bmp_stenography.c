#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
        fprintf(stderr, "ERROR: need to provide --<type> and up to two <filename> arguments\n");
        exit(1);
    }

    char* type = argv[1];
    char* filename = argv[2];
    char* filename2 = argv[3];

    // opening the <filename> argument to be read
    FILE* file = fopen(filename, "r+");
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
        fprintf(stderr, "ERROR: file format not supported\n");
        exit(1);
    }

    // checking that the size of the DIB header is 40
    if (read_dib_header.size != 40) {
        fprintf(stderr, "ERROR: file format not supported\n");
        exit(1);
    }

    // checking that the pixel data is encoded in 24-bit RGB
    if (read_dib_header.bits_per_pixel != 24) {
        fprintf(stderr, "ERROR: file format not supported\n");
        exit(1);
    }


    if (!strcmp(type, "--info")) { // if the argument is --info
        // printing the contents of the BMP header and DIB header from the created structs
        printf("=== BMP Header ===\nType: %c%c\nSize: %d\nReserved 1: %d\nReserved 2: %d\nImage Offset: %d\n\n=== DIB Header ===\nSize: %d\nWidth: %d\nHeight: %d\n# color panes: %d\n# bits per pixel: %d\nCompression scheme: %d\nImage size: %d\nHorizontal resolution: %d\nVertical resolution: %d\n# colors in palette: %d\n# important colors: %d\n", read_bmp_header.format_identifier1, read_bmp_header.format_identifier2, read_bmp_header.size_in_bits, read_bmp_header.reserved1, read_bmp_header.reserved2, read_bmp_header.offset, read_dib_header.size, read_dib_header.width, read_dib_header.height, read_dib_header.color_panes, read_dib_header.bits_per_pixel, read_dib_header.compression_scheme, read_dib_header.image_size, read_dib_header.horizontal_res, read_dib_header.vertical_res, read_dib_header.num_of_colors, read_dib_header.num_of_important_colors);
    } else if (!strcmp(type, "--reveal")) {
        // moving file pointer to the start of the pixel array
        fseek(file, read_bmp_header.offset, SEEK_SET);
        
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

                // if there's an encoded 0 for the pixel then its the end of the text so break
                if (pixel_array.blue == '\0' && pixel_array.green == '\0' && pixel_array.red == '\0') {
                    flag = 1;
                    break;
                }

                // transforming it
                char g_lsb = pixel_array.green & 0x0F; // 4 LSB of green preceded by 0000
                char r_lsb = pixel_array.red & 0x0F; // 4 LSB of red preceded by 0000
                char b_lsb = pixel_array.blue & 0x0F; // 4 LSB of blue preceded by 0000

                // what XOR'd with b_lsb gets you g_lsb? <- that will reveal the secret 4 LSB of green
                g_lsb = g_lsb ^ b_lsb;
                // what XOR'd with b_lsb gets you r_lsb? <- that will reveal the secret 4 LSB of red
                r_lsb = r_lsb ^ b_lsb;

                // green lsb is nybble1 and red lsb is nybble2 for the hidden char
                char c = (g_lsb << 4) | r_lsb;

                printf("%c", c);

            }

            // checking for extra padding at end of row and skipping them if they exist
            if ((3 * read_dib_header.width) % 4 != 0) { // not a multiple of 4
                fseek(file, 4 - ((3 * read_dib_header.width) % 4), SEEK_CUR); // advance by the remainder of the way to 4
            } 
        }
        printf("\n");
    } else if (!strcmp(type, "--hide")) {
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

        // read filename1 and transform the current pixel according to the current char and if you reach the strings sentinel stop by encoding a 0
        FILE* file = fopen (filename, "r+");
        if (file == NULL) {
            fprintf(stderr, "Error opening bmp file\n");
            exit(1);
        }

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
    fclose(file);

    return 0;
}