#include "LUT.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define LEN_LINE 100
#include <stdbool.h>
#include <string.h>
#include <errno.h>



extern LUT * initializationLUT(){
    unsigned char array[] = {0,2,2,6,6,22,22,255};
    unsigned char prefix[] = {0b0 , 0b10 , 0b110 ,0b111};
    unsigned char prefix_len[] = {1 , 2 , 3 , 3};
    unsigned char valeur_translate_len[] = {1 , 2 , 4 , 8};
    int j = 0;
    LUT * l = malloc(sizeof(LUT) * LEN);
    if(!l)
        exit(EXIT_FAILURE);
    for (int i = 0; i < LEN; i++)
    {
        l[i].b_inf = array[j];
        l[i].b_sup = array[j + 1];
        l[i].p = prefix_len[i];
        l[i].q = valeur_translate_len[i];
        l[i].C_k = prefix[i];
        l[i].L = l[i].b_sup - l[i].b_inf;

        j +=2;
    }
    
    return l;

}



extern int find_interval(LUT * lut , unsigned char y){
    for (int i = 0; i < LEN; i++)
    {
        if((y >= lut[i].b_inf && y < lut[i].b_sup) || (y == lut[i].b_sup && lut[i].b_sup == 255)){
            lut[i].valeur_translate = (y - lut[i].b_inf < 0) ? 0 : y - lut[i].b_inf;
            return i;
        }
            
    }
    return -1;
    
}

