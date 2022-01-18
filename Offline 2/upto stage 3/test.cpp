#include<iostream>
#include<stdlib.h>
#include<math.h>
#include <bits/stdc++.h>
#include<string.h>
#include<list>
#include<fstream>

using namespace std;

ofstream output;

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

class triangle
{
    public:

    point points[3];
    int colors[3];

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

double screen_width;
double screen_height;

double Left_X;
double Right_X;

double Bottom_Y;
double Top_Y;

double Front_Z;
double Rear_Z;

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
        t[i].print();
        t[i].printToFile();
        cout<<endl;
    }

    output.close();
}


void modeling_Transformation()
{
    matrix identity_matrix;

    identity_matrix.identity();

    //identity_matrix.print();
    matrices.push(identity_matrix);

    ifstream input("input/1/scene.txt");

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
            cout<<endl;

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
            cout<<endl;

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

        triangles_projected.push_back(t);
    }

     printTriangles(triangles_projected, 3);
}

void Z_buffer()
{
    ifstream input;
    input.open("1/config.txt");

    input>>screen_width;
    input>>screen_height;

    input>>Bottom_Y;
    Top_Y = -Bottom_Y;

    input>>Left_X;
    Right_X = -Left_X;

    input>>Front_Z;
    input>>Rear_Z;

    input.close();
}


int main()
{
    cout<<setprecision(7)<<fixed;
    //setprecision(5);
    //cout<<PI<<endl;
    modeling_Transformation();
    view_Transformation();
    projection_Transformation();

    
    

    return 0;
}