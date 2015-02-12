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

#define COLLIDE_DIST 30000.0

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
static int tempsChute = 1000;
static Board jeu;





int main(int argc, char **argv)
{
	//initialize applications
	glutInit(&argc, argv);
    init();
	
	arVideoCapStart();

	clock_start=GetTickCount();

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
	glPushMatrix();
		glTranslatef(translateX, 0.0, translateDown);
		glRotatef(rotationPlus,0,0,1.0);
		glutSolidCube(20);
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

	areClose=0;

	if(time_c>tempsChute && translateDown>15)
	{
		translateDown -= 20;
		clock_start=GetTickCount();
		drawTetris();
	}
	

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

	if(object[0].visible && object[1].visible)
	{
		float T[3],dist;
		T[0]=object[0].trans[0][3]-object[1].trans[0][3];
		T[1]=object[0].trans[1][3]-object[1].trans[1][3];
		T[2]=object[0].trans[2][3]-object[1].trans[2][3];
		dist = sqrt(T[0]*T[0]+T[1]*T[1]+T[2]*T[2]);
	
		if(dist<200)
			areClose = 1;
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


	if(object[2].visible == 0 && changement==1)
	{
		translateX+=20;
		changement=0;
	}
	if(object[3].visible == 0 && changement2==1)
	{
		translateX-=20;
		changement2=0;
	}
	if(object[1].visible == 0 && translateDown > 15 && changement3==1)
	{
		tempsChute=100;
		translateDown-=20;
	}
	if(object[1].visible)
	{
		tempsChute=1000;
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
		glColor3f(0.0,1.0,0.0);
		glTranslatef(0,0,10);	
		glRectf(-40,40,40,-40);
		glMaterialfv(GL_FRONT, GL_SPECULAR, green);
		glMaterialfv(GL_FRONT, GL_AMBIENT, green);
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

/* draw the user object */
static int  draw_object( int obj_id, double gl_para[16])
{

    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
	int orientation = object[0].trans[2][2];
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd( gl_para );
		

 	/* set the material */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	

	glMaterialfv(GL_FRONT, GL_SPECULAR, red);
	glMaterialfv(GL_FRONT, GL_AMBIENT, red);

	
	if(obj_id == 0  )
	{	
		drawTetris();
	}
	else
	{	
		drawFleche(obj_id);
	}


	glLoadIdentity();
	glMaterialfv(GL_FRONT, GL_SPECULAR, red);
	glMaterialfv(GL_FRONT, GL_AMBIENT, red);

    argDrawMode2D();

    return 0;
}
