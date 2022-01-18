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
struct point center;
bool moving[5];
bool overlapping[5][5];
int tracker;
int bubble_count;



double direction_x[5];
double direction_y[5];

struct point position[5];

double absolute_velocity;
double angles[5];
struct point velocity[5];
bool inside[5];
int count[5];

bool move_flag;

double vector_value(struct point v)
{
    return sqrt(pow(v.x,2) + pow(v.y, 2) + pow(v.z, 2));
}

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


point normalize(struct point p)
{
    double val = vector_value(p);

    struct point result;
    result.x = p.x/val;
    result.y = p.y/val;
    result.z = p.z/val;

    return result;
}

point a_to_b(struct point a, struct point b)
{
    struct point result;

    result.x = b.x - a.x;
    result.y = b.y - a.y;
    result.z = b.z - a.z;

    return result;
}

void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);

            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);

            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }
    glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0; i<segments; i++)
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

    for(int i=0; i<5; i++)
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


void initVelocities(int i)
{
    velocity[i].x = -absolute_velocity * cos(angles[i]);
    velocity[i].y = -absolute_velocity * sin(angles[i]);
    velocity[i].z = 0;

    if(i < 5)
        printf("%f, %f\n", -velocity[i].x, -velocity[i].y);

    moving[i] = true;
}

void updateVelocities(double f)
{
    for(int i=0; i<5; i++)
    {
        velocity[i].x *= f;
        velocity[i].y *= f;

    }

    absolute_velocity = vector_value(velocity[0]);
}

void updateInside()
{
    for(int i=0; i<5; i++)
    {
        if(!moving[i])
            return;

        struct point n = a_to_b(position[i], center);

        if(bubble_radius + vector_value(n) <= radius)
        {
            inside[i] = true;
        }
    }
}

void reflectWithCircle()
{
    for(int i=0; i<5; i++)
    {
        if(!moving[i])
            return;

        struct point n = a_to_b(center, position[i]);

        if(bubble_radius + vector_value(n) >= radius)
        {

            if(inside[i])
            {
                n = normalize(n);

                double dot = dotProduct(velocity[i], n);

                struct point vect;

                vect.x = 2*dot*n.x;
                vect.y = 2*dot*n.y;
                vect.z = 0;
                struct point r = a_to_b(vect, velocity[i]);

                velocity[i].x = r.x;
                velocity[i].y = r.y;
                velocity[i].z = 0;
            }
        }


    }
}

void updateOverlapping()
{
    if(tracker < 11)
        return;

    for(int i=0; i<4; i++)
    {
        for(int j=i+1; j<5; j++)
        {
            if(!moving[i] || !moving[j])
                continue;

            if(inside[i] && inside[j])
            {
                struct point n = a_to_b(position[j], position[i]);

                if(vector_value(n) > bubble_diameter)
                    overlapping[i][j] = false;
            }
        }
    }
}

void reflectWithBubbles()
{
    if(tracker < 11)
        return;

    for(int i=0; i<4; i++)
    {
        for(int j=i+1; j<5; j++)
        {
            if(!moving[i] || !moving[j])
                continue;

            struct point n = a_to_b(position[j], position[i]);

            if(bubble_diameter >= vector_value(n))
            {
                if(inside[i] && inside[j] && !overlapping[i][j])
                {
                    n = normalize(n);

                    double dot = dotProduct(velocity[i], n);

                    struct point vect;

                    vect.x = 2*dot*n.x;
                    vect.y = 2*dot*n.y;
                    vect.z = 0;
                    struct point r = a_to_b(vect, velocity[i]);

                    velocity[i].x = r.x;
                    velocity[i].y = r.y;
                    velocity[i].z = 0;

                    n.x = -n.x;
                    n.y = -n.y;
                    n.z = 0;

                    dot = dotProduct(velocity[j], n);

                    vect.x = 2*dot*n.x;
                    vect.y = 2*dot*n.y;
                    vect.z = 0;

                    r = a_to_b(vect, velocity[j]);

                    velocity[j].x = r.x;
                    velocity[j].y = r.y;
                    velocity[j].z = 0;

                }
            }


        }
    }
}

void updateBubbles()
{
    //updateVelocities();

    double len = side * 2;

    for(int i=0; i<5; i++)
    {
        if(!moving[i])
            return;

        //update x
        if(position[i].x + bubble_diameter >= len || position[i].x <= 0)
        {
            velocity[i].x = -velocity[i].x;

        }

        //update y
        if(position[i].y + bubble_diameter >= len || position[i].y <= 0)
        {
            velocity[i].y = -velocity[i].y;
        }

        position[i].x += velocity[i].x;
        position[i].y += velocity[i].y;

    }

}


void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0; i<segments; i++)
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
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
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
            }
            glEnd();
        }
    }
}


void drawSS()
{
    glColor3f(1, 0, 0);
    drawCircle(radius, 80);

    drawBox();

    glColor3f(1,1,1);


    glTranslatef(-side+bubble_radius, -side+bubble_radius, 0);
    center.x = side - bubble_radius;
    center.y = side - bubble_radius;
    center.z = 0;

    drawBubbles();
}

void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case 'p':
        move_flag = !move_flag;
        break;

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:

        //down arrow key

        if(absolute_velocity > 1)
            updateVelocities(0.91);

        break;

    case GLUT_KEY_UP:

        // up arrow key

        if(absolute_velocity < 4)
            updateVelocities(1.1);

        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
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



void display()
{

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
    if(tracker %10 == 0)
    {
        tracker = 0;

        initVelocities(bubble_count);

        bubble_count++;

    }

    if(bubble_count <= 5)
        tracker++;

    else
        tracker = 11;

    if(move_flag)
        updateBubbles();

    updateInside();
    updateOverlapping();

    reflectWithCircle();

    //updateOverlapping();
    reflectWithBubbles();


    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
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
    center.x = 0;
    center.y = 0;
    center.z = 0;

    move_flag = true;

    srand(time(0));

    for(int i=0; i<5; i++)
    {
        position[i].x = 0;
        position[i].y = 0;
        position[i].z = 0;

        inside[i] = false;
        count[i] = 0;
        moving[i] = false;



        angles[i] = pi/180 * (rand() % 89 + 1);
        //printf("%f\n", angles[i]);

    }

    for(int i=0; i<4; i++)
    {
        for(int j=i+1; j<5; j++)
        {
            overlapping[i][j] = true;
        }
    }

    bubble_count = 0;
    tracker = 0;

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

int main(int argc, char **argv)
{
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
