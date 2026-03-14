#include "./decodage.h"
#include "LUT.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define LEN_LINE 100
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static void vide_Decodage(Decodage * decodage){
    free(decodage->header);
    vide_Quadtree(decodage->decompression->quadtree_decompression , decodage->decompression->size_Decompression);
    free(decodage->decompression->pixmap_compression);
    free(decodage->decompression->pixmap_decompression);
    free(decodage->decompression);
}





static int find_prefix(unsigned char C_k ,int longueur_lue ,LUT * lut){
    for (int i = 0; i < LEN; i++)
    {
        if(lut[i].C_k == C_k && lut[i].p == longueur_lue)
            return i;
    }
    return -1;
}

static void fullBit(Decompression * decompression){
    decompression->indexCompression = (!decompression->pos)? decompression->indexCompression  + 1 : decompression->indexCompression ;
    decompression->pos = (!decompression->pos)? 8 : decompression->pos;
}

unsigned char sendBit(unsigned char x , int i){
    return (x >> i) & 1;
}

int lenght_prefix(Decompression * decompression , LUT * lut){
    unsigned char prefix = 0;
    int pos = decompression->pos;
    int index = decompression->indexCompression;
    int j;
    for (int i = 0; i < 4; i++)
    {
        if(!pos){
            pos = 8;
            index++;
        }
        pos--;
        unsigned char x = sendBit(decompression->pixmap_compression[index] , pos);
        prefix <<= 1;
        prefix |= x;
        j = find_prefix(prefix ,i + 1 ,lut);
        if(j != -1)
            break;

    }
    return j;
    
}



static int pullbits(Decompression * decompression , LUT * lut ){
    unsigned char valeur_translate = 0;
    unsigned char x;
    int j = lenght_prefix(decompression , lut);
    for (int i = 0; i < lut[j].p; i++)
    {
        
        fullBit(decompression);
        decompression->pos--;
       
    }
   
    for (int i = 0; i < lut[j].q  ; i++)
    {
        fullBit(decompression);
        decompression->pos--;
        x = sendBit(decompression->pixmap_compression[decompression->indexCompression] , decompression->pos);
        valeur_translate <<= 1;
        valeur_translate |= x;
        
    }
    return (int) (valeur_translate) + lut[j].b_inf;

    
    
    
}


static unsigned char overflowUnsignedChar(int x){
    if(x < 0)
        return (unsigned char) 0;
    else if(x > 255)
        return (unsigned char) 255;
    return (unsigned char) x;
}




signed char overflowEcartSignedChar(int x){
    if(x > 127)
        return (signed char) 127;
    else if (x < -127)
    {
        return (signed char) -127;
    }
    return (signed char)x;
}

int conversionU_To_S(unsigned char y){
    if(y % 2 == 1 )
        return (int) (-(y + 1) / 2);
    return (int) (y / 2);
    
}


void amplitude1D(Decompression * decompression , LUT * lut){
    for (; decompression->indexDecompression <= decompression->size_Decompression;)
        {
            unsigned char y =overflowUnsignedChar(pullbits(decompression , lut));
            unsigned char x = overflowEcartSignedChar(conversionU_To_S(y));
            decompression->quadtree_decompression[decompression->indexDecompression]->x = decompression->quadtree_decompression[decompression->indexDecompression]->LastNode->x + x;
            decompression->indexDecompression++;
        }
}


void amplitude3D(Decompression * decompression , LUT * lut){
    for (int i = 3; i < decompression->size_Decompression; i+= 3)
    {
        int dr = conversionU_To_S(pullbits(decompression, lut));
        int dg = conversionU_To_S(pullbits(decompression, lut));
        int db = conversionU_To_S(pullbits(decompression, lut));

        decompression->quadtree_decompression[i]->x   = decompression->quadtree_decompression[i]->LastNode->x + dr;
        decompression->quadtree_decompression[i + 1]->x = decompression->quadtree_decompression[i + 1]->LastNode->x + dg;
        decompression->quadtree_decompression[i + 2]->x = decompression->quadtree_decompression[i + 2]->LastNode->x + db;
        
    }
    
}
static void decompressionPixmap(Decompression * decompression , LUT * lut , int plan){
    switch (plan)
    {
    case 1:
        amplitude1D(decompression , lut);
        break;
    
    default:
        amplitude3D( decompression , lut);
        break;
    }
}



static Decodage * initializeDecodage(){
    Decodage * decodage = malloc(sizeof(Decodage));
    if(!decodage)
        exit(EXIT_FAILURE);
    decodage->header = calloc(1, sizeof(HeaderDIF));
    if(!decodage->header)
        exit(EXIT_FAILURE);
    decodage->decompression = calloc(1,sizeof(Decompression));
    if(!decodage->decompression)
        exit(EXIT_FAILURE);
    return decodage;
    
}


static void load_Pixmap(Decompression * decompression , int sizeCompression , int sizeDecompression , int plan){
    decompression->size_Compression = sizeCompression;
    decompression->size_Decompression = sizeDecompression;
    decompression->pixmap_compression = calloc(decompression->size_Compression , sizeof(unsigned char));
    decompression->quadtree_decompression = initializationQuadtree(sizeDecompression + 1 , plan);
    decompression->pixmap_decompression = calloc(decompression->size_Decompression + 1, sizeof(unsigned char));
    if(!decompression->pixmap_compression || !decompression->pixmap_decompression){
        fprintf(stderr , "impossible de charger le tableau DIF\n");
        exit(EXIT_FAILURE);
    }
    decompression->pos = 8;
    decompression->indexCompression = plan;
    decompression->indexDecompression = plan;
}

bool check_valeur_translate_len(uint8_t valeur_translate_len[4]){
    uint8_t valeur_translate_len_correct[] = {1 , 2 , 4 , 8};
    for (int i = 0; i < 4; i++)
    {
        if(valeur_translate_len_correct[i] != valeur_translate_len[i])
            return false;
    }
    return true;
}
static void update_Header(HeaderDIF * header  , Decompression * decompression, FILE * file){
    for (int i = 0; i < 5; i++)
    {
        switch (i)
        {
        case MAGIC_NUMBER_D:
            uint16_t magic_number;
            size_t size = fread(&magic_number, sizeof(uint16_t) , 1 , file);
            if(size != 1)
                exit(EXIT_FAILURE);
            header->layer = (magic_number == (uint16_t) (0xD1FF) )? 1 : (magic_number == (uint16_t) (0xD3FF)? 3 : -1 );
            if(header->layer == -1)
                exit(EXIT_FAILURE);
            break;
        case WIDTH_D:
            uint16_t width;
            size = fread(&width, sizeof(uint16_t) , 1 , file);
            if(size != 1)
                exit(EXIT_FAILURE);
            header->width = (int) width;
            break;
        case HEIGHT_D:
            uint16_t height;
            size = fread(&height, sizeof(uint16_t)  , 1 , file);
            if(size != 1)
                exit(EXIT_FAILURE);
            header->height = (int) height;
            load_Pixmap(decompression ,1.375 * header->height * header->width * header->layer , header->height * header->width * header->layer , header->layer);

            break;
        case NIVEAU:
            uint8_t niveau;
            size = fread(&niveau, sizeof(uint8_t) , 1 , file);
            if(size != 1 || niveau != 4)
                exit(EXIT_FAILURE);

            break;

        case INTERVAL:
            uint8_t valeur_translate_len[4];
            size = fread(valeur_translate_len, sizeof(uint8_t) , 4 , file);
            if(size != 4 || !check_valeur_translate_len(valeur_translate_len))
                exit(EXIT_FAILURE);
            break;
    

        default:
            break;
        }
    }
    
}



static Decodage * lireDIF(const char * name){
    FILE * file = fopen(name , "rb");
    if(!file)
        exit(EXIT_FAILURE);
    Decodage * decodage = initializeDecodage();
    update_Header(decodage->header , decodage->decompression, file);
    size_t sizeLu = fread(decodage->decompression->pixmap_compression, sizeof(uint8_t) , decodage->decompression->size_Compression , file);
    if((int)sizeLu > decodage->decompression->size_Compression){
        fprintf(stderr , "erreur de lecture du fichier dif\n");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    
    return decodage;
}

static void addFirstPixel(Decompression * decompression){
    for (int i = 0; i < decompression->indexCompression; i++)
    {
        decompression->quadtree_decompression[i]->x = decompression->pixmap_compression[i];
    }
    
}


void convertQuadtreeToArray(Decompression * decompression){
    for (int i = 0; i < decompression->size_Decompression; i++)
    {
        decompression->pixmap_decompression[i] = decompression->quadtree_decompression[i]->x;
    }
    
}

void write_PNM(const char * name , Decodage * pnm){
    FILE * file = fopen(name , "wb");
    if(!file){
        fprintf(stderr , "impoosible d'écrire dans le fichier");
        exit(EXIT_FAILURE);
    }
    char commentaire[] = "# Created by GIMP version 2.10.30 PNM plug-in";
    fprintf(file , "%s\n", (pnm->header->layer == 1) ? "P5" : "P6");
    fprintf(file , "%s\n", commentaire);
    fprintf(file , "%d %d\n" , pnm->header->width , pnm->header->height);
    fprintf(file , "%d\n" , 255);
    size_t size = fwrite(pnm->decompression->pixmap_decompression , sizeof(unsigned char) , pnm->decompression->size_Decompression , file);
    if((int)size != pnm->decompression->size_Decompression){
        fprintf(stderr , "erreur on ecrit pas le même nombre de données\n");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}


extern int diftopnm(const char* difinput, const char* pnmoutput){
    Decodage * decodage = lireDIF(difinput);
    LUT * lut = initializationLUT();
    addFirstPixel(decodage->decompression);
    decompressionPixmap(decodage->decompression , lut , decodage->header->layer);
    convertQuadtreeToArray(decodage->decompression);
    write_PNM(pnmoutput , decodage);
    vide_Decodage(decodage);
    return 0;
}




