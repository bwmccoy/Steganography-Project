#ifndef STRUCTS_H 
#define STRUCTS_H


#pragma pack(1)
typedef struct _BMP_Header {
    char format_identifier1; 
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

#endif