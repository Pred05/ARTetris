#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef __APPLE__
#  include <GL/glut.h>
#else
#  include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/video.h>

#include "object.h"
#include "board.h"

#define COLLIDE_DIST 15000.0f

#define LARGEURCUBE 10
#define LARGEUR (BOARD_WIDTH*LARGEURCUBE)
#define HAUTEUR (BOARD_HEIGHT*LARGEURCUBE)


/* Object Data */
char            *model_name = "Data/object_dataTetris";
ObjectData_T    *object;
int             objectnum;

int             xsize, ysize;
int				thresh = 100;
int             count = 0;

/* set up the video format globals */

#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

static int clock_start;
static int clock_hop;
static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static int draw( ObjectData_T *object, int objectnum );
static int  draw_object( int obj_id, double gl_para[16] );
static int areClose;
static int rotationPlus=0;
static  GLfloat   red[]				= {1.0, 0.0, 0.0, 1.0};
static  GLfloat   blue[]				= {0.0, 0.0, 1.0, 1.0};
static  GLfloat   green[]				= {0.0, 1.0, 0.0, 1.0};
static int translateDown = 300;
static int translateX = 10;
static int changement = 0;
static int changement2= 0;
static int changement3= 0;
static int tempsChute = 1500;
static int tempsRotation = 750;
static int isDraw = 0;
static int score=0;
static BOOL pause = TRUE;
static char scoreTitre[5];
static char scoreChar[4];
static char gameOverChar[9];
static BOOL isGameOverVar;



static int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}

static float transformX(int x) {
	return ((-LARGEUR/2) + (x * LARGEURCUBE) + (LARGEURCUBE/2));
}

static float transformY(int y) {
	return (HAUTEUR -( y * LARGEURCUBE) );
}

static void setOpenGLColor(int color) {
	if(CYAN==color)
		glColor3f(129.0/255.0,207.0/255.0,224.0/255.0);
	else if(BLUE==color)
		glColor3f(31.0/255,58.0/255,147.0/255);
	else if(ORANGE==color)
		glColor3f(244.0/255,179.0/255,80.0/255);
	else if(YELLOW==color)
		glColor3f(249.0/255,191.0/255,59.0/255);
	else if(GREEN==color)
		glColor3f(39.0/255,194.0/255,129.0/255);
	else if(PURPLE==color)
		glColor3f(145.0/255,61.0/255,136.0/255);
	else if(RED==color)
		glColor3f(214.0/255,69.0/255,65.0/255);

}

int main(int argc, char **argv)
{
	//Piece p1;
	//initialize applications
	glutInit(&argc, argv);
    init();
	
	arVideoCapStart();

	clock_start=GetTickCount();
	clock_hop=GetTickCount();
	
	srand(time(NULL));
	jeu.currentPiece.kind = rand_a_b(0,7);//= Piece(rand_a_b(0,7),rand_a_b(0,4));
	jeu.currentPiece.orientation = rand_a_b(0,4);
	jeu.currentPiece.posX=ORIGIN_X;
	jeu.currentPiece.posY=ORIGIN_Y;

	strcpy(scoreTitre,"Score");
	strcpy(gameOverChar,"Game Over");
	score = 100;
	sprintf(scoreChar,"%d",score);

	isGameOverVar = FALSE;

	//start the main event loop
    argMainLoop( NULL, keyEvent, mainLoop );
	
	return 0;
}

static void   keyEvent( unsigned char key, int x, int y)   
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }
}


static void drawTetris()
{
	int i=0,y=0;
	for(i=0;i<BOARD_HEIGHT;i++)
	{
		for(y=0;y<BOARD_WIDTH;y++)
		{
			//printf("[%d]",jeu.area[y][i]);
			if(jeu.area[y][i]!=FREE && jeu.area[y][i]!=GHOST)
			{
				glPushMatrix();
					setOpenGLColor(jeu.area[y][i]);
					glTranslatef(transformX(y),0 , transformY(i));
					glRotatef(rotationPlus,0,0,1.0);
					glutSolidCube(LARGEURCUBE);
				glPopMatrix();
			}
		}
		//printf("\n");
	}
}

static void drawGameOver() {
	int l,lScore,i;

	l = (int)strlen(gameOverChar);
	lScore = (int)strlen(scoreChar);

	glPushMatrix();
		glLineWidth(3.0);
		glTranslatef(-57,0.0,HAUTEUR/2);
		glColor3f(0.0,0.0,0.0);
		glPushMatrix();
			glScalef(0.16,0.2,0.2);
			glRotatef(90,1,0,0);
			for(i=0;i<l;i++)
			{
				glutStrokeCharacter(GLUT_STROKE_ROMAN,gameOverChar[i]);
			}
		glPopMatrix();
		glPushMatrix();
			glTranslatef(35+20-lScore*5,0,-40);
			glScalef(0.16,0.2,0.2);
			glRotatef(90,1,0,0);
			for(i=0;i<lScore;i++)
			{
				glutStrokeCharacter(GLUT_STROKE_ROMAN,scoreChar[i]);
			}
		glPopMatrix();
	glPopMatrix();
}


/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             i,j,k;
	clock_t time_c=(double)(GetTickCount()- clock_start);
	clock_t time_cc=(double)(GetTickCount()- clock_hop);
	
	

    /* grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
	
    if( count == 0 ) arUtilTimerReset();  
    count++;

	/*draw the video*/
    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

	glColor3f( 1.0, 0.0, 0.0 );
	glLineWidth(6.0);

	/* detect the markers in the video frame */ 
	if(arDetectMarker(dataPtr, thresh, 
		&marker_info, &marker_num) < 0 ) {
		cleanup(); 
		exit(0);
	}
	for( i = 0; i < marker_num; i++ ) {
		argDrawSquare(marker_info[i].vertex,0,0);
	}

	/* check for known patterns */
    for( i = 0; i < objectnum; i++ ) {
		k = -1;
		for( j = 0; j < marker_num; j++ ) {
	        if( object[i].id == marker_info[j].id) {
				/* you've found a pattern */
				//printf("Found pattern: %d ",patt_id);
				glColor3f( 0.0, 1.0, 0.0 );
				argDrawSquare(marker_info[j].vertex,0,0);

				if( k == -1 ) k = j;
		        else /* make sure you have the best pattern (highest confidence factor) */
					if( marker_info[k].cf < marker_info[j].cf ) k = j;
			}
		}
		if( k == -1 ) {
			object[i].visible = 0;
			continue;
		}
		
		/* calculate the transform for each marker */
		if( object[i].visible == 0 ) {
            arGetTransMat(&marker_info[k],
                          object[i].marker_center, object[i].marker_width,
                          object[i].trans);
        }
        else {
            arGetTransMatCont(&marker_info[k], object[i].trans,
                          object[i].marker_center, object[i].marker_width,
                          object[i].trans);
        }
        object[i].visible = 1;
	}

	/// PROOGGG

	if( object[0].visible && object[1].visible && object[2].visible && object[3].visible) {
		pause=FALSE;
	}
	if( object[0].visible==0 && object[1].visible==0 && object[2].visible==0 && object[3].visible==0) {
		pause=TRUE;
	}

	if(pause==0 && isGameOverVar!=1)
	{
		if(time_c>tempsChute)
		{
			clock_start=GetTickCount();
			if(isCurrentPieceFallen()==1)
			{
				Piece p1;
				if (isGameOver()==1) {
					clear();
					isGameOverVar = TRUE;
					return;
				}
				else
				{
					p1.kind = rand_a_b(0,7);//= Piece(rand_a_b(0,7),rand_a_b(0,4));
					p1.orientation = rand_a_b(0,4);
					printf("kind %d , orientation %d ", p1.kind,p1.orientation);
					newPiece(p1);
					score += deletePossibleLines();
					sprintf(scoreChar,"%d",score);
					printf("Mon score : %d\n",score);
				}
			}
			else
			{
				moveCurrentPieceDown();
			}
		}


		if(time_cc>tempsRotation)
		{
			clock_hop=GetTickCount();
			/*check for object collisions between markers*/
			if(object[2].visible && object[4].visible){
				if(checkCollisions(object[2],object[4])){	
					rotateCurrentPieceLeft();
				}
			}
			if(object[3].visible && object[4].visible){
				if(checkCollisions(object[3],object[4])){
					rotateCurrentPieceRight();
				}
			}
		}
	}

	
	arVideoCapNext();
	/* draw the AR graphics */
    draw( object, objectnum );

	/*swap the graphics buffers*/
	argSwapBuffers();
}

static void init( void )
{
	ARParam  wparam;

    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

	/* load in the object data - trained markers and associated bitmap files */
    if( (object=read_ObjData(model_name, &objectnum)) == NULL ) exit(0);
    printf("Objectfile num = %d\n", objectnum);

    /* open the graphics window */
    argInit( &cparam, 2.0, 0, 0, 0, 0 );
}

/* cleanup function called when program exits */
static void cleanup(void)
{
	arVideoCapStop();
    arVideoClose();
    argCleanup();
}



/* draw the the AR objects */
static int draw( ObjectData_T *object, int objectnum )
{
    int     i;
    double  gl_para[16];
       
	glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);


	if(isGameOverVar!=1)
	{
		if(object[2].visible == 0 && changement==1)
		{
			moveCurrentPieceRight();
			changement=0;
		}
		if(object[3].visible == 0 && changement2==1)
		{
			moveCurrentPieceLeft();
			changement2=0;
		}
		if(object[1].visible == 0 &&  changement3==1)
		{
			dropCurrentPiece();
			changement3=0;
		}
		if(object[1].visible)
		{
			changement3=1;
		}
		if(object[2].visible && changement==0)
		{
			changement=1;
		}
		if(object[3].visible && changement2==0)
		{
			changement2=1;
		}
	}
	

    /* calculate the viewing parameters - gl_para */
    for( i = 0; i < objectnum; i++ ) {
        if( object[i].visible == 0 ) continue;
        argConvGlpara(object[i].trans, gl_para);
        draw_object( object[i].id, gl_para);
    }

	
     
	glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
	
    return(0);
}




static void drawFleche(int obj_id)
{
	glPushMatrix();
		glTranslatef(0,0,1);
		glColor3f(0.2,0.2,0.2);	
		glRectf(-55,55,55,-55);
		glColor3f(1.0,1.0,1.0);
		glBegin(GL_TRIANGLES);
			if(obj_id==1)
			{
				glVertex3f(-40, 40,1);
				glVertex3f(40,40,1);
				glVertex3f(0,-20,1);
			}
			if(obj_id==2)
			{
				glVertex3f(-40, 40,1);
				glVertex3f(-40,-40,1);
				glVertex3f(20,0,1);
			}
			else if(obj_id==3)
			{
				glVertex3f(40, 40,1);
				glVertex3f(40,-40,1);
				glVertex3f(-20,0,1);
			}
			glEnd();
	glPopMatrix();
}

static void drawScore() {
	int l,lTitre,i;

	l = (int)strlen(scoreChar);
	lTitre = (int)strlen(scoreTitre);
	glPushMatrix();
		glLineWidth(3.0);
		glTranslatef((LARGEUR/2.0)+10.0,0.0,50.0);
		glColor3f(0.0/255.0,0.0/255.0,0.0/255.0);
		glPushMatrix();
			glScalef(0.2,0.2,0.2);
			glRotatef(45,1,0,0);
			for(i=0;i<lTitre;i++)
			{
				glutStrokeCharacter(GLUT_STROKE_ROMAN,scoreTitre[i]);
			}
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0.0,-20.0);
			glScalef(0.1,0.1,0.1);
			glRotatef(45,1,0,0);
			for(i=0;i<l;i++)
			{
				glutStrokeCharacter(GLUT_STROKE_ROMAN,scoreChar[i]);
			}
		glPopMatrix();
	glPopMatrix();
}


static void drawMur() {
	glPushMatrix();
		glColor3f(1,1,1);
		glRectf(-50,50,50,-50);

		glColor3f(1,0,0);
		glLineWidth(2.0);
		//limite
		glBegin(GL_LINES);
			glVertex3f(-(LARGEUR/2.0),0,HAUTEUR-(LARGEURCUBE/2));
			glVertex3f(LARGEUR/2.0,0,HAUTEUR-(LARGEURCUBE/2));
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(-(LARGEUR/2.0),-10,HAUTEUR-(LARGEURCUBE/2));
			glVertex3f(LARGEUR/2.0,-10,HAUTEUR-(LARGEURCUBE/2));
		glEnd();

		glColor3f(0.2,0.2,0.2);
		glTranslatef(0,0,1);
		//dessous
		glBegin(GL_QUADS);
			glVertex3f((-LARGEUR/2.0)-5.0,0,0);
			glVertex3f((-LARGEUR/2.0)-5.0,-10,0);
			glVertex3f((LARGEUR/2.0)+5.0,-10,0);
			glVertex3f((LARGEUR/2.0)+5.0,0,0);
		glEnd();
		//cote gauche
		glBegin(GL_QUADS);
			glVertex3f((-LARGEUR/2.0)-5.0,0,0);
			glVertex3f((-LARGEUR/2.0)-5.0,0,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-2.0,0,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-2.0,0,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((-LARGEUR/2.0)-5.0,-10,0);
			glVertex3f((-LARGEUR/2.0)-5.0,-10,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-2.0,-10,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-2.0,-10,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((-LARGEUR/2.0)-5.0,0,0);
			glVertex3f((-LARGEUR/2.0)-5.0,-10,0);
			glVertex3f((-LARGEUR/2.0)-5.0,-10,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-5.0,0,HAUTEUR);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((-LARGEUR/2.0)-2.0,0,0);
			glVertex3f((-LARGEUR/2.0)-2.0,-10,0);
			glVertex3f((-LARGEUR/2.0)-2.0,-10,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-2.0,0,HAUTEUR);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((-LARGEUR/2.0)-2.0,0,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-5.0,0,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-5.0,-10,HAUTEUR);
			glVertex3f((-LARGEUR/2.0)-2.0,-10,HAUTEUR);
		glEnd();

		//Cote droit
		glBegin(GL_QUADS);
			glVertex3f((LARGEUR/2.0)+5.0,0,0);
			glVertex3f((LARGEUR/2.0)+5.0,0,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+2.0,0,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+2.0,0,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((LARGEUR/2.0)+5.0,-10,0);
			glVertex3f((LARGEUR/2.0)+5.0,-10,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+2.0,-10,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+2.0,-10,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((LARGEUR/2.0)+5.0,0,0);
			glVertex3f((LARGEUR/2.0)+5.0,-10,0);
			glVertex3f((LARGEUR/2.0)+5.0,-10,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+5.0,0,HAUTEUR);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((LARGEUR/2.0)+2.0,0,0);
			glVertex3f((LARGEUR/2.0)+2.0,-10,0);
			glVertex3f((LARGEUR/2.0)+2.0,-10,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+2.0,0,HAUTEUR);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f((LARGEUR/2.0)+2.0,0,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+5.0,0,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+5.0,-10,HAUTEUR);
			glVertex3f((LARGEUR/2.0)+2.0,-10,HAUTEUR);
		glEnd();
	glPopMatrix();
}

/* draw the user object */
static int  draw_object( int obj_id, double gl_para[16])
{

    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {0.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {1, 1, 1, 1};
    GLfloat   lightZeroColor[]  = {1, 1, 1, 1};
	int orientation = object[0].trans[2][2];
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd( gl_para );
		

 	/* set the material */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

    //glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	

	//glMaterialfv(GL_FRONT, GL_SPECULAR, red);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, red);

	
	if(obj_id == 0)
	{	
		drawTetris(); 
		drawMur();
		drawScore();
		if(isGameOverVar)
		{
			drawGameOver();
		}
	}
	else if(obj_id != 4)
	{	
		drawFleche(obj_id);
	}

	
	glLoadIdentity();
	//glMaterialfv(GL_FRONT, GL_SPECULAR, red);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, red);

    argDrawMode2D();

    return 0;
}


/* check collision between two markers */
static int checkCollisions( ObjectData_T object0, ObjectData_T object1)
{
	float x1,y1,z1;
	float x2,y2,z2;
	float dist;

	x1 = object0.trans[0][3];
	y1 = object0.trans[1][3];
	z1 = object0.trans[2][3];

	x2 = object1.trans[0][3];
	y2 = object1.trans[1][3];
	z2 = object1.trans[2][3];

	dist = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2);

	//printf("Dist = %3.2f\n",COLLIDE_DIST);

	if(dist < COLLIDE_DIST)
	{
		return 1;
	}
	else 
		return 0;
}
