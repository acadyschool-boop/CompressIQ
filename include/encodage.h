#ifndef _ENCODAGE_H_
#define _ENCODAGE_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define LEN_LINE 100
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#define TAILLE 256
#define LENGHT 30

typedef enum Header{
    MAGIC_NUMBER = 0 , WIDTH = 1 , HEIGHT = 2 , MAX = 3
}Header;

typedef struct img{
    char code[3][TAILLE];

}Img;

typedef struct HeaderPNM{
    Header header;
    char magic_number;
    char * fileName;
    unsigned char layer;
    unsigned short width;
    unsigned short height;
    int plan;
    int max;
}HeaderPNM;


typedef struct Compression{
    unsigned char * pixmap;
    unsigned char * pixmap_compression;
    int size;
    int index;
    int pos;
}Compression;

typedef struct Encodage{
    HeaderPNM * headerPNM;
    Compression * compression;
}Encodage;

int pnmtodif(const char* pnminput, const char* difoutput);

#endif