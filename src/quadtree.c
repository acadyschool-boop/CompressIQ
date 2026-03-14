#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define LEN_LINE 100
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "quadtree.h"


static Node * initializationNode(Node * lastNode){
    Node *node = malloc(sizeof(Node));
    if(!node){
        fprintf(stderr ,"erreur node \n");
        exit(EXIT_FAILURE);
    }
    node->LastNode = malloc(sizeof(Node));
    if(!node->LastNode)
        exit(EXIT_FAILURE);
    node->LastNode = lastNode;
    return node;
}

static void initializationFirstNode(Quadtree * quadtree,int plan){
    for (int i = 0; i < plan; i++)
    {
        quadtree[i] = initializationNode(NULL);
    }
    
}

extern Quadtree * initializationQuadtree(int size , int plan){
    Quadtree *quadtree = calloc(size,sizeof(Quadtree));
    if(!quadtree){
        fprintf(stderr , "imposssible de créer \n");
        exit(EXIT_FAILURE);
    }

    initializationFirstNode(quadtree, plan);

    for (int i = plan; i < size; i+= plan)
    {
        
        switch (plan)
        {
        case 1:
            quadtree[i] = initializationNode(quadtree[i - 1]);
            break;
        
        default:
            quadtree[i] = initializationNode(quadtree[i - 3]);
            quadtree[i + 1] = initializationNode(quadtree[i - 2]);
            quadtree[i + 2] = initializationNode(quadtree[i - 1]);
            break;
        }
    
    }
    
    return quadtree;
}



extern void vide_Quadtree(Quadtree * quadtree , int taille){
    for (int i = 0; i < taille; i++)
    {

        free(quadtree[i]->LastNode);
    }
    free(quadtree);
}
