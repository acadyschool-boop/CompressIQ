#ifndef _DECODAGE_H_
#define _DECODAGE_H_
#include "quadtree.h"

typedef enum Header_File{
    MAGIC_NUMBER_D = 0 , WIDTH_D = 1 , HEIGHT_D = 2 , NIVEAU = 3 , INTERVAL = 4 , FIRST_PIXEL = 5
}Header_File;


typedef struct HeaderDIF{
    int layer;
    int width;
    int height;
    int max;
    Header_File header;
}HeaderDIF;

typedef struct Decompression{
    Quadtree * quadtree_decompression;
    unsigned char * pixmap_decompression;
    unsigned char * pixmap_compression;
    int size_Compression;
    int size_Decompression;
    int pos;
    int indexCompression;
    int indexDecompression;
}Decompression;


typedef struct Decodage{
    HeaderDIF * header;
    Decompression * decompression;
}Decodage;

int diftopnm(const char* difinput, const char* pnmoutput);
#endif