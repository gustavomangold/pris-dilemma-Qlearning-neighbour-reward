#include <GL/glut.h>

#define WINDOW_SIZE_X  1700
#define WINDOW_SIZE_Y  800
#define SIM_SIZE       700
#define SITE_SIZE      6
#define BORDER         1

#define TIME_STRINGX  (0.80)
#define TIME_STRINGY  (0.95)
#define TIC_SIZE	   5

#define MEASURES      1000

#define Line(x1,y1,x2,y2)  glBegin(GL_LINES);  \
   glVertex2f ((x1),(y1)); glVertex2f ((x2),(y2)); glEnd();

void simulation(void);

void myinit(void) 
{
  //glClearColor(1.0, 1.0, 1.0, 1.0); //Sets background to white
  glClearColor(0.0, 0.0, 0.0, 0.0); //Sets background to black
  //glColor3f(1.0, 0.0, 0.0); // Red
  glEnable( GL_POINT_SMOOTH );
  glEnable (GL_LINE_STIPPLE);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0,(GLdouble) WINDOW_SIZE_X, 0.0, (GLdouble) WINDOW_SIZE_Y);
  glMatrixMode(GL_MODELVIEW);
}

void initialize_glut_display(void (*func)(void), int *argc, char **argv, char *window_title)
{
	glutInit(argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutInitWindowPosition (1300, 0);
	glutCreateWindow(window_title);
	//printf("Passing function simulation to GLUT\n");fflush(stdout);
	//printf("Done Create Window\n"); fflush(stdout);
	glutDisplayFunc (simulation);
	//printf("Done simulation\n"); fflush(stdout);
	//printf("Calling myInit()\n");fflush(stdout);
	myinit();
	//printf("Calling glutMainLoop()\n");fflush(stdout);
	glutMainLoop();
}


void view2d(int LSIZE,int *s, int numsteps, int TOTALSTEPS, long *t, unsigned long num_c, unsigned long num_d,unsigned long NUM_DEFECTS)
{
  char title[50];
  int x,y,i,j;
  static unsigned long int nc[MEASURES],nd[MEASURES];
  long int L2=LSIZE*LSIZE;
  
  int OFFSETY = (int)(0.9*(WINDOW_SIZE_Y-SIM_SIZE));
  int OFFSETX = (int)(0.1*(WINDOW_SIZE_Y-SIM_SIZE));
  
  int SIM_ENDX = OFFSETX + SIM_SIZE;
  int SIM_ENDY = OFFSETY + SIM_SIZE;
  
  int GRAPHX   = (int)(0.9*(WINDOW_SIZE_X - SIM_SIZE));
  int GRAPHY   = SIM_ENDY/2;
	
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0); //White
  Line(OFFSETX - 1, SIM_SIZE + OFFSETY +1, SIM_SIZE + OFFSETX +1 ,SIM_SIZE + OFFSETY+1);
  Line(SIM_SIZE + OFFSETX +1  , OFFSETY -1, SIM_SIZE + OFFSETX +1  , SIM_SIZE + OFFSETY+1);
  Line(OFFSETX  -1            , OFFSETY -1, OFFSETX  -1            , SIM_SIZE+OFFSETY +1 );
  Line(OFFSETX  -1            , OFFSETY -1, SIM_SIZE + OFFSETX +1  , OFFSETY -1);
//  glBegin(GL_LINES);  
//  glVertex2d(OFFSETX - 1            , );             
//  glVertex2d( , S);              
  //glVertex2d(SIM_SIZE + OFFSETX +1  , OFFSETY -1          );
  //glVertex2d(SIM_SIZE + OFFSETX +1  , SIM_SIZE + OFFSETY+1);             
  //glVertex2d(OFFSETX  -1            , OFFSETY -1          );
  //glVertex2d(OFFSETX  -1            , SIM_SIZE+OFFSETY +1 );  
 // glVertex2d(OFFSETX  -1            , OFFSETY -1        );
 // glVertex2d(SIM_SIZE + OFFSETX +1  , OFFSETY -1        ); 
 // glEnd();  
  
  
  int RECX = SIM_ENDX + 3*OFFSETX;
  int RECY = SIM_ENDY/2;
  
  glColor3f(1.0, 1.0, 1.0); //White
  glBegin(GL_QUADS);
  glVertex3f( RECX         , RECY+GRAPHY, 0.);
  glVertex3f( RECX         , RECY       , 0.);
  glVertex3f( RECX + GRAPHX, RECY       , 0.);
  glVertex3f( RECX + GRAPHX, RECY+GRAPHY, 0.);
  glEnd();
  
  int OX = RECX + 6*OFFSETX; //Graph ORIGIN
  int OY = RECY + 3*OFFSETX;
  //int OX1 = OX+1;
 // int OY1 = OY+1;
  int GRLX = (int)(0.9*GRAPHX);//Graph Length
  int GRLY = (int)(0.9*GRAPHY);
  glColor3f(0.0, 0.0, 0.0); //Black
  
  glLineWidth (2.0);
  Line(OX,OY,OX,OY+GRLY);
  Line(OX,OY,OX+GRLX,OY);
/*  glBegin(GL_LINES);
  glVertex2d(OX     ,OY);             
  glVertex2d(OX     ,OY+GRLY);              
  glVertex2d(OX     ,OY);
  glVertex2d(OX+GRLX,OY);
  */  
  
  for (i=0;i<=10;++i)
	{
		Line(OX,OY + i*GRLY*1.0/10.0,OX+TIC_SIZE,OY + i*GRLY*1.0/10.0);
//		glVertex2d(OX         ,OY + i*GRLY*1.0/10.0);
	//	glVertex2d(OX+TIC_SIZE,OY + i*GRLY*1.0/10.0);
		
		glLineStipple (1, 0x0101);  /*  dotted  */
	    Line(OX,OY + i*GRLY*1.0/10.0,OX+GRLX,OY + i*GRLY*1.0/10.0);
	    glLineStipple (1,65535); /* continuous - 2^16-1*/
		sprintf(title,"%.1f",i*1.0/10.0);
		glRasterPos2i( (GLint) OX - 3*OFFSETX, (GLint) OY + i*GRLY*1.0/10.0 - 6);
		int char_pos = 0;
		char cc = title[char_pos];
//	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, title[0]);
    //GLUT_BITMAP_HELVETICA_12;GLUT_BITMAP_8_BY_13
		while (cc!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cc);
			++char_pos;
			cc=title[char_pos];
		}		
	
	}	
	
	sprintf(title,"f(t)");
    glRasterPos2i( (GLint) OX - 5.8*OFFSETX, (GLint) OY + 5*GRLY*1.0/10.0 - 6);
	int char_pos = 0;
	char cc = title[char_pos];
	while (cc!=0)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cc);
		++char_pos;
		cc=title[char_pos];
	}		
		
	int XLABEL=1;
	int n=0;		
	while(XLABEL<=TOTALSTEPS)
	{
		int x0;
		//frexp(XLABEL, &n);
		x0 = OX + log10(XLABEL)*GRLX*1.0/log10(TOTALSTEPS);
		glLineWidth (2.0);
		Line(x0, OY, x0, OY + TIC_SIZE );
		glLineWidth (1.0);
		glLineStipple (1, 0x0101);  /*  dotted  */
	    Line(x0, OY,x0, OY+GRLY );
	    glLineStipple (1,0xFFFF); /* continuous - 2^16-1 = 65535*/
		
		sprintf(title,"%d",n);
		glRasterPos2i( (GLint)( OX + log10(XLABEL)*GRLX*1.0/log10(TOTALSTEPS))+OFFSETX, (GLint) OY-1.2*OFFSETX);
		int char_pos = 0;
		char cc = title[char_pos];
//	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, title[0]);
    //GLUT_BITMAP_HELVETICA_12;GLUT_BITMAP_8_BY_13
		while (cc!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, cc);
			++char_pos;
			cc=title[char_pos];
		}		
		
		sprintf(title,"%d",10);
		glRasterPos2i( (GLint)( OX + log10(XLABEL)*GRLX*1.0/log10(TOTALSTEPS))-10, (GLint) OY-2*OFFSETX);
		char_pos = 0;
		cc = title[char_pos];
//	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, title[0]);
    //GLUT_BITMAP_HELVETICA_12;GLUT_BITMAP_8_BY_13
		while (cc!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cc);
			++char_pos;
			cc=title[char_pos];
		}		
		
		
		XLABEL*=10;
		n++;
	}	
	
	sprintf(title,"t");
		glRasterPos2i( (GLint)( OX + log10(XLABEL/8)*GRLX*1.0/log10(TOTALSTEPS))+OFFSETX, (GLint) OY-2.5*OFFSETX);
		char_pos = 0;
		cc = title[char_pos];
//	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, title[0]);
    //GLUT_BITMAP_HELVETICA_12;GLUT_BITMAP_8_BY_13
		while (cc!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cc);
			++char_pos;
			cc=title[char_pos];
		}		
	
	
	glLineWidth (1.0);		
	
    
			
			
	
	//glDisable (GL_LINE_STIPPLE);    
  int xgr;
  int ygr;
  glPointSize(4.0);
  for (i=0;i<MEASURES;++i)
  {
	   if (t[i] <= numsteps)
		{	
			if (t[i] == numsteps) 
			{
				nc[i] = num_c;
				nd[i] = num_d;
				//printf("%5ld %5ld %5ld\n",nc[i],nd[i],nl[i]);
			}
			
			xgr = OX + (int)(log10(t[i])*GRLX*1.0/log10(TOTALSTEPS));

			glColor3f(1.0, 0.0, 0.0);//Red
			//ygr = OY1 + (int)(nd[i]*(GRLY-1)*1.0/(L2-NUM_DEFECTS));
			ygr = OY + (int)(nd[i]*GRLY*1.0/(L2-NUM_DEFECTS)); 					
			glBegin(GL_POINTS);
			glVertex2i(xgr,ygr);
			glEnd();
			
			glColor3f(0.0, 0.0, 1.0);//Blue
			ygr = OY + (int)(nc[i]*GRLY*1.0/(L2-NUM_DEFECTS)); 
					
			glBegin(GL_POINTS);
			glVertex2i(xgr,ygr);
			glEnd();
			
			/*glColor3f(0.0, 1.0, 0.0);//Green
			
			ygr = OY + (int)(nl[i]*GRLY*1.0/(L2-NUM_DEFECTS)); 
					
			glBegin(GL_POINTS);
			glVertex2i(xgr,ygr);
			glEnd();*/
			
		}
		else break;
  }   
     
	for(j=0;j<LSIZE;++j)
		for(i=0;i<LSIZE;++i)
		{
			switch (s[j*LSIZE+i])
			{
				case  0: glColor3f(0.0, 0.0, 0.0); // Black
					break;
				case  1: glColor3f(0.0, 0.0, 1.0); // Blue
					break;
				case -1: glColor3f(1.0, 0.0, 0.0); // Red
					break;
				case  2: glColor3f(0.0, 1.0, 0.0); // Green
					break;
				case  3: glColor3f(0.5, 0.0, 0.0); // Dark Red
					break;
				case  4: glColor3f(0.0, 0.0, 0.5); // Dark Blue
					break;
				case  5: glColor3f(0.0, 0.5, 0.0); // Dark Green
					break;
			}
			x = i*(SITE_SIZE+BORDER) + OFFSETX;
			y = j*(SITE_SIZE+BORDER) + OFFSETY;
			glBegin(GL_QUADS);
			glVertex3f( x          , y          , 0.);
			glVertex3f( x+SITE_SIZE, y          , 0.);
			glVertex3f( x+SITE_SIZE, y+SITE_SIZE, 0.);
			glVertex3f( x          , y+SITE_SIZE, 0.);
			glEnd();
		}
	
	 //glutBitmapCharacter(void *font, int character);
	glColor3f(1.0, 1.0, 1.0); //White
    sprintf(title,"t = %6d",numsteps);
	glRasterPos2i( (GLint) (TIME_STRINGX*SIM_SIZE)+OFFSETX, (GLint) (TIME_STRINGY*SIM_SIZE)+OFFSETY);
	
	char_pos = 0;
	cc = title[char_pos];
//	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, title[0]);
    //GLUT_BITMAP_HELVETICA_12;GLUT_BITMAP_8_BY_13
	while (cc!=0)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cc);
		++char_pos;
		cc=title[char_pos];
	}
	
	glFlush();
     
 }
      /*for (j=0;j<WINDOW_SIZE/(SITE_SIZE+BORDER);++j)
	for (i = 0; i < WINDOW_SIZE/(SITE_SIZE+BORDER); i++) 
	  {
	    if (gsl_rng_uniform(r) < 0.5) glColor3f(0.0, 0.0, 1.0);
	    else  glColor3f(1.0, 0.0, 0.0);
	    
	    x = i*(SITE_SIZE+BORDER);
	    y = j*(SITE_SIZE+BORDER);
	    
	    glBegin(GL_QUADS);
	    glVertex3f( x          , y          , 0.);
	    glVertex3f( x+SITE_SIZE, y          , 0.);
	    glVertex3f( x+SITE_SIZE, y+SITE_SIZE, 0.);
	    glVertex3f( x          , y+SITE_SIZE, 0.);
	    glEnd();
	  }
      
      glFlush();
    
}*/



