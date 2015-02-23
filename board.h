/* board.h */
 
#include "piece.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define BOARD_H
#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10
 
#define ORIGIN_X 5
#define ORIGIN_Y 1
 
enum  { FREE, FILLED };
 
 
typedef struct Board {
	Piece currentPiece;
    Piece ghostPiece;
	int area[BOARD_WIDTH][BOARD_HEIGHT];
} Board;

Board jeu;

/*class Board
{
private:
    Piece currentPiece;
    Piece ghostPiece;
 
    void flood(int i, int j, int px, int py, int k, int o, int value, BOOL visited[][SIZEE]);
    void flood(int i, int j, int px, int py, int k, int o, BOOL &flag, BOOL visited[][SIZEE]);
    void floodFill(int i, int j, int px, int py, int k, int o, int value);
 
public:
    int area[BOARD_WIDTH][BOARD_HEIGHT];

    Board();
 
    void setCurrentPiece(Piece p);
    Piece getCurrentPiece();
 
    void drawPiece(Piece p);
    void clearPiece(Piece p);
 
    void newPiece(Piece p);
 
    BOOL isCurrentPieceMovable(int x, int y);
    BOOL isCurrentPieceRotable(int o);
 
    void moveCurrentPieceDown();
    void moveCurrentPieceLeft();
    void moveCurrentPieceRight();
 
    void rotateCurrentPieceLeft();
    void rotateCurrentPieceRight();
 
    void deleteLine(int y);
    int deletePossibleLines();
 
    void dropCurrentPiece();
    BOOL isCurrentPieceFallen();
	BOOL isGameOver();
 
    void clear();
};*/


void flood(int i, int j, int px, int py, int k, int o, int value, BOOL visited[][SIZEE]);
    void floodB(int i, int j, int px, int py, int k, int o, BOOL *flag, BOOL visited[][SIZEE]);
    void floodFill(int i, int j, int px, int py, int k, int o, int value);
	void setCurrentPiece(Piece p);
    Piece getCurrentPiece();
 
    void drawPiece(Piece p);
    void clearPiece(Piece p);
 
    void newPiece(Piece p);
 
    BOOL isCurrentPieceMovable(int x, int y);
    BOOL isCurrentPieceRotable(int o);
 
    void moveCurrentPieceDown();
    void moveCurrentPieceLeft();
    void moveCurrentPieceRight();
 
    void rotateCurrentPieceLeft();
    void rotateCurrentPieceRight();
 
    void deleteLine(int y);
    int deletePossibleLines();
 
    void dropCurrentPiece();
    BOOL isCurrentPieceFallen();
	BOOL isGameOver();
 
    void clear();
 