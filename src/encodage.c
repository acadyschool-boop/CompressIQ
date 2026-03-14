#include "encodage.h"
#include "LUT.h"

static void vide_Encodage(Encodage * encodage){
    free(encodage->compression->pixmap);
    free(encodage->compression->pixmap_compression);
    free(encodage->compression);
    free(encodage->headerPNM->fileName);
    free(encodage->headerPNM);
    free(encodage);
}


static Encodage * initializeEncodage(){
    Encodage * encodage = malloc(sizeof(Encodage));
    if(!encodage)
        exit(EXIT_FAILURE);
    encodage->headerPNM = calloc(1, sizeof(HeaderPNM));
    if(!encodage->headerPNM)
        exit(EXIT_FAILURE);
    encodage->compression = calloc(1,sizeof(Compression));
    if(!encodage->compression)
        exit(EXIT_FAILURE);
    return encodage;
    
}


void lireName(int lenght , Encodage * encodage , const char * name){
    encodage->headerPNM->fileName = malloc(sizeof(char) * lenght);
    if(!encodage->headerPNM->fileName)
        exit(EXIT_FAILURE);
    strcpy(encodage->headerPNM->fileName , name);
}

static void load_Pixmap(Compression * compression , int size){
    compression->size = size;
    compression->pixmap = calloc(compression->size , sizeof(unsigned char));
    int max_bits  = compression->size * 1.375 ;
    compression->pixmap_compression = calloc(max_bits, sizeof(unsigned char));
    if(!compression->pixmap || !compression->pixmap_compression){
        fprintf(stderr , "impossible de charger le tableau DIF\n");
        exit(EXIT_FAILURE);
    }
    compression->pos = 8;
}



static int readint(int *x, char *str){
    char * endptr;
    errno = 0;
    if(!str)
        return 0;
    int val = strtoll(str , &endptr , 10);

    switch (errno)
    {
    case ERANGE:
        return 0;
        break;
    
    default:
        if(*endptr != '\0'&& *endptr != '\n'){
            printf("lecture jusqu'au bout %c\n" , *endptr);
            return 0;
        }
            
        break;
    }

    *x = val;
    return 1;
    
}

static void updatePNM(HeaderPNM * pnm , char * str){

    int erreur , x;
    switch (pnm->header)
            {
            case MAGIC_NUMBER:
                pnm->magic_number =  str[1];
                pnm->plan = (pnm->magic_number == '5') ? 1 : 3;
                pnm->layer =  (pnm->magic_number == '5') ? 0xD1FF : 0xD3FF;
                pnm->header++;
                break;
            case WIDTH:
                
                erreur = readint(&x , str);
                pnm->width = (unsigned short) x;
                if(!erreur){
                    fprintf(stderr , "lecture impossible de width\n");
                    exit(EXIT_FAILURE);
                }
                    
                pnm->header++;
                break;
            case HEIGHT:
                erreur = readint(&x , str);
                pnm->height = (unsigned short) x;
                if(!erreur){
                    fprintf(stderr , "lecture impossible de height\n");
                    exit(EXIT_FAILURE);
                }
                pnm->header++;
                break;
            case MAX:
                erreur = readint(&pnm->max , str);
                if(!erreur){
                    fprintf(stderr , "lecture impossible de max\n");
                    exit(EXIT_FAILURE);
                }
                pnm->header++;
            default:
                break;
            }
       
}

static void extract_word(char * buffer , HeaderPNM *pnm ){
    char * separators = " ";
    char * strToken = strtok(buffer , separators);
    for (; strToken != NULL ; strToken = strtok(NULL, separators))
    {
        updatePNM(pnm , strToken);
    }
    
}


static void read_Header(FILE * file , HeaderPNM * pnm){

    char buffer[LEN_LINE];
    for (; pnm->header <= MAX; )
        {
            char * error = fgets(buffer , sizeof(buffer) , file);
            if(!error){
                fprintf(stderr , "impossible de lire la ligne \n");
                exit(EXIT_FAILURE);
            } 
            switch (buffer[0])
            {
            case '#':
                break;
            
            default:
                extract_word(buffer , pnm);
                break;
            }
            

    }
}



void read_file(FILE * file , Img * img){

    for (int i = 0;fscanf(file , "%s" , img->code[i]) != EOF ; i++);
 
    
    
    
}


static char * extractword(char * buffer ){
    char * separators = ".";
    char * strToken = strtok(buffer , separators);
    return strToken;
    
}



bool test_format(Img * img , char * category , char * format , char * encoding){
    if(strstr(img->code[0] , category) && strstr(img->code[0] , format) && strstr(img->code[1] , encoding))
        return true;
    return false;
}


void convert_file(char * category , Encodage * encodage , const char * fileName){
    char buffer[TAILLE] = {0}, command[TAILLE] = {0}, bufferName[TAILLE] = {0};
    strcpy(bufferName, fileName);
    
    char * name = extractword(bufferName);
    if (!name){
        fprintf(stderr , "name null");
        return;
    }
    if(strstr(category, "audio")){
        sprintf(buffer, "sox %s -n spectrogram -o %s.png", fileName, name);
        FILE *f1 = popen(buffer, "r");
        if(f1) pclose(f1);

        sprintf(command, "convert %s.png %s.pnm", name, name);
    } 
    else {
        sprintf(command, "convert %s %s.pnm", fileName, name);
    }

    FILE *f2 = popen(command, "r");
    if(f2) pclose(f2);

    char finalName[TAILLE];
    sprintf(finalName, "%s.pnm", name); 
    lireName(strlen(finalName), encodage, finalName);
}


int check_file(const char *filepath, char* category, char* format, char* encoding , Encodage * encodage){
    char string[TAILLE] ={0};
    strcat(string , "file -ib ");
    strcat(string , filepath);
    Img img;
    FILE *file = popen(string , "r");
    if(!file){
        fprintf(stderr , "file null\n");
        return 0;
    }
        
    read_file(file , &img);
    bool result = test_format(&img , category , format , encoding);
    
    convert_file(img.code[0] , encodage , filepath);
    
    pclose(file);
    return result;
}







static Encodage * read_PNM(const char * name){
    Encodage * encodage = initializeEncodage();
    char buffer[TAILLE] = {0};
    strcpy(buffer , name);
    check_file(name, extractword(buffer),"pnm" , "binary",encodage);
    FILE * file = fopen(encodage->headerPNM->fileName , "rb");
    if(!file){
        fprintf(stderr, "fichier introuvable name : %s\n" , encodage->headerPNM->fileName);
        exit(EXIT_FAILURE);
    }

    read_Header(file , encodage->headerPNM);
    load_Pixmap(encodage->compression ,(int) encodage->headerPNM->height *(int) encodage->headerPNM->width * encodage->headerPNM->plan);
    size_t sizeLu = fread(encodage->compression->pixmap , sizeof(unsigned char) , encodage->compression->size , file);
    
    if((int)sizeLu != encodage->compression->size){
        fprintf(stderr , "erreur on ne lit pas assez d'élèments \n");
        exit(EXIT_FAILURE);
    }
    return encodage;
}

static unsigned char sendBit(unsigned char x , int i){
    return (x >> i) & 1;
}


static void fullBit(Compression * pixmap){
    if (pixmap->pos == 0) {
        pixmap->index++;
        pixmap->pos = 8;
        pixmap->pixmap_compression[pixmap->index] = 0; 
    }
}


static void pushbits(Compression * pixmap , LUT * lut , int j){
        
    unsigned char tmp;
    for (int i = lut[j].p - 1; i >= 0 ; i--)
    {
        fullBit(pixmap);
        pixmap->pos--;
        tmp = sendBit(lut[j].C_k , i);
        tmp <<= pixmap->pos;
        pixmap->pixmap_compression[pixmap->index] |= tmp; 
        
        
    }

    for (int i = lut[j].q - 1; i >= 0 ; i--)
    {
        fullBit(pixmap);
        pixmap->pos--;  
        tmp = sendBit(lut[j].valeur_translate , i);
        tmp <<= pixmap->pos;
        pixmap->pixmap_compression[pixmap->index] |= tmp; 
        
        
        
    }
    
}

static unsigned char conversionS_To_U(signed char x){
    return (x < 0) ? (unsigned char) (-2* x - 1) : (unsigned char)(2 * x);
}





static void amplitudePixel1D(Compression *pnm , LUT * lut){
    for (int i = 1; i < pnm->size; i++)
    {
        signed char x = (int)pnm->pixmap[i] - (int)pnm->pixmap[i - 1];
        unsigned char y = conversionS_To_U(x);
        int j = find_interval(lut , y);
        pushbits(pnm ,lut , j);

    }
    
}


static void amplitudePixel3D(Compression * pnm , LUT * lut){
    for (int i = 3; i < pnm->size; i+=3)
    {
        
        signed char x1 = (int )pnm->pixmap[i] - (int )pnm->pixmap[i - 3];
        signed char x2 = (int)pnm->pixmap[i + 1] - (int)pnm->pixmap[i - 2];
        signed char x3 = (int )pnm->pixmap[i + 2] - (int )pnm->pixmap[i - 1];

        unsigned char y1 = conversionS_To_U(x1);
        unsigned char y2 = conversionS_To_U(x2);
        unsigned char y3 = conversionS_To_U(x3);
        int j1 = find_interval(lut , y1);
        pushbits(pnm ,lut , j1);
        int j2 = find_interval(lut , y2);
        pushbits(pnm ,lut , j2);
        int j3 = find_interval(lut , y3);
        pushbits(pnm ,lut , j3);



    }
    
}





static void write_DIF(const char * name , Encodage * encodage){
    uint8_t valeur_translate_len[4] = {1 , 2 , 4 , 8};

    FILE * file = fopen(name , "wb");

    if(!file){
        fprintf(stderr , "impoosible d'écrire dans le fichier name : %s\n" , name);
        exit(EXIT_FAILURE);
    }
    uint16_t magicNumber = (encodage->headerPNM->magic_number == '5')? (uint16_t)0xD1FF : (uint16_t)0xD3FF;
    size_t size = fwrite(&magicNumber , sizeof(uint16_t) , 1 , file);
    if(size != 1)
        exit(EXIT_FAILURE);
    uint16_t width =(uint16_t) encodage->headerPNM->width; 
    size = fwrite(&width, sizeof(uint16_t) , 1 , file);
    if(size != 1)
        exit(EXIT_FAILURE);
    uint16_t height =(uint16_t) encodage->headerPNM->height; 

    size = fwrite(&height , sizeof(uint16_t) , 1 , file);
    if(size != 1)
        exit(EXIT_FAILURE);
    uint8_t niveau = 4;
    size = fwrite(&niveau , sizeof(uint8_t) , 1 , file);
    if(size != 1)
        exit(EXIT_FAILURE);
    size = fwrite(valeur_translate_len , sizeof(uint8_t) , 4 , file);
    if(size != 4)
        exit(EXIT_FAILURE);
 
    size = fwrite(encodage->compression->pixmap_compression, sizeof(uint8_t) , encodage->compression->index , file);

    if((int)size != encodage->compression->index)
            exit(EXIT_FAILURE);
    fclose(file);
}

static void addFirstPixel(Compression * compression , int plan){
    for (int i = 0; i < plan; i++)
    {
        compression->pixmap_compression[i] = compression->pixmap[i];
    }
    compression->index = plan;
}

extern int pnmtodif(const char* pnminput, const char* difoutput){
    FILE * file = fopen(pnminput , "rb");
    if(!file)
        exit(EXIT_FAILURE);

    Encodage * encodage = read_PNM(pnminput);
    LUT *lut = initializationLUT();
    addFirstPixel(encodage->compression , encodage->headerPNM->plan);

    switch (encodage->headerPNM->plan)
    {
    case 1:
        amplitudePixel1D(encodage->compression , lut);
        break;
    
    default:
        amplitudePixel3D(encodage->compression , lut);
        break;
    }

    write_DIF(difoutput , encodage);
    free(lut);
    vide_Encodage(encodage);
    fclose(file);
    return 0;
}


