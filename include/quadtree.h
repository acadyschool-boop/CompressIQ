#ifndef _QUADTREE_H_
#define _QUADTREE_H_


typedef struct Node{
    unsigned char x;
    struct Node * LastNode;
}Node,*Quadtree;


Quadtree * initializationQuadtree(int size , int plan);
void vide_Quadtree(Quadtree * quadtree , int taille);
#endif