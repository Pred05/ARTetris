/* piece.h */
#include "shapes.h"

#define PIECE_H
#define PIVOT_X 1
#define PIVOT_Y 2
#define TRUE 1
#define FALSE 0

typedef int BOOL;

 
enum { CYAN = 1, BLUE, ORANGE, YELLOW, GREEN, PURPLE, RED, GHOST }; // Les couleurs de chaque pi�ce, GHOST est pour la pi�ce fant�me

typedef struct Pieces {
    int kind; // Le type de la pi�ce
    int orientation; // Son orientation (sa rotation courante)
    int color; // Sa couleur
 
    int posX; // Son ordonn�e dans l'aire de jeu
    int posY; // Son abscisse dans l'aire de jeu
 
} Piece;
    /*Piece();
    Piece(int k, int o);
    Piece(const Piece &p);
 
    void setKind(int k);
    void setOrientation(int o);
 
    int getKind();
    int getOrientation();
 
    void setColor(int c);
    int getColor();
 
    void setPosX(int x);
    int getPosX();
 
    void setPosY(int y);
    int getPosY();
};*/
 