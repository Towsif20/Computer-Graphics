
class Point
{
	public:
	double x,y,z;

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

	double dotProduct(Point A)
    {
        return A.x*x + A.y*y + A.z*z;
    }

    Point crossProduct(Point B)
    {
        Point product;

        product.x = y * B.z - z * B.y;
        product.y = z * B.x - x * B.z;
        product.z = x * B.y - y * B.x;

        return product;
    }

    point rotate(point axis, double angle)
    {
        point rotated;

        point toBeRotated;

        toBeRotated.x = this->x;
        toBeRotated.y = this->y;
        toBeRotated.z = this->z;

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

        Point rotated;

        double c = cos(rotationAngle);
        double s = sin(rotationAngle);
        struct Point cross = crossProduct(axis, toBeRotated);
        double dot = dotProduct(axis, toBeRotated);

        rotated.x = toBeRotated.x*c + cross.x*s + axis.x*dot*(1-c);
        rotated.y = toBeRotated.y*c + cross.y*s + axis.y*dot*(1-c);
        rotated.z = toBeRotated.z*c + cross.z*s + axis.z*dot*(1-c);

        return rotated;
    }

	void print()
	{
	    cout<<x<<" "<<y<<" "<<z<<endl;
	}
};

//double dotProduct(Point A, Point B)
//{
//    return A.x*B.x + A.y*B.y + A.z*B.z;
//}
//
//Point crossProduct(struct Point A, struct Point B)
//{
//    Point product;
//
//    product.x = A.y * B.z - A.z * B.y;
//    product.y = A.z * B.x - A.x * B.z;
//    product.z = A.x * B.y - A.y * B.x;
//
//    return product;
//}
//
//
//Point rotateVector(Point toBeRotated, Point axis, double rotationAngle)
//{
//    Point rotated;
//
//    double c = cos(rotationAngle);
//    double s = sin(rotationAngle);
//    struct Point cross = crossProduct(axis, toBeRotated);
//    double dot = dotProduct(axis, toBeRotated);
//
//    rotated.x = toBeRotated.x*c + cross.x*s + axis.x*dot*(1-c);
//    rotated.y = toBeRotated.y*c + cross.y*s + axis.y*dot*(1-c);
//    rotated.z = toBeRotated.z*c + cross.z*s + axis.z*dot*(1-c);
//
//    return rotated;
//}


class Object
{
    public:

    Point reference_point;
    double height, width, length;
    double color[3];
    double coEfficients[4]; // reflection coefficients
    int shine; // exponent term of specular component

    object(){}
    virtual void draw(){}
    virtual void print(){}

    void setReferencePoint(Point p)
    {
        reference_point.x = p.x;
        reference_point.y = p.y;
        reference_point.z = p.z;
    }

    void setLength(double l)
    {
        length = l;
    }

    void setWidth(double w)
    {
        width = w;
    }

    void setHeight(double h)
    {
        height = h;
    }

    void setColor(double r, double g, double b)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setShine(int sh)
    {
        shine = sh;
    }

    void setCoEfficients(double ambient, double diffuse, double specular, double recurs)
    {
        coEfficients[0] = ambient;
        coEfficients[1] = diffuse;
        coEfficients[2] = specular;
        coEfficients[3] = recurs;
    }

    void printBasic()
    {
        for(int i=0;i<3;i++)
        {
            cout<<color[i]<<" ";
        }
        cout<<endl;

        for(int i=0;i<4;i++)
        {
            cout<<coEfficients[i]<<" ";
        }
        cout<<endl;

        cout<<shine<<endl;
    }
};

class Sphere : public Object
{

    public:

    Sphere(Point center, double radius)
    {
        reference_point.x = center.x;
        reference_point.y = center.y;
        reference_point.z = center.z;

        length = radius;
    }

    void draw()
    {
        // write codes for drawing sphere

        glPushMatrix();

        glTranslatef(reference_point.x, reference_point.y, reference_point.z);

        int slices = 60;
        int stacks = 20;

        Point points[100][100];
        int i,j;
        double h,r;

        //generate points
        for(i=0;i<=stacks;i++)
        {
            h=length*sin(((double)i/(double)stacks)*(pi/2));
            r=length*cos(((double)i/(double)stacks)*(pi/2));
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
            //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);

            glColor3f(color[0], color[1], color[2]);

            for(j=0;j<slices;j++)
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

        glPopMatrix();
    }

    void print()
    {
        reference_point.print();
        cout<<length<<endl;
        printBasic();
    }
};


class Triangle : public Object
{
    public:

    Point points[3];

    Triangle(Point p[3])
    {
        for(int i=0;i<3;i++)
        {
            points[i].x = p[i].x;
            points[i].y = p[i].y;
            points[i].z = p[i].z;
        }
    }

    void draw()
    {
        // write codes for drawing sphere
        glBegin(GL_TRIANGLES);
        {
            glColor3f(color[0], color[1], color[2]);

            for(int i=0;i<3;i++)
            {
                glVertex3f(points[i].x, points[i].y, points[i].z);
            }
        }
        glEnd();
    }

    void print()
    {
        for(int i=0;i<3;i++)
        {
            points[i].print();
        }

        printBasic();
    }
};

class General : public Object
{
    public:

    double A, B, C, D, E, F, G, H, I, J;

    General(double A, double B, double C, double D, double E, double F, double G, double H, double I, double J)
    {
        this->A = A;
        this->B = B;
        this->C = C;
        this->D = D;
        this->E = E;
        this->F = F;
        this->G = G;
        this->H = H;
        this->I = I;
        this->J = J;
    }

    void draw()
    {
        // write codes for drawing sphere

    }

    void print()
    {
        cout<<this->A<<" ";
        cout<<this->B<<" ";
        cout<<this->C<<" ";
        cout<<this->D<<" ";
        cout<<this->E<<" ";
        cout<<this->F<<" ";
        cout<<this->G<<" ";
        cout<<this->H<<" ";
        cout<<this->I<<" ";
        cout<<this->J<<" ";

        cout<<endl;

        printBasic();
    }
};

class Floor : public Object
{
    public:
    double floor_size;

    Floor(double floorWidth, double tileWidth)
    {
        reference_point.x = -floorWidth/2;
        reference_point.y = -floorWidth/2;
        reference_point.z = 0;

        length = tileWidth;
        floor_size = floorWidth;

        setCoEfficients(0.2, 0.2, 0.2, 0.2);
        setColor(0.5, 0.5, 0.5);
        setShine(5);
    }


    void draw()
    {
        bool flag = false;

        for(double i=reference_point.x ; i<=floor_size/2 ; i+=length)
        {
            for(double j=reference_point.y ; j<=floor_size/2 ; j+=length)
            {
                if(flag)
                {
                    glColor3f(0.2 , 0.2 , 0.2);
                }
                else
                {
                    glColor3f(0.8 , 0.8 , 0.8);
                }

                flag = !flag;

                glBegin(GL_QUADS);
                {
                    glVertex3f(i , j , 0);
                    glVertex3f(i , j+length , 0);
                    glVertex3f(i+length , j+length , 0);
                    glVertex3f(i+length , j , 0);
                }
                glEnd();
            }
        }
    }

};




class Light
{
    public:

    Point light_pos;
    double color[3];

    void setLightPos(Point p)
    {
        light_pos.x = p.x;
        light_pos.y = p.y;
        light_pos.z = p.z;
    }

    void setColor(double r, double g, double b)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void draw()
    {
        glPointSize(5);
        glColor3f(color[0] , color[1] , color[2]);

        glBegin(GL_POINTS);

        glVertex3f(light_pos.x , light_pos.y , light_pos.z);

        glEnd();
    }

    void print()
    {
        light_pos.print();
        for(int i=0;i<3;i++)
        {
            cout<<color[i]<<" ";
        }
        cout<<endl;
    }
};


































