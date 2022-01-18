#include<iostream>
#include<stdlib.h>
#include<math.h>
#include <bits/stdc++.h>
#include<string.h>
#include<list>
#include<fstream>
#include "bitmap_image.hpp"

using namespace std;

ofstream output;
string input_folder = "4";

#define PI (2*acos(0.0))

class point
{
    public:

    double x;
    double y;
    double z;
    double w;

    point()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 1;
    }

    point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    double absolute_value()
    {
        return sqrt(x*x + y*y + z*z);
    }

    void normalize()
    {
        double val = this->absolute_value();
        
        x /= val;
        y /= val;
        z /= val;
    }

    void scale_w_to_1()
    {
        x /= w;
        y /= w;
        z /= w;

        w = 1;
    }

    point add(point p)
    {
        point sum;

        sum.x = this->x + p.x;
        sum.y = this->y + p.y;
        sum.z = this->z + p.z;

        return sum;
    }

    point sub(point p)
    {
        point sum;

        sum.x = this->x - p.x;
        sum.y = this->y - p.y;
        sum.z = this->z - p.z;

        return sum;
    }

    double dotProduct(point p)
    {
        return  this->x * p.x 
            +   this->y * p.y
            +   this->z * p.z;
    }


    point crossProduct(point p)
    {
        point product;

        product.x = this->y * p.z - this->z * p.y;
        product.y = this->z * p.x - this->x * p.z;
        product.z = this->x * p.y - this->y * p.x;

        return product;
    }

    point rotate(point axis, double angle)
    {
        point rotated;

        point toBeRotated;

        toBeRotated.x = this->x;
        toBeRotated.y = this->y;
        toBeRotated.z = this->z;
        toBeRotated.w = this->w;

        double c = cos(angle);

        if(c < 0.0001)
            c = 0;

        double s = sin(angle);
        
        point cross = axis.crossProduct(toBeRotated);
        double dot = axis.dotProduct(toBeRotated);

        rotated.x = toBeRotated.x*c + cross.x*s + axis.x*dot*(1-c);
        rotated.y = toBeRotated.y*c + cross.y*s + axis.y*dot*(1-c);
        rotated.z = toBeRotated.z*c + cross.z*s + axis.z*dot*(1-c);

        return rotated;
    }

    void print()
    {
        cout<<x<<" "<<y<<" "<<z<<endl;
        // cout<<x<<" "<<y<<" "<<z<<" "<<w<<endl;
        // cout<<"x = "<<x<<endl;
        // cout<<"y = "<<y<<endl;
        // cout<<"z = "<<z<<endl;
        // cout<<"w = "<<w<<endl;
    }

    void printToFile()
    {
        output<<x<<" "<<y<<" "<<z<<endl;
    }
};

class color
{
    public:

    int rgb[3];

    void setRandomColor()
    {       
        for(int i=0;i<3;i++)
        {
            rgb[i] = rand()%256;
        }
    }
};

double max_of_three(double a, double b, double c)
{
    double ans = a;

    if(ans < b)
        ans = b;

    if(ans < c)
        ans = c;

    return ans;
}

double min_of_three(double a, double b, double c)
{
    double ans = a;

    if(ans > b)
        ans = b;

    if(ans > c)
        ans = c;

    return ans;
}

class triangle
{
    public:

    point points[3];
    color clr;

    void setRandomColor()
    {
        clr.setRandomColor();
    }

    double max_X()
    {
        return max_of_three(points[0].x, points[1].x, points[2].x);
    }

    double min_X()
    {
        return min_of_three(points[0].x, points[1].x, points[2].x);
    }

    double max_Y()
    {
        return max_of_three(points[0].y, points[1].y, points[2].y);
    }


    double min_Y()
    {
        return min_of_three(points[0].y, points[1].y, points[2].y);
    }

    double solveX_with_scanline(int firstIndex, int secondIndex, double ys)
    {
        double x1 = points[firstIndex].x;
        double y1 = points[firstIndex].y;

        double x2 = points[secondIndex].x;
        double y2 = points[secondIndex].y;

        if(y1 == y2)
            return this->max_X() + 10;

        double ans = x1 + ((ys-y1) / (y1-y2)) * (x1-x2);

        return ans;
    }

    double solveZ_with_scanline(int firstIndex, int secondIndex, double ys)
    {
        double z1 = points[firstIndex].z;
        double y1 = points[firstIndex].y;

        double z2 = points[secondIndex].z;
        double y2 = points[secondIndex].y;

        if(y1 == y2)
            return 0;

        double ans = z1 + ((ys-y1) / (y1-y2)) * (z1-z2);

        return ans;
    }


    void print()
    {
        for(int i=0;i<3;i++)
        {
            points[i].print();
        }
    }

    void printToFile()
    {
        for(int i=0;i<3;i++)
        {
            points[i].printToFile();
        }

        output<<endl;
    }
};

class matrix
{
    public:

    double values[4][4];

    matrix()
    {
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                values[i][j] = 0;
            }
        }

        values[3][3] = 1;
    }

    void setValues(double square[4][4])
    {
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                values[i][j] = square[i][j];
            }
        }
    }

    void identity()
    {
        for(int i=0;i<4;i++)
        {
            this->values[i][i] = 1;
        }
    }

    void translationMatrix(double tx, double ty, double tz)
    {
        identity();

        values[0][3] = tx;
        values[1][3] = ty;
        values[2][3] = tz;
    }

    void scalingMatrix(double sx, double sy, double sz)
    {
        this->values[0][0] = sx;
        this->values[1][1] = sy;
        this->values[2][2] = sz;
    }

    void rotationMatrix(point c1, point c2, point c3)
    {
        double array[4][4] = 
                            {
                                {c1.x, c2.x, c3.x, 0},
                                {c1.y, c2.y, c3.y, 0},
                                {c1.z, c2.z, c3.z, 0},
                                {0, 0, 0, 1},
                            };

        
        this->setValues(array);
    }

    matrix multiply(matrix second)
    {
        matrix product;
        double v[4][4];

        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                v[i][j] = 0;
            }
        }

        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                for(int k=0;k<4;k++)
                {
                    v[i][j] += this->values[i][k] * second.values[k][j];
                }
            }
        }

        product.setValues(v);
        return product;
    }


    point multiply(point p)
    {
        point product;

        double point_values[4] = {p.x, p.y, p.z, p.w};

        double product_values[4] = {0, 0, 0, 0};

        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                product_values[i] += this->values[i][j] * point_values[j];
            }
        }

        product.x = product_values[0];
        product.y = product_values[1];
        product.z = product_values[2];
        product.w = product_values[3];

        return product;
    }


    void print()
    {
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                cout<<values[i][j]<<" ";
            }

            cout<<endl;

        }

        cout<<endl;
    }
};


point eye;
point look;
point right;
point up;

point i_vector(1, 0, 0);
point j_vector(0, 1, 0);
point k_vector(0, 0, 1);

double fovY;
double aspectRatio;
double near;
double far;

stack<matrix> matrices;
stack<int> push_marks;
vector<triangle> triangles;
vector<triangle> triangles_viewed;
vector<triangle> triangles_projected;

int screen_width;
int screen_height;

double Left_limit_X;
double Right_limit_X;

double Bottom_limit_Y;
double Top_limit_Y;

double Front_limit_Z;
double Rear_limit_Z;

double ** z_buffer;
color ** frame_buffer;

void initialize_buffers()
{
    z_buffer = new double*[screen_height];
    frame_buffer = new color*[screen_height];

    for(int i=0;i<screen_height;i++)
    {
        z_buffer[i] = new double[screen_width];
        frame_buffer[i] = new color[screen_width];
    }

    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            z_buffer[i][j] = Rear_limit_Z;

            for(int k=0;k<3;k++)
            {
                frame_buffer[i][j].rgb[k] = 0;
            }
            
        }
    }
}

void free_buffer_memory()
{
    for (int i = 0; i < screen_height; i++)
    {
        delete[] z_buffer[i];
        delete[] frame_buffer[i];
    }

    delete[] z_buffer;
    delete[] frame_buffer;
    
}


void printTriangles(vector<triangle> t, int stage)
{
    string filename = "output/stage" + to_string(stage) + ".txt";

    //ofstream output(filename);
    output.open(filename);

    output<<"";

    output.close();

    output.open(filename, ios::app);

    output<<setprecision(7)<<fixed;

    int size = t.size();

    for(int i=0;i<size;i++)
    {
        //t[i].print();
        t[i].printToFile();
        //cout<<endl;
    }

    output.close();
}

void print_Z_Buffer()
{
    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
           double val = z_buffer[i][j];

           if(val < Rear_limit_Z)
           {
               cout<<val<<" ";
           }
        }
        cout<<endl;
    }
}

void print_Z_Buffer_ToFile()
{
    output.open("output/z_buffer.txt");

    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
           double val = z_buffer[i][j];

           if(val < Rear_limit_Z)
           {
               output<<val<<" ";
           }
        }
        output<<endl;
    }

    output.close();
}

void modeling_Transformation()
{
    matrix identity_matrix;

    identity_matrix.identity();

    //identity_matrix.print();
    matrices.push(identity_matrix);

    ifstream input("input/" + input_folder + "/scene.txt");

    input>>eye.x;
    input>>eye.y;
    input>>eye.z;

    input>>look.x;
    input>>look.y;
    input>>look.z;
    
    input>>up.x;
    input>>up.y;
    input>>up.z;

    input>>fovY;
    input>>aspectRatio;
    input>>near;
    input>>far;

    
    // eye.print();
    // look.print();
    // up.print();
    
    //up.normalize();

    


    while (true)
    {
        string command;

        input>>command;

        //cout<<command<<endl;

        if(command == "triangle")
        {
            triangle t;

            for(int i=0;i<3;i++)
            {
                input>>t.points[i].x;
                input>>t.points[i].y;
                input>>t.points[i].z;
            }

            // t.a.print();
            // t.b.print();
            // t.c.print();

            matrix top = matrices.top();

            //top.print();

            for(int i=0;i<3;i++)
            {
                t.points[i] = top.multiply(t.points[i]);
            }

            triangles.push_back(t);



            // t.a.print();
            // t.b.print();
            // t.c.print();
            // cout<<endl;
        }

        else if(command == "translate")
        {
            double tx, ty, tz;

            input>>tx>>ty>>tz;

            matrix T;

            T.translationMatrix(tx, ty, tz);
            //T.print();
            //cout<<endl;

            matrix top = matrices.top();

            matrix product = top.multiply(T);

            //product.print();

            matrices.push(product);
        }

        else if(command == "scale")
        {
            double sx, sy, sz;

            input>>sx>>sy>>sz;

            matrix S;

            S.scalingMatrix(sx, sy, sz);

            //S.print();
            //cout<<endl;

            matrix top = matrices.top();

            matrix product = top.multiply(S);

            //product.print();

            matrices.push(product);
        }

        else if(command == "rotate")
        {
            double angle, ax, ay, az;

            input>>angle>>ax>>ay>>az;

            angle *= PI / 180;   //  take in radian

            point axis(ax, ay, az);

            axis.normalize();

            //axis.print();

            // i_vector.print();
            // j_vector.print();
            // k_vector.print();

            point i_rotated = i_vector.rotate(axis, angle);
            point j_rotated = j_vector.rotate(axis, angle);
            point k_rotated = k_vector.rotate(axis, angle);

            matrix R;

            // i_rotated.print();
            // j_rotated.print();
            // k_rotated.print();

            R.rotationMatrix(i_rotated, j_rotated, k_rotated);

            //R.print();

            matrix top = matrices.top();

            matrix product = top.multiply(R);

            //product.print();

            matrices.push(product);
        }

        else if(command == "push")
        {
            int size = matrices.size();

            push_marks.push(size);
        }

        else if(command == "pop")
        {
            if(push_marks.size() > 0)
            {
                int top = push_marks.top();

                while (matrices.size() != top)
                {
                    matrices.pop();
                }
                

                push_marks.pop();
            }
        }

        else if(command == "end")
            break;
    }
    
    input.close();

    printTriangles(triangles, 1);
}


void view_Transformation()
{
    point l;
    point r;
    point u;

    l = look.sub(eye);

    l.normalize();

    r = l.crossProduct(up);

    r.normalize();

    u = r.crossProduct(l);

    matrix T;

    T.translationMatrix(-eye.x, -eye.y, -eye.z);

    matrix R;

    double arr[4][4] = {{r.x, r.y, r.z, 0}, {u.x, u.y, u.z, 0}, {-l.x, -l.y, -l.z, 0}, {0, 0, 0, 1}};

    R.setValues(arr);

    matrix V = R.multiply(T);

    int size = triangles.size();

    for(int i=0;i<size;i++)
    {
        triangle viewed;

        for(int j=0;j<3;j++)
        {
            viewed.points[j] = V.multiply(triangles[i].points[j]);
        }

        triangles_viewed.push_back(viewed);
    }

    printTriangles(triangles_viewed, 2);
   
}

void projection_Transformation()
{
    double fovX = fovY * aspectRatio;
    double t = near * tan((fovY/2) * (PI/180));
    double r = near * tan((fovX/2) * (PI/180));

    double v00 = near/r;
    double v11 = near/t;

    double v22 = -(far + near)/(far - near);
    double v23 = -(2 * far * near)/(far - near);

    double values[4][4] = {{v00, 0, 0, 0}, {0, v11, 0, 0}, {0, 0, v22, v23}, {0, 0, -1, 0}};

    matrix P;

    P.setValues(values);

    //P.print();

    int size = triangles_viewed.size();

    for(int i=0;i<size;i++)
    {
        triangle t;

        for(int j=0;j<3;j++)
        {
            t.points[j] = P.multiply(triangles_viewed[i].points[j]);

            t.points[j].scale_w_to_1();
        }

        t.setRandomColor();

        triangles_projected.push_back(t);
    }

     printTriangles(triangles_projected, 3);
}

double left_column;
double right_column;

double left_z;
double right_z;

void find_Intersecting_X_Z(double xa, double xb, double za, double zb)
{
    if(xa < xb)
    {
        left_column = xa;
        right_column = xb;

        left_z = za;
        right_z = zb;
    }
    else
    {
        left_column = xb;
        right_column = xa;

        left_z = zb;
        right_z = za;
    }

}

void Z_buffer_Procedure()
{
    ifstream input;
    input.open("input/" + input_folder + "/config.txt");

    input>>screen_width;
    input>>screen_height;

    input>>Left_limit_X;
    Right_limit_X = -Left_limit_X;

    input>>Bottom_limit_Y;
    Top_limit_Y = -Bottom_limit_Y;

    input>>Front_limit_Z;
    input>>Rear_limit_Z;

    // cout<<screen_width<<endl;
    // cout<<screen_height<<endl;
    // cout<<Bottom_limit_Y<<endl;
    // cout<<Left_limit_X<<endl;
    // cout<<Front_limit_Z<<endl;
    // cout<<Rear_limit_Z<<endl;

    input.close();

    initialize_buffers();

    double dx = (Right_limit_X - Left_limit_X) / screen_width;
    double dy = (Top_limit_Y - Bottom_limit_Y) / screen_height;

    // cout<<dx<<endl;
    // cout<<dy<<endl;

    double Left_X = Left_limit_X + dx/2;
    double Top_Y = Top_limit_Y - dy/2;

    double Right_X = Right_limit_X - dx/2;
    double Bottom_Y = Bottom_limit_Y + dy/2;

    // cout<<Left_X<<endl;
    // cout<<Top_Y<<endl;
    // cout<<Right_X<<endl;
    // cout<<Bottom_Y<<endl;

    for(triangle t : triangles_projected)
    {
        double top_scanline = Top_Y;
        double bottom_scanline = Bottom_Y;

        double max_x = t.max_X();
        double min_x = t.min_X();

        double max_y = t.max_Y();
        double min_y = t.min_Y();

        int start_row = 0;
        int end_row = screen_height - 1;

        if(max_y < Top_Y)
        {
            //cout<<"max y = "<<max_y<<endl;
            start_row = ceil((Top_Y - max_y)/dy);

            top_scanline = Top_Y - start_row * dy;

        }

        if(min_y > Bottom_Y)
        {
            //cout<<"min y = "<<min_y/dy<<endl;
            end_row = floor((Top_Y - min_y)/dy);

            bottom_scanline = Top_Y - end_row * dy;
        }


        for(int row=start_row;row<=end_row;row++)
        {
            double scanline = Top_Y - (row * dy);

            double xa = t.solveX_with_scanline(0, 1, scanline);
            double xb = t.solveX_with_scanline(0, 2, scanline);
            double xc = t.solveX_with_scanline(1, 2, scanline);

            double za = t.solveZ_with_scanline(0, 1, scanline);
            double zb = t.solveZ_with_scanline(0, 2, scanline);
            double zc = t.solveZ_with_scanline(1, 2, scanline);
            

            if(xc < min_x || xc > max_x)
            {
                find_Intersecting_X_Z(xa, xb, za, zb);
            }

            else if(xa < min_x || xa > max_x)
            {
                find_Intersecting_X_Z(xb, xc, zb, zc);
            }

            else
            {
                find_Intersecting_X_Z(xc, xa, zc, za);             
            }

            // cout<<endl;
            // cout<<left_column<<endl;
            // cout<<right_column<<endl;
            // cout<<left_z<<endl;
            // cout<<right_z<<endl;
            // cout<<endl;

            double start_column = 0;
            double end_column = screen_width - 1;

            if(left_column > Left_X)
            {
                start_column = round((left_column - Left_X)/dx);
            }

            if(right_column < Right_X)
            {
                end_column = round((right_column - Left_X)/dx);
            }


            for(int column=start_column;column<=end_column;column++)
            {
                double intersecting_column = Left_X + (column * dx);

                // cout<<right_z<<" ";
                // cout<<left_z;
                // cout<<endl;

                double zp = right_z -
                           ((right_z - left_z) / (right_column - left_column)) * (right_column - intersecting_column);

                
                if(zp >= Front_limit_Z && zp < z_buffer[row][column])
                {
                    z_buffer[row][column] = zp;


                    for(int i=0;i<3;i++)
                    {
                        frame_buffer[row][column].rgb[i] = t.clr.rgb[i];
                    }
                }

                
            }
            
        }
        
    }

    //print_Z_Buffer();
    print_Z_Buffer_ToFile();

    //free_buffer_memory();
}


void drawBMP()
{
    bitmap_image image(screen_width,screen_height);

    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            image.set_pixel(j, i, frame_buffer[i][j].rgb[0], frame_buffer[i][j].rgb[1], frame_buffer[i][j].rgb[2]);
        }
    }

    image.save_image("output/output.bmp");
}



int main()
{
    //srand(time(0));
    cout<<setprecision(7)<<fixed;
    
    modeling_Transformation();
    view_Transformation();
    projection_Transformation();

    Z_buffer_Procedure();
    //print_Z_Buffer();

    drawBMP();

    free_buffer_memory();

    return 0;
}