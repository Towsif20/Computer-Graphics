/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */
#include <windows.h>
#ifdef __APPLE__
// #include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define pi (2 * acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double drawpoint = 0;

double camera_x_mover;
double camera_y_mover;
double camera_z_mover;

struct point
{
    double x, y, z;
};

struct point_vector
{
    double x, y, z;
};

struct point pos;
struct point up;
struct point right;
struct point look;

struct point for_that_special_shape;

struct point guns_up;
struct point guns_look;
struct point guns_right;

double barrel_mover_up;
double barrel_rotator;
double grip_mover_right;
double reloader;

double wall = 100;
point bullets[1000];
int bullets_number = 0;

double dotProduct(double vect_A[], double vect_B[])
{
    double product = 0;
    for (int i = 0; i < 3; i++)
        product = product + vect_A[i] * vect_B[i];
    return product;
}

point_vector crossProduct(double vect_A[], double vect_B[])
{
    point_vector cross_P;

    cross_P.x = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    cross_P.y = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
    cross_P.z = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];

    return cross_P;
}

point rotation(point vect, point axes, double rotation_angle)
{
    point after_rotation;

    double cos_value = cos(rotation_angle);
    double one_minus_cos = 1 - cos_value;
    double sin_value = sin(rotation_angle);

    double vect_v[] = {vect.x, vect.y, vect.z};
    double vect_k[] = {axes.x, axes.y, axes.z};

    point_vector k_cross_v = crossProduct(vect_k, vect_v);

    double k_dot_v = dotProduct(vect_k, vect_v);

    after_rotation.x = vect.x * cos_value + k_cross_v.x * sin_value + axes.x * k_dot_v * one_minus_cos;
    after_rotation.y = vect.y * cos_value + k_cross_v.y * sin_value + axes.y * k_dot_v * one_minus_cos;
    after_rotation.z = vect.z * cos_value + k_cross_v.z * sin_value + axes.z * k_dot_v * one_minus_cos;

    return after_rotation;
}

void rotate_about_u(double camAngle)
{
    /*right.x = -(1/sqrt(2));
	right.y = 1/sqrt(2);
	right.z = 0;

	look.x = -(1/sqrt(2));
	look.y = -(1/sqrt(2));
	look.z = 0;*/

    point look_rotated = rotation(look, up, camAngle);
    look.x = look_rotated.x;
    look.y = look_rotated.y;
    look.z = look_rotated.z;

    point right_rotated = rotation(right, up, camAngle);
    right.x = right_rotated.x;
    right.y = right_rotated.y;
    right.z = right_rotated.z;
}

void rotate_about_l(double camAngle)
{
    /*up.x = 0;
	up.y = 0;
	up.z = 1;

	right.x = -(1/sqrt(2));
	right.y = 1/sqrt(2);
	right.z = 0;*/

    point up_rotated = rotation(up, look, camAngle);
    up.x = up_rotated.x;
    up.y = up_rotated.y;
    up.z = up_rotated.z;

    point right_rotated = rotation(right, look, camAngle);
    right.x = right_rotated.x;
    right.y = right_rotated.y;
    right.z = right_rotated.z;
}

void rotate_about_r(double camAngle)
{
    /*up.x = 0;
	up.y = 0;
	up.z = 1;

	look.x = -(1/sqrt(2));
	look.y = -(1/sqrt(2));
	look.z = 0;*/

    point look_rotated = rotation(look, right, camAngle);
    look.x = look_rotated.x;
    look.y = look_rotated.y;
    look.z = look_rotated.z;

    point up_rotated = rotation(up, right, camAngle);
    up.x = up_rotated.x;
    up.y = up_rotated.y;
    up.z = up_rotated.z;
}

void move_forward()
{
    pos.x = pos.x + look.x * 2;
    pos.y = pos.y + look.y * 2;
    pos.z = pos.z + look.z * 2;
}

void move_backwards()
{
    pos.x = pos.x - look.x * 2;
    pos.y = pos.y - look.y * 2;
    pos.z = pos.z - look.z * 2;
}

void move_right()
{
    pos.x = pos.x + right.x * 2;
    pos.y = pos.y + right.y * 2;
    pos.z = pos.z + right.z * 2;
}

void move_left()
{
    pos.x = pos.x - right.x * 2;
    pos.y = pos.y - right.y * 2;
    pos.z = pos.z - right.z * 2;
}

void move_up()
{
    pos.x = pos.x + up.x * 2;
    pos.y = pos.y + up.y * 2;
    pos.z = pos.z + up.z * 2;
}

void move_down()
{
    pos.x = pos.x - up.x * 2;
    pos.y = pos.y - up.y * 2;
    pos.z = pos.z - up.z * 2;
}

void load_the_gun()
{
    guns_up.x = 0;
    guns_up.y = 0;
    guns_up.z = 1;

    guns_look.x = 0;
    guns_look.y = 1;
    guns_look.z = 0;

    guns_right.x = 1;
    guns_right.y = 0;
    guns_right.z = 0;

    struct point first_point;
    struct point second_point;
    struct point third_point;

    double grip_mover_right_in_radian = grip_mover_right * (pi / 180);
    double reloader_in_radian = reloader * (pi / 180);
    double barrel_mover_up_in_radian = barrel_mover_up * (pi / 180);

    point guns_look_rotated_1 = rotation(guns_look, guns_up, grip_mover_right_in_radian);
    point guns_right_rotated_1 = rotation(guns_right, guns_up, grip_mover_right_in_radian);

    //printf("%f" , guns_right_rotated_1.z);

    point guns_look_rotated_2 = rotation(guns_look_rotated_1, guns_right_rotated_1, reloader_in_radian);
    point guns_up_rotated_2 = rotation(guns_up, guns_right_rotated_1, reloader_in_radian);

    first_point.x = guns_look_rotated_2.x * 30;
    first_point.y = guns_look_rotated_2.y * 30;
    first_point.z = guns_look_rotated_2.z * 30;

    point guns_look_rotated_3 = rotation(guns_look_rotated_2, guns_right_rotated_1, barrel_mover_up_in_radian);

    second_point.x = first_point.x + guns_look_rotated_3.x;
    second_point.y = first_point.y + guns_look_rotated_3.y;
    second_point.z = first_point.z + guns_look_rotated_3.z;

    third_point.y = 295;

    double the_ys_ratio = (third_point.y - second_point.y) / (second_point.y - first_point.y);

    third_point.x = second_point.x + (second_point.x - first_point.x) * the_ys_ratio;
    third_point.z = second_point.z + (second_point.z - first_point.z) * the_ys_ratio;

    double abs_of_x = abs(third_point.x);
    double abs_of_z = abs(third_point.z);

    if (abs_of_x < 100 && abs_of_z < 100)
    {
        bullets[bullets_number] = third_point;
        bullets_number++;
    }

    //printf("%f" , third_point.x);
}

void drawPoint()
{
    glPointSize(5);
    glColor3f(1, 0, 0);

    //glVertex3f(0 , 295 , 0);
    int i;
    for (i = 0; i < bullets_number; i++)
    {
        glBegin(GL_POINTS);

        glVertex3f(bullets[i].x, bullets[i].y, bullets[i].z);

        glEnd();
        glFlush();
    }
}

void drawAxes()
{
    if (drawaxes == 1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f(500, 0, 0);
            glVertex3f(-500, 0, 0);

            glVertex3f(0, -500, 0);
            glVertex3f(0, 500, 0);

            glVertex3f(0, 0, 500);
            glVertex3f(0, 0, -500);
        }
        glEnd();
    }
}

void drawGrid()
{
    int i;
    if (drawgrid == 1)
    {
        glColor3f(0.6, 0.6, 0.6); //grey
        glBegin(GL_LINES);
        {
            for (i = -8; i <= 8; i++)
            {

                if (i == 0)
                    continue; //SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i * 10, -90, 0);
                glVertex3f(i * 10, 90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i * 10, 0);
                glVertex3f(90, i * 10, 0);
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
        glVertex3f(a, 300, a);
        glVertex3f(a, 300, -a);
        glVertex3f(-a, 300, -a);
        glVertex3f(-a, 300, a);
    }
    glEnd();
}

void drawCircle(double radius, int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7, 0.7, 0.7);
    //generate points
    for (i = 0; i <= segments; i++)
    {
        points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
        points[i].y = radius * sin(((double)i / (double)segments) * 2 * pi);
    }
    //draw segments using generated points
    for (i = 0; i < segments; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(points[i].x, points[i].y, 0);
            glVertex3f(points[i + 1].x, points[i + 1].y, 0);
        }
        glEnd();
    }
}

void drawCone(double radius, double height, int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for (i = 0; i <= segments; i++)
    {
        points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
        points[i].y = radius * sin(((double)i / (double)segments) * 2 * pi);
    }
    //draw triangles using generated points
    for (i = 0; i < segments; i++)
    {
        //create shading effect
        if (i < segments / 2)
            shade = 2 * (double)i / (double)segments;
        else
            shade = 2 * (1.0 - (double)i / (double)segments);
        glColor3f(shade, shade, shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0, 0, height);
            glVertex3f(points[i].x, points[i].y, 0);
            glVertex3f(points[i + 1].x, points[i + 1].y, 0);
        }
        glEnd();
    }
}

void drawSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (pi / 2));
        r = radius * cos(((double)i / (double)stacks) * (pi / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
            points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
            points[i][j].z = h;
        }
    }
    //draw quads using generated points
    for (i = 0; i < stacks; i++)
    {
        glColor3f((double)i / (double)stacks, (double)i / (double)stacks, (double)i / (double)stacks);
        for (j = 0; j < slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawLeftSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (pi / 2));
        r = radius * cos(((double)i / (double)stacks) * (pi / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
            points[i][j].z = r * sin(((double)j / (double)slices) * 2 * pi);
            points[i][j].y = -h;
        }
    }

    for (i = 0; i < stacks; i++)
    {
        int color = 0;
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for (j = 0; j < slices; j++)
        {
            color = 1 - color;
            glColor3f(color, color, color);
            glBegin(GL_QUADS);
            {

                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawRightSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (pi / 2));
        r = radius * cos(((double)i / (double)stacks) * (pi / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
            points[i][j].z = r * sin(((double)j / (double)slices) * 2 * pi);
            points[i][j].y = h;
        }
    }

    for (i = 0; i < stacks; i++)
    {
        int color = 0;
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for (j = 0; j < slices; j++)
        {
            color = 1 - color;
            glColor3f(color, color, color);
            glBegin(GL_QUADS);
            {

                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawSmallSphere(double radius, int slices, int stacks)
{
    struct point points[100][100];
    int i, j;
    double h, r;
    //generate points
    for (i = 0; i <= stacks; i++)
    {
        h = radius * sin(((double)i / (double)stacks) * (pi / 2));
        r = radius * cos(((double)i / (double)stacks) * (pi / 2));
        for (j = 0; j <= slices; j++)
        {
            points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
            points[i][j].z = r * sin(((double)j / (double)slices) * 2 * pi);
            points[i][j].y = -h + radius;
        }
    }

    for (i = 0; i < stacks; i++)
    {
        int color = 0;
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for (j = 0; j < slices; j++)
        {
            color = 1 - color;
            glColor3f(color, color, color);
            glBegin(GL_QUADS);
            {

                glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
            }
            glEnd();
        }
    }
}

void drawCylinder(double radius, double height, int segments)
{
    int i;
    //double shade;
    struct point points[100];

    for (i = 0; i <= segments; i++)
    {
        points[i].x = radius * cos(((double)i / (double)segments) * 2 * pi);
        points[i].z = radius * sin(((double)i / (double)segments) * 2 * pi);
    }

    for_that_special_shape.x = points[0].x;
    for_that_special_shape.z = points[0].z;
    for_that_special_shape.y = 0;

    int color = 0;
    for (i = 0; i < segments; i++)
    {
        color = 1 - color;
        glColor3f(color, color, color);

        glBegin(GL_QUADS);
        {
            glVertex3f(points[i].x, 0 + radius, points[i].z);
            glVertex3f(points[i + 1].x, 0 + radius, points[i + 1].z);
            glVertex3f(points[i + 1].x, height + radius, points[i + 1].z);
            glVertex3f(points[i].x, height + radius, points[i].z);
        }
        glEnd();
    }
}

void drawOneFourthCircle(double radius, double distance, int segments)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for (i = 0; i <= segments; i++)
    {
        points[i].x = radius * cos(((double)i / (double)segments) * (pi / 2));
        points[i].y = radius * sin(((double)i / (double)segments) * (pi / 2));
    }
    //draw segments using generated points
    for (i = 0; i < segments; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(-points[i].x + radius * 2, points[i].y + distance, 0);
            glVertex3f(-points[i + 1].x + radius * 2, points[i + 1].y + distance, 0);
        }
        glEnd();
    }
}

void drawLastShape(double radius, double distance, int segments)
{
    double circle_x = for_that_special_shape.x * 2;
    double circle_y = for_that_special_shape.y;
    double circle_z = for_that_special_shape.z;

    double rotation_angle = 360 / segments;

    //glTranslatef(circle_x , circle_y , circle_z);
    drawOneFourthCircle(radius, distance, 30);
    //glTranslatef(-50 , circle_y , circle_z);

    int i;
    int color = 1;
    double angle_rotating = 0.2;
    for (i = 0; i < segments; i++)
    {
        color = 1 - color;
        double current_angle = 0;
        glColor3f(color, color, color);
        while (current_angle <= rotation_angle)
        {
            glRotatef(angle_rotating, 0, 1, 0);
            drawOneFourthCircle(radius, distance, 30);

            current_angle += 0.2;
        }
    }
}

void drawSS()
{
    glColor3f(1, 0, 0);
    drawSquare(20);

    glRotatef(angle, 0, 0, 1);
    glTranslatef(110, 0, 0);
    glRotatef(2 * angle, 0, 0, 1);
    glColor3f(0, 1, 0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle, 0, 0, 1);
        glTranslatef(60, 0, 0);
        glRotatef(2 * angle, 0, 0, 1);
        glColor3f(0, 0, 1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3 * angle, 0, 0, 1);
    glTranslatef(40, 0, 0);
    glRotatef(4 * angle, 0, 0, 1);
    glColor3f(1, 1, 0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {

    case '1':
        //drawgrid=1-drawgrid;
        //cameraAngle = cameraAngle += 0.02;
        rotate_about_u(cameraAngle);
        break;

    case '2':
        //cameraAngle = cameraAngle -= 0.02;
        rotate_about_u(-cameraAngle);
        break;

    case '3':
        //cameraAngle = cameraAngle += 0.02;
        rotate_about_r(cameraAngle);
        break;

    case '4':
        //cameraAngle = cameraAngle -= 0.02;
        rotate_about_r(-cameraAngle);
        break;

    case '5':
        //cameraAngle = cameraAngle += 0.02;
        rotate_about_l(cameraAngle);
        break;

    case '6':
        //cameraAngle = cameraAngle -= 0.02;
        rotate_about_l(-cameraAngle);
        break;

    case 'a':
        if (barrel_mover_up != 45)
            barrel_mover_up += 1;
        break;

    case 's':
        if (barrel_mover_up != -45)
            barrel_mover_up -= 1;
        break;

    case 'f':
        if (barrel_rotator != -60)
            barrel_rotator -= 1;
        break;

    case 'd':
        if (barrel_rotator != 60)
            barrel_rotator += 1;
        break;

    case 'q':
        if (grip_mover_right != 45)
            grip_mover_right += 1;
        break;

    case 'w':
        if (grip_mover_right != -45)
            grip_mover_right -= 1;
        break;

    case 'e':
        if (reloader != 45)
            reloader += 1;
        break;

    case 'r':
        if (reloader != -45)
            reloader -= 1;
        break;
    default:
        break;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_DOWN: //down arrow key
                        //cameraHeight -= 3.0;
                        //camera_z_mover += 3;
        //pos.y += 2;
        //pos.x += 2;
        //pos.z += 2;
        move_backwards();
        break;
    case GLUT_KEY_UP: // up arrow key
        //cameraHeight += 3.0;
        //camera_z_mover -= 3;
        //pos.y -= 2;
        //pos.x -= 2;
        //pos.z -= 2;
        move_forward();
        break;

    case GLUT_KEY_RIGHT:
        //cameraAngle += 03;
        //camera_x_mover += 3;
        //pos.x -= 1.5;
        //pos.y += 1.5;
        move_right();
        break;
    case GLUT_KEY_LEFT:
        //cameraAngle -= 03;
        //camera_x_mover -= 3;
        //pos.x += 1.5;
        //pos.y -= 1.5;
        move_left();
        break;

    case GLUT_KEY_PAGE_UP:
        //camera_y_mover += 3;
        //pos.z += 2;
        move_up();
        break;
    case GLUT_KEY_PAGE_DOWN:
        //camera_y_mover -= 3;
        //pos.z -= 2;
        move_down();
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

void mouseListener(int button, int state, int x, int y)
{ //x, y is the x-y of the screen (2D)
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
            //printf("go");
            load_the_gun();
            //drawPoint();
        }
        break;

    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
        { // 2 times?? in ONE click? -- solution is checking DOWN or UP
            drawaxes = 1 - drawaxes;
        }
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
    glClearColor(0, 0, 0, 0); //color black
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
    //gluLookAt(0+camera_x_mover, 0+camera_y_mover , 100+camera_z_mover,	0 + camera_x_mover , 0+camera_y_mover , 0+camera_z_mover ,	0,1,0);
    gluLookAt(pos.x, pos.y, pos.z, pos.x + look.x, pos.y + look.y, pos.z + look.z, up.x, up.y, up.z);

    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);

    /****************************
	/ Add your objects from here
	****************************/
    //add objects

    drawAxes();
    drawGrid();
    drawSquare(wall);

    drawPoint();

    //glColor3f(1,0,0);
    //drawSquare(20);

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

    //drawSphere(30,24,20);

    glRotatef(grip_mover_right, 0, 0, 1);

    drawLeftSphere(30, 60, 20);

    glRotatef(reloader, 1, 0, 0);

    drawRightSphere(30, 60, 20);

    glTranslatef(0, 30, 0);

    glRotatef(barrel_mover_up, 1, 0, 0);
    glRotatef(barrel_rotator, 0, 1, 0);

    drawSmallSphere(10, 60, 20);
    drawCylinder(10, 100, 60);
    drawLastShape(10, 100, 60);

    //glTranslatef(0 , -40 , 0);

    //drawPoint();

    //drawCircle(30 , 60);
    //glTranslatef(20 , 0 , 0);
    //drawOneFourthCircle(10 , 60);
    //glTranslatef(-20 , 0 , 0);
    //glRotatef(45 , 0 , 1 , 0);

    //drawOneFourthCircle(10 , 60);

    //drawCylinder(10 , 100 , 60);
    //drawLeftSphere(30 , 60 , 20);
    //drawCone(20 , 30 , 30);
    //glTranslatef(0 , 40 , 0);

    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}

void animate()
{
    angle += 0.05;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid = 0;
    drawaxes = 1;
    cameraHeight = 150.0;
    cameraAngle = 0.02;
    angle = 0;

    camera_x_mover = 0;
    camera_y_mover = 0;
    camera_z_mover = 0;

    pos.x = 100;
    pos.y = 100;
    pos.z = 0;

    up.x = 0;
    up.y = 0;
    up.z = 1;

    right.x = -(1 / sqrt(2));
    right.y = 1 / sqrt(2);
    right.z = 0;

    look.x = -(1 / sqrt(2));
    look.y = -(1 / sqrt(2));
    look.z = 0;

    barrel_mover_up = 0;
    barrel_rotator = 0;
    grip_mover_right = 0;
    reloader = 0;

    //clear the screen
    glClearColor(0, 0, 0, 0);

    /************************
	/ set-up projection here
	************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80, 1, 1, 1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); //Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST); //enable Depth Testing

    glutDisplayFunc(display); //display callback function
    glutIdleFunc(animate);    //what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop(); //The main loop of OpenGL

    return 0;
}
