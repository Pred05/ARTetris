/* board.cpp */
#include "board.h"
 
void flood(int i, int j, int px, int py, int k, int o, int value, BOOL visited[SIZEE][SIZEE])
{
    if(px < 0 || px >= SIZEE || py < 0 || py >= SIZEE || visited[px][py] || PIECES[k][o][px][py] == FREE)
        return;
 
    visited[px][py] = TRUE;

    jeu.area[i][j] = value;
 
    flood(i, j - 1, px, py - 1, k, o, value, visited);
    flood(i + 1, j, px + 1, py, k, o, value, visited);
    flood(i, j + 1, px, py + 1, k, o, value, visited);
    flood(i - 1, j, px - 1, py, k, o, value, visited);
}
 
void floodB(int i, int j, int px, int py, int k, int o, BOOL *flag, BOOL visited[][SIZEE])
{
    if(px < 0 || px >= SIZEE || py < 0 || py >= SIZEE || visited[px][py] || PIECES[k][o][px][py] == FREE)
        return;
 
    visited[px][py] = TRUE;
 

    if(i < 0 || i >= BOARD_WIDTH || j < 0 || j >= BOARD_HEIGHT || jeu.area[i][j] != FREE )
    {
		printf("ici i=%d j=%j \n",i,j);
        *flag = FALSE;
        return;
    }
 
    floodB(i, j - 1, px, py - 1, k, o, flag, visited);
    floodB(i + 1, j, px + 1, py, k, o, flag, visited);
    floodB(i, j + 1, px, py + 1, k, o, flag, visited);
    floodB(i - 1, j, px - 1, py, k, o, flag, visited);
}
 
void floodFill(int i, int j, int px, int py, int k, int o, int value)
{
    BOOL visited[SIZEE][SIZEE];
	int l=0;
    for(l =0; l < SIZEE; ++l)
	{
		int m=0;
        for(m = 0; m < SIZEE; ++m)
            visited[l][m] = FALSE;
	}
 
    flood(i, j, px, py, k, o, value, visited);
}
 
void initBoard()
{
	int i=0;
    for(i = 0; i < BOARD_WIDTH; ++i)
	{
		int j=0;
        for(j = 0; j < BOARD_HEIGHT; ++j)
            jeu.area[i][j] = FREE;
	}
}

void setCurrentPiece(Piece p)
{
    jeu.currentPiece = p;
}
 
/*Piece Board::getCurrentPiece()
{
    return jeu.currentPiece;
}*/
 
void drawPiece(Piece p)
{
	int i = p.posX;
	int j = p.posY;
 
	int k = p.kind;
	int o = p.orientation;
 
    switch(k)
    {
        case I:
			p.color=CYAN;
            break;
        case J:
            p.color=BLUE;
            break;
        case L:
            p.color=ORANGE;
            break;
        case O:
            p.color=YELLOW;
            break;
        case S:
            p.color=GREEN;
            break;
        case T:
            p.color=PURPLE;
            break;
        case Z:
            p.color=RED;
            break;
        default:
            break;
    }
 
	floodFill(i, j, PIVOT_X, PIVOT_Y, k, o, p.color);
}
 
void clearPiece(Piece p)
{
	int i = p.posX;
	int j = p.posY;
 
	int k = p.kind;
	int o = p.orientation;
 
    floodFill(i, j, PIVOT_X, PIVOT_Y, k, o, FREE);
}
 
void newPiece(Piece p)
{
	p.posX=ORIGIN_X;
    p.posY=ORIGIN_Y;
 
    drawPiece(p);
 
	jeu.currentPiece = p;
}
 
BOOL isCurrentPieceMovable(int x, int y)
{

	int visited[SIZEE][SIZEE];
	int l=0;
    BOOL movable;
	int m = 0;
	int k;
	int o;

	movable = TRUE;

	clearPiece(jeu.currentPiece);

    for(l = 0; l < SIZEE; ++l)
	{
        for(m = 0; m < SIZEE; ++m)
            visited[l][m] = FALSE;
	}
 
    k = jeu.currentPiece.kind;
    o = jeu.currentPiece.orientation;
 
    floodB(x, y, PIVOT_X, PIVOT_Y, k, o, &movable, visited);
 
    drawPiece(jeu.currentPiece);
 
    return movable;
}
 
BOOL isCurrentPieceRotable(int o)
{
    BOOL rotable;
 
    BOOL visited[SIZEE][SIZEE];
	int i = 0;
	int j = 0;
	int k;

	rotable=TRUE;
	clearPiece(jeu.currentPiece);
 
	
    for(i = 0; i < SIZEE; ++i)
	{
		
        for(j = 0; j < SIZEE; ++j)
            visited[i][j] = FALSE;
	}
 
    k = jeu.currentPiece.kind;
 
    floodB(jeu.currentPiece.posX, jeu.currentPiece.posY, PIVOT_X, PIVOT_Y, k, o, &rotable, visited);
 
    drawPiece(jeu.currentPiece);
 
    return rotable;
}
 
void moveCurrentPieceDown()
{
    int x = jeu.currentPiece.posX;
    int y = jeu.currentPiece.posY;
 
    if(isCurrentPieceMovable(x, y+1))
    {
        clearPiece(jeu.currentPiece);
        jeu.currentPiece.posY=(y + 1);
 
        drawPiece(jeu.currentPiece);
    }
}
 
void moveCurrentPieceLeft()
{
    int x = jeu.currentPiece.posX;
    int y = jeu.currentPiece.posY;
 
    if(isCurrentPieceMovable(x-1, y))
    {
        clearPiece(jeu.currentPiece);
        jeu.currentPiece.posX=(x-1);
 
        drawPiece(jeu.currentPiece);
    }
}
 
void moveCurrentPieceRight()
{
    int x = jeu.currentPiece.posX;
    int y = jeu.currentPiece.posY;
 
    if(isCurrentPieceMovable(x+1,y))
    {
        clearPiece(jeu.currentPiece);
        jeu.currentPiece.posX=(x + 1);
 
        drawPiece(jeu.currentPiece);
    }
}
 
void rotateCurrentPieceLeft()
{
    int o = jeu.currentPiece.orientation;
 
    if(o > 0)
        o--;
    else
        o = NB_ROTATIONS - 1;
 
    if(isCurrentPieceRotable(o))
    {
        clearPiece(jeu.currentPiece);
 
        jeu.currentPiece.orientation=o;
        drawPiece(jeu.currentPiece);
    }
}
 
void rotateCurrentPieceRight()
{
    int o = jeu.currentPiece.orientation;
 
    if(o < NB_ROTATIONS - 1)
        o++;
    else
        o = 0;
 
    if(isCurrentPieceRotable(o))
    {
        clearPiece(jeu.currentPiece);
        jeu.currentPiece.orientation=o;
 
        drawPiece(jeu.currentPiece);
    }
}
 
void deleteLine(int y)
{
	int j = y;
	int i = 0;
    clearPiece(jeu.currentPiece);

    for(j = y; j > 0; --j)
    {
		
        for(i = 0; i < BOARD_WIDTH; ++i)
            jeu.area[i][j] = jeu.area[i][j-1];
    }
 
    drawPiece(jeu.currentPiece);
}
 
int deletePossibleLines()
{
    int nbLinesDeleted = 0;
 
	int j = 0;
    for(j = 0; j < BOARD_HEIGHT; ++j)
    {
        int i = 0;
 
        for(i=0; i < BOARD_WIDTH && jeu.area[i][j] != FREE; ++i);
 
        if(i == BOARD_WIDTH)
        {
            nbLinesDeleted++;
            deleteLine(j);
        }
    }
 
    return nbLinesDeleted;
}
 
void dropCurrentPiece()
{
    int x = jeu.currentPiece.posX;
    int y = jeu.currentPiece.posY;
 
    while(isCurrentPieceMovable(x, y++))
        moveCurrentPieceDown();
}
 
BOOL isCurrentPieceFallen()
{
    int x = jeu.currentPiece.posX;
    int y = jeu.currentPiece.posY;
 
    if(isCurrentPieceMovable(x, y+1))
        return FALSE;
 
    return TRUE;
}
 
BOOL isGameOver()
{
	int i = 0;
    for(i = 0; i < BOARD_WIDTH; ++i)
    {
        if(jeu.area[i][0] != FREE)
            return TRUE;
    }
 
    return FALSE;
}
 
void clear()
{
	int i = 0;
	int j = 0;
    for(i = 0; i < BOARD_WIDTH; ++i)
    {
        for(j = 0; j < BOARD_HEIGHT; ++j)
            jeu.area[i][j] = FREE;
    }
}