#ifndef _LUT_H_
#define _LUT_H_
#define LEN 4

typedef struct quantificateur{
    unsigned char b_inf;
    unsigned char b_sup;
    unsigned char p;
    unsigned char valeur_translate;
    unsigned char C_k;
    unsigned char L;
    unsigned char q;
}LUT;



typedef struct TauxCompression{
    int histogramme[LEN];
    float taux;
}TauxCompression;

LUT * initializationLUT();
int find_interval(LUT * lut , unsigned char pixel);

#endif