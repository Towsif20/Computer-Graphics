#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;

struct point
{
	double x,y,z;
};


double side = 130;
double radius = 80;
double bubble_radius = 15;
double bubble_diameter = 2 * bubble_radius;



double direction_x[5];
double direction_y[5];

struct point position[5];

double absolute_velocity;
double angles[5];
double velocity_x[5];
double velocity_y[5];

bool move_flag;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}





void drawBubbles()
{

    for(int i=0;i<5;i++)
    {
        glPushMatrix();

        glTranslatef(position[i].x, position[i].y, position[i].z);
        drawCircle(bubble_radius, 50);

        //glTranslatef(bubble_x, bubble_y, 0);

        glPopMatrix();
    }
}


void drawBox()
{
    glColor3f(0, 1, 0);

    glBegin(GL_LINES);

    glVertex3f(-side,-side,0);
    glVertex3f(-side,side,0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(-side,side,0);
    glVertex3f(side,side,0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(side,side,0);
    glVertex3f(side,-side,0);

    glEnd();

    glBegin(GL_LINES);

    glVertex3f(side,-side,0);
    glVertex3f(-side,-side,0);

    glEnd();
}

void updateVelocities()
{
    for(int i=0;i<5;i++)
    {
        velocity_x[i] = absolute_velocity * cos(angles[i]);
        velocity_y[i] = absolute_velocity * sin(angles[i]);
    }
}

void updateBubbles()
{
    updateVelocities();

    double len = side * 2;

    for(int i=0;i<5;i++)
    {
        //update x
        if(position[i].x + bubble_diameter >= len || position[i].x <= 0)
        {
            direction_x[i] = -direction_x[i];
            position[i].x += direction_x[i] * velocity_x[i];
        }

        if(position[i].x + bubble_diameter < len  || position[i].x > 0)
            position[i].x += direction_x[i] * velocity_x[i];


        //update y
        if(position[i].y + bubble_diameter >= len || position[i].y <= 0)
        {
            direction_y[i] = -direction_y[i];
            position[i].y += direction_y[i] * velocity_y[i];
        }

        if(position[i].y + bubble_diameter < len  || position[i].y > 0)
            position[i].y += direction_y[i] * velocity_y[i];
    }

}


void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawSS()
{
//    glColor3f(1,0,0);
//    drawSquare(20);
//
//    glRotatef(angle,0,0,1);
//    glTranslatef(110,0,0);
//    glRotatef(2*angle,0,0,1);
//    glColor3f(0,1,0);
//    drawSquare(15);
//
//    glPushMatrix();
//    {
//        glRotatef(angle,0,0,1);
//        glTranslatef(60,0,0);
//        glRotatef(2*angle,0,0,1);
//        glColor3f(0,0,1);
//        drawSquare(10);
//    }
//    glPopMatrix();
//
//    glRotatef(3*angle,0,0,1);
//    glTranslatef(40,0,0);
//    glRotatef(4*angle,0,0,1);
//    glColor3f(1,1,0);
//    drawSquare(5);


    glColor3f(1, 0, 0);
    drawCircle(radius, 80);

    drawBox();

    glColor3f(1,1,1);


    glTranslatef(-side+bubble_radius, -side+bubble_radius, 0);


    drawBubbles();
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'p':
			move_flag = !move_flag;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:

		    //down arrow key
			if(absolute_velocity > 1)
                absolute_velocity -= 0.1;

			break;

		case GLUT_KEY_UP:

		    // up arrow key
			if(absolute_velocity < 3)
                absolute_velocity += 0.1;

			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}





void animate()
{
    if(move_flag)
        updateBubbles();

	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;

	side = 130;
	radius = 80;
	bubble_radius = 15;
	bubble_diameter = 2*bubble_radius;
	absolute_velocity = 2;
	move_flag = true;

	srand(time(0));

	for(int i=0;i<5;i++)
    {
        position[i].x = 0;
        position[i].y = 0;
        position[i].z = 0;

        direction_x[i] = -1;
        direction_y[i] = -1;

        angles[i] = pi/180 * (rand() % 89 + 1);
        printf("%f\n", angles[i]);

    }

    //updateVelocities();
	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
