#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

#define UP 1
#define RIGHT 2
#define LOOK 3

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double radius = 30;
double cylinder_radius = 10;
int stacks_large = 80;
int slices_large = 80;
double wall_side = 200;
double y_distance = 500;


double rotate_angle_about_x = 0;
double rotate_angle_about_cylinder_axis = 0;
double rotate_angle_about_z = 0;
double cylinder_rotate_angle_about_x = 0;

struct point
{
	double x,y,z;
};

struct point pos;
struct point u;
struct point l;
struct point r;

struct point points_on_wall[1000];
int count;

double dotProduct(struct point A, struct point B)
{
    return A.x*B.x + A.y*B.y + A.z*B.z;
}

point crossProduct(struct point A, struct point B)
{
    struct point product;

    product.x = A.y * B.z - A.z * B.y;
    product.y = A.z * B.x - A.x * B.z;
    product.z = A.x * B.y - A.y * B.x;

    return product;
}


point rotateVector(struct point toBeRotated, struct point axis, double rotationAngle)
{
    struct point rotated;

    double c = cos(rotationAngle);
    double s = sin(rotationAngle);
    struct point cross = crossProduct(axis, toBeRotated);
    double dot = dotProduct(axis, toBeRotated);

    rotated.x = toBeRotated.x*c + cross.x*s + axis.x*dot*(1-c);
    rotated.y = toBeRotated.y*c + cross.y*s + axis.y*dot*(1-c);
    rotated.z = toBeRotated.z*c + cross.z*s + axis.z*dot*(1-c);

    return rotated;
}


void update_vector(int id, struct point new_vector)
{
    if(id == UP)
    {
        u.x = new_vector.x;
        u.y = new_vector.y;
        u.z = new_vector.z;
    }

    if(id == LOOK)
    {
        l.x = new_vector.x;
        l.y = new_vector.y;
        l.z = new_vector.z;
    }

    if(id == RIGHT)
    {
        r.x = new_vector.x;
        r.y = new_vector.y;
        r.z = new_vector.z;
    }
}


void rotate_about_u(double rotationAngle)
{
    struct point rotated_l = rotateVector(l, u, rotationAngle);
    update_vector(LOOK, rotated_l);

    struct point rotated_r = rotateVector(r, u, rotationAngle);
    update_vector(RIGHT, rotated_r);
}

void rotate_about_r(double rotationAngle)
{
    struct point rotated_l = rotateVector(l, r, rotationAngle);
    update_vector(LOOK, rotated_l);

    struct point rotated_u = rotateVector(u, r, rotationAngle);
    update_vector(UP, rotated_u);

}

void rotate_about_l(double rotationAngle)
{
    struct point rotated_u = rotateVector(u, l, rotationAngle);
    update_vector(UP, rotated_u);

    struct point rotated_r = rotateVector(r, l, rotationAngle);
    update_vector(RIGHT, rotated_r);
}


void move_along_l(int direction)
{
    pos.x += direction*l.x;
    pos.y += direction*l.y;
    pos.z += direction*l.z;
}

void move_along_r(int direction)
{
    pos.x += direction*r.x;
    pos.y += direction*r.y;
    pos.z += direction*r.z;
}

void move_along_u(int direction)
{
    pos.x += direction*u.x;
    pos.y += direction*u.y;
    pos.z += direction*u.z;
}

void rotate_object_about_z(int direction)
{
    if(direction == 1)
    {
        if(rotate_angle_about_z < 45)
            rotate_angle_about_z += 1;
    }

    if(direction == -1)
    {
        if(rotate_angle_about_z > -45)
            rotate_angle_about_z -= 1;
    }
}


void rotate_object_about_x(int direction)
{
    if(direction == 1)
    {
        if(rotate_angle_about_x < 45)
            rotate_angle_about_x += 1;
    }

    if(direction == -1)
    {
        if(rotate_angle_about_x > -45)
            rotate_angle_about_x -= 1;
    }
}


void cyliner_rotate_object_about_x(int direction)
{
    if(direction == 1)
    {
        if(cylinder_rotate_angle_about_x < 45)
            cylinder_rotate_angle_about_x += 1;
    }

    if(direction == -1)
    {
        if(cylinder_rotate_angle_about_x > -45)
            cylinder_rotate_angle_about_x -= 1;
    }
}

void rotate_object_about_cylinder_axis(int direction)
{
    if(direction == 1)
    {
        if(rotate_angle_about_cylinder_axis < 45)
            rotate_angle_about_cylinder_axis += 1;
    }

    if(direction == -1)
    {
        if(rotate_angle_about_cylinder_axis > -45)
            rotate_angle_about_cylinder_axis -= 1;
    }
}

void drawWall()
{
    glColor3f(0.8,0.8,0.8);

    glBegin(GL_QUADS);
    {
        glVertex3f( wall_side, y_distance, wall_side);
		glVertex3f( wall_side, y_distance, -wall_side);
		glVertex3f(-wall_side, y_distance, -wall_side);
		glVertex3f(-wall_side, y_distance, wall_side);
    }
    glEnd();
}

void drawPointsOnWall()
{
    glPushMatrix();

    glColor3b(210,105,30);

    glPointSize(6);

    int i;
    for(i = 0 ; i < count ; i++)
    {
        //glPushMatrix();

        if(   points_on_wall[i].x > wall_side
           || points_on_wall[i].z > wall_side
           || points_on_wall[i].x < -wall_side
           || points_on_wall[i].z < -wall_side )
                continue;

        glBegin(GL_POINTS);

        glVertex3f(points_on_wall[i].x , points_on_wall[i].y , points_on_wall[i].z);

        glEnd();

        //glPopMatrix();

    }

    glPopMatrix();

}

void shoot()
{
    point gun_u, gun_l, gun_r;
    point p1, p2, final_point;

    final_point.y = y_distance - radius;

    double angle1 = rotate_angle_about_z * pi / 180;
    double angle2 = rotate_angle_about_x * pi / 180;
    double angle3 = cylinder_rotate_angle_about_x * pi / 180;

    gun_u.x = 0;
    gun_u.y = 0;
    gun_u.z = 1;

    gun_l.x = 0;
    gun_l.y = 1;
    gun_l.z = 0;

    gun_r.x = 1;
    gun_r.y = 0;
    gun_r.z = 0;

    //gun right after rotating the whole thing
    gun_r = rotateVector(gun_r, gun_u, angle1);
    //gun look after rotating the whole thing
    gun_l = rotateVector(gun_l, gun_u, angle1);

    //gun look after rotating with half sphere
    gun_l = rotateVector(gun_l, gun_r, angle2);

    p1.x = gun_l.x * radius;
    p1.y = gun_l.y * radius;
    p1.z = gun_l.z * radius;

    //gun look after rotating only the cylinder
    gun_l = rotateVector(gun_l, gun_r, angle3);

    p2.x = p1.x + gun_l.x;
    p2.y = p1.y + gun_l.y;
    p2.z = p1.z + gun_l.z;

    final_point.x = p2.x  + (p2.x - p1.x) * (final_point.y - p2.y)/(p2.y - p1.y);
    final_point.z = p2.z  + (p2.z - p1.z) * (final_point.y - p2.y)/(p2.y - p1.y);

    points_on_wall[count++] = final_point;

}

void drawAxes()
{
    float axis_length = 500;

	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( axis_length,0,0);
			glVertex3f(-axis_length,0,0);

			glVertex3f(0,-axis_length,0);
			glVertex3f(0, axis_length,0);

			glVertex3f(0,0, axis_length);
			glVertex3f(0,0,-axis_length);
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

void drawSquare(double wall_side)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( wall_side, wall_side,2);
		glVertex3f( wall_side,-wall_side,2);
		glVertex3f(-wall_side,-wall_side,2);
		glVertex3f(-wall_side, wall_side,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
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




void drawLeftHalfSphere(double radius,int slices,int stacks)
{
    double color = 0;

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
			points[i][j].y=h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
		    color = !color;

		    glColor3f(color, color, color);

			glBegin(GL_QUADS);
			{
				glVertex3f(points[i][j].x, -points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x, -points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x, -points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x, -points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}


void drawRightHalfSphere(double radius,int slices,int stacks)
{
    double color = 0;
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
			points[i][j].y=h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
		    color = !color;

		    glColor3f(color, color, color);

			glBegin(GL_QUADS);
			{
				glVertex3f(points[i][j].x, points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x, points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x, points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}


void drawOutsideHalfSphere(double radius,int slices,int stacks)
{
    double color = 0;
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
			points[i][j].y= radius - h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
		    color = !color;

		    glColor3f(color, color, color);

			glBegin(GL_QUADS);
			{
				glVertex3f(points[i][j].x, points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x, points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x, points[i+1][j].y,points[i+1][j].z);

			}glEnd();
		}
	}
}

void drawCylinder(double radius,double height,int segments)
{
    int i;
    double color = 1;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].z=radius*sin(((double)i/(double)segments)*2*pi);
    }

    //draw quads using generated points
    for(i=0;i<segments;i++)
    {
        color = !color;
        glColor3f(color,color,color);

        glBegin(GL_QUADS);
        {
            glVertex3f(points[i].x ,radius, points[i].z);
			glVertex3f(points[i+1].x, radius, points[i+1].z);
			glVertex3f(points[i+1].x, radius + height, points[i+1].z);
			glVertex3f(points[i].x, radius + height, points[i].z);
        }
        glEnd();
    }
}

void drawSingleCurve(double radius, double distance, int segments)
{
    struct point points[100];

    for(int i=0;i<=segments;i++)
    {
        points[i].x = radius*cos(((double)i/(double)segments)*(pi/2));
        points[i].y = radius*sin(((double)i/(double)segments)*(pi/2));
    }

    for(int i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(-points[i].x + radius*2 ,points[i].y + distance, 0);
			glVertex3f(-points[i+1].x + radius*2, points[i+1].y + distance, 0);
        }
        glEnd();
    }
}


void drawCurvature(double radius, double distance, int segments)
{
    double color = 0;

    drawSingleCurve(radius, distance, 30);

    double angle_per_block = 360/segments;

    double angle_rotate = 0.2;

    for(int i=0;i<segments;i++)
    {
        color = !color;

        double current = 0;

        glColor3f(color, color, color);

        for(current=0; current<=angle_per_block; current += 0.2)
        {
            glRotatef(angle_rotate, 0, 1, 0);
            drawSingleCurve(radius, distance, 30);
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


    glRotatef(rotate_angle_about_z, 0, 0, 1);

    drawLeftHalfSphere(radius, slices_large, stacks_large);

    glRotatef(rotate_angle_about_x, 1, 0, 0);

	drawRightHalfSphere(radius, slices_large, stacks_large);

    glTranslatef(0, radius, 0);

    glRotatef(cylinder_rotate_angle_about_x, 1, 0, 0);
    glRotatef(rotate_angle_about_cylinder_axis, 0, 1, 0);

    drawOutsideHalfSphere(cylinder_radius, 50, 40);

    double height = 100;
    int segments = 60;

    drawCylinder(cylinder_radius, height, segments);
    drawCurvature(cylinder_radius, cylinder_radius + height, segments);

    //drawPointsOnWall();
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			rotate_about_u(cameraAngle);
			break;

        case '2':
			rotate_about_u(-cameraAngle);
			break;

        case '3':
			rotate_about_r(cameraAngle);
			break;

        case '4':
			rotate_about_r(-cameraAngle);
			break;


        case '5':
			rotate_about_l(cameraAngle);
			break;

        case '6':
			rotate_about_l(-cameraAngle);
			break;

        case 'q':
            rotate_object_about_z(1);

            break;

        case 'w':
            rotate_object_about_z(-1);
            break;

        case 'e':
            rotate_object_about_x(1);
            break;

        case 'r':
            rotate_object_about_x(-1);
            break;

        case 'a':
            cyliner_rotate_object_about_x(1);
            break;

        case 's':
            cyliner_rotate_object_about_x(-1);
            break;

        case 'd':
            rotate_object_about_cylinder_axis(1);
            break;

        case 'f':
            rotate_object_about_cylinder_axis(-1);
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 3.0;
			move_along_l(-1);
			break;

		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			move_along_l(1);
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			move_along_r(1);
			break;

		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;
			move_along_r(-1);
			break;

		case GLUT_KEY_PAGE_UP:
		    move_along_u(1);
			break;

		case GLUT_KEY_PAGE_DOWN:
		    move_along_u(-1);
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:

			if(state == GLUT_DOWN)
            {
                // 2 times?? in ONE click? -- solution is checking DOWN or UP
                shoot();

			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			if(state == GLUT_DOWN)
            {
                drawaxes = !drawaxes;
            }
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
	gluLookAt(pos.x, pos.y, pos.z,  	pos.x + l.x, pos.y + l.y, pos.z + l.z,      u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();
	drawWall();
	drawPointsOnWall();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);





	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle = 0.02;
	angle=0;

	pos.x = 100;
	pos.y = 100;
	pos.z = 0;

	u.x = 0;
	u.y = 0;
	u.z = 1;

	l.x = -1/(sqrt(2));
	l.y = -1/(sqrt(2));
	l.z = 0;

	r.x = -1/(sqrt(2));
	r.y = 1/(sqrt(2));
    r.z = 0;

    count = 0;

    rotate_angle_about_x = 0;
    rotate_angle_about_cylinder_axis = 0;
    rotate_angle_about_z = 0;

    cylinder_rotate_angle_about_x = 0;

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
