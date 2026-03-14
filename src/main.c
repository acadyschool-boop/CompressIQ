#include "encodage.h"
#include "decodage.h"


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define TAILLE 256




static char * extract_word(char * buffer ){
    char * separators = ".";
    char * strToken = strtok(buffer , separators);
    return strToken;
    
}




void gerer_options(int argc, char *argv[]) {
    int opt;
    char *fichier_entree = NULL;
    int mode = 0; // 1 pour encodage, 2 pour décodage
    char buffer[TAILLE] = {0} , fileName[TAILLE] = {0};
    // ":" après une lettre signifie que l'option attend un argument (le nom du fichier)
    while ((opt = getopt(argc, argv, "e:d:ht")) != -1) {
        switch (opt) {
            case 'e':
                mode = 1;
                fichier_entree = optarg; // optarg contient le texte juste après -e
                break;
            case 'd':
                mode = 2;
                fichier_entree = optarg; // optarg contient le texte juste après -d
                break;
            case 'h':
                printf("Aide : utilisez -e [fichier] pour encoder ou -d [fichier] pour décoder.\n");
                return;
            case 't':
                printf("Mode test activé.\n");
                break;
            case ':': // Si l'option est présente mais sans argument
                printf("L'option -%c nécessite un nom de fichier.\n", optopt);
                break;
            case '?':
                printf("Option inconnue : -%c\n", optopt);
                break;
        }
    }

    // Vérification après la boucle
    if (mode == 1 && fichier_entree) {
        printf("Lancement de l'ENCODAGE sur : %s\n", fichier_entree);
        strcpy(fileName , fichier_entree);
        sprintf(buffer  , "%s.dif", extract_word(fichier_entree));
        pnmtodif(fileName, buffer);
    } else if (mode == 2 && fichier_entree) {
        printf("Lancement du DÉCODAGE sur : %s\n", fichier_entree);
        strcpy(fileName , fichier_entree);
        sprintf(buffer  , "%s.pnm" , extract_word(fichier_entree));
        diftopnm(fileName, buffer);
        // Appelle ta fonction de décodage ici
    } else {
        printf("Erreur : Vous devez spécifier -e ou -d avec un fichier.\n");
    }
}

int main(int argc , char *argv[]){
    gerer_options(argc, argv);
    return 0;
}