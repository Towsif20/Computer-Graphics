class Object;

extern vector<Object*> objects;
extern int level_of_recursion;


class Point
{
	public:
	double x,y,z;

	Point()
	{

	}

	Point(double x, double y, double z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}

	double absolute_value()
	{
	    return sqrt(x*x + y*y + z*z);
	}

	Point scale(double f)
	{
	    Point res;

	    res.x = x * f;
	    res.y = y * f;
	    res.z = z * f;

	    return res;
	}

	Point add(Point p)
	{
	    Point res;

	    res.x = x + p.x;
	    res.y = y + p.y;
	    res.z = z + p.z;

	    return res;
	}

	Point sub(Point p)
	{
	    Point res;

	    res.x = x - p.x;
	    res.y = y - p.y;
	    res.z = z - p.z;

	    return res;
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

    Point rotate(Point axis, double angle)
    {
        Point rotated;

        Point toBeRotated;

        toBeRotated.x = this->x;
        toBeRotated.y = this->y;
        toBeRotated.z = this->z;

        double c = cos(angle);

        if(c < 0.0001)
            c = 0;

        double s = sin(angle);

        Point cross = axis.crossProduct(toBeRotated);
        double dot = axis.dotProduct(toBeRotated);

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


extern vector<Light> lights;








class Ray
{
    public:

    Point start;
    Point dir;

    Ray(Point s, Point d)
    {
        start.x = s.x;
        start.y = s.y;
        start.z = s.z;

        dir.x = d.x;
        dir.y = d.y;
        dir.z = d.z;

        dir.normalize();
    }

};









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
    virtual Point normalVector(Point p)
    {
        return Point(0, 0, 1);
    }

    virtual double intersect(Ray r, double *color, int level)
    {
        return -1.0;
    }

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
        reference_point.print();

        cout<<"length = "<<length<<endl;
        cout<<"width = "<<width<<endl;
        cout<<"height = "<<height<<endl;

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

    Point normalVector(Point p)
    {
        Point n = p.sub(reference_point);

        n.normalize();

        return n;
    }

    double intersect(Ray ray, double *color, int level)
    {
//        for(int i=0;i<3;i++)
//        {
//            color[i] = this->color[i];
//        }

        Point oc = ray.start.sub(reference_point);


        double a = ray.dir.dotProduct(ray.dir);

        double b = 2.0 * oc.dotProduct(ray.dir);

        double c = oc.dotProduct(oc) - length*length;

        double discriminant = b*b - 4*a*c;


        double t = -1;

        if(discriminant < 0.0)
        {
            return -1.0;
        }

        double numerator = -b - sqrt(discriminant);

        if(numerator > 0.0)
        {
            //cout<<numerator<<endl;
            t = numerator / (2.0 * a);
        }

        else
        {
            numerator = -b + sqrt(discriminant);

            if(numerator > 0.0)
            {
                //cout<<numerator<<endl;
                t = numerator / (2.0 * a);
            }
            else
            {
                return -1;
            }
        }


        if(level == 0)
        {
            return t;
        }

        Point intersecting_point(ray.start.x + t*ray.dir.x, ray.start.y + t*ray.dir.y, ray.start.z + t*ray.dir.z);

        double color_to_pass[3];

        for(int i=0;i<3;i++)
        {
            color_to_pass[i] = this->color[i] * coEfficients[0];
        }

        Point n = this->normalVector(intersecting_point);

        for(int i=0;i<lights.size();i++)
        {
            Ray light_to_point(lights[i].light_pos, intersecting_point.sub(lights[i].light_pos));

            double t_current = (intersecting_point.x - light_to_point.start.x)/light_to_point.dir.x;

            double* dummy_again = new double[3];
            bool hidden = false;

            for(int j=0;j<objects.size();j++)
            {
                double t_temp = objects[j]->intersect(light_to_point, dummy_again, 0);
                if(t_temp > 0 && floor(t_temp) < floor(t_current))
                {
                    hidden = true;
                    break;
                }
            }

            delete dummy_again;

            if(hidden)
                continue;



            double lambert = n.dotProduct(light_to_point.dir);

//            if(lambert < 0)
//                lambert = 0;


            //Point reflected = n.scale(2*lambert).sub(light_to_point.dir);
            Point reflected = light_to_point.dir.sub(n.scale(2*lambert));

            double phong = -reflected.dotProduct(ray.dir);
            if(phong < 0)
                phong = 0;

            for(int j=0;j<3;j++)
            {

                color_to_pass[j] += (lights[i].color[j]
                                            *this->coEfficients[1]
                                            *(lambert)*this->color[j]);

                color_to_pass[j] += (lights[i].color[j]
                                            *this->coEfficients[2]*pow((phong), shine));


            }


        }

        for(int j=0;j<3;j++)
        {
           color[j] = color_to_pass[j];

        }

        if(level >= level_of_recursion)
            return t;

        double lambert = n.dotProduct(ray.dir);

        Point reflected_direction = ray.dir.sub(n.scale(2*lambert));

        Point dummy_intersection = intersecting_point.add(reflected_direction);

        Ray reflected_ray(dummy_intersection, reflected_direction);

        int nearest = -1;
        int tMin = 1000000;
        double* dummy = new double[3];
        double* color_after_reflected = new double[3];

        for(int i=0;i<objects.size();i++)
        {
            double t_temp = objects[i]->intersect(reflected_ray, dummy, 0);

            if(t_temp > 0 && t_temp < tMin)
            {
                tMin = t_temp;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            tMin = objects[nearest]->intersect(reflected_ray, color_after_reflected, level + 1);

            for(int i=0;i<3;i++)
            {
                color[i] += color_after_reflected[i] * coEfficients[3];
            }
        }

        delete dummy;
        delete color_after_reflected;

        return t;


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

    Point normalVector(Point p)
    {
        Point e1 = points[1].sub(points[0]);
        Point e2 = points[2].sub(points[0]);

        Point n = e1.crossProduct(e2);

        n.normalize();

        return n;
    }

    double intersect(Ray ray, double* color, int level)
    {
        for(int i=0;i<3;i++)
        {
            color[i] = this->color[i];
        }

        Point e1 = points[0].sub(points[1]);
        Point e2 = points[0].sub(points[2]);

        Point h = e1.crossProduct(e2);

        double a = h.dotProduct(ray.dir);

        if (abs(a) < 0.00001)
        {
            return -1.0;
        }

        double f = 1/a;

        Point d = points[0].sub(ray.start);

        Point c = d.crossProduct(e2);

        double u = f * c.dotProduct(ray.dir);

        if (u < 0.0 || u > 1.0)
        {
            return -1.0;
        }

        Point q = e1.crossProduct(d);

        double v = f * q.dotProduct(ray.dir);

        if(v < 0.0 || (u+v) > 1.0)
        {
            return -1.0;
        }

        double t = f * h.dotProduct(d);

        if(t <= 0.00001)
        {
            //cout<<"triangle intersect : "<<t<<endl;
            return -1;
        }

        if(level == 0)
            return t;


        Point intersecting_point(ray.start.x + t*ray.dir.x, ray.start.y + t*ray.dir.y, ray.start.z + t*ray.dir.z);

        double color_to_pass[3];

        for(int i=0;i<3;i++)
        {
            color_to_pass[i] = this->color[i] * coEfficients[0];
        }

        Point n = this->normalVector(intersecting_point);

        for(int i=0;i<lights.size();i++)
        {
            Ray light_to_point(lights[i].light_pos, intersecting_point.sub(lights[i].light_pos));

            double t_current = (intersecting_point.x - light_to_point.start.x)/light_to_point.dir.x;

            double* dummy = new double[3];
            bool hidden = false;

            for(int j=0;j<objects.size();j++)
            {
                double t_temp = objects[j]->intersect(light_to_point, dummy, 0);
                if(t_temp > 0 && floor(t_temp) < floor(t_current))
                {
                    hidden = true;
                    break;
                }
            }

            delete dummy;

            if(hidden)
                continue;



            double lambert = n.dotProduct(light_to_point.dir);
//            if(lambert < 0)
//                lambert = 0;

            //Point reflected = n.scale(2*lambert).sub(light_to_point.dir);
            Point reflected = light_to_point.dir.sub(n.scale(2*lambert));

            double phong = -reflected.dotProduct(ray.dir);
            if(phong < 0)
                phong = 0;

            for(int j=0;j<3;j++)
            {

                color_to_pass[j] += lights[i].color[j]
                                            *this->coEfficients[1]
                                            *(lambert)*this->color[j];

                color_to_pass[j] += lights[i].color[j]
                                            *this->coEfficients[2]*pow((phong), shine);


            }


        }

        for(int j=0;j<3;j++)
        {
           color[j] = color_to_pass[j];

        }

        if(level >= level_of_recursion)
            return t;

        double lambert = n.dotProduct(ray.dir);

        Point reflected_direction = ray.dir.sub(n.scale(2*lambert));

        Point dummy_intersection = intersecting_point.add(reflected_direction);

        Ray reflected_ray(dummy_intersection, reflected_direction);

        int nearest = -1;
        int tMin = 1000000;
        double* dummy = new double[3];
        double* color_after_reflected = new double[3];

        for(int i=0;i<objects.size();i++)
        {
            double t_temp = objects[i]->intersect(reflected_ray, dummy, 0);

            if(t_temp > 0 && t_temp < tMin)
            {
                tMin = t_temp;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            tMin = objects[nearest]->intersect(reflected_ray, color_after_reflected, level + 1);

            for(int i=0;i<3;i++)
            {
                color[i] += color_after_reflected[i] * coEfficients[3];
            }
        }

        delete dummy;
        delete color_after_reflected;


        return t;
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

    Point normalVector(Point p)
    {
        double x, y, z;

        x  = p.x;
        y  = p.y;
        z  = p.z;

        Point n;

        n.x = 2*A*x + D*y + E*z + G;
        n.y = 2*B*y + D*x + F*z + H;
        n.z = 2*C*z + E*x + F*y + I;

        n.normalize();

        return n;
    }

    double intersect(Ray ray, double* color, int level)
    {
        for(int i=0;i<3;i++)
        {
            color[i] = this->color[i];
        }

        double rx = ray.start.x;
        double ry = ray.start.y;
        double rz = ray.start.z;

        double dx = ray.dir.x;
        double dy = ray.dir.y;
        double dz = ray.dir.z;

        double a = A*dx*dx + B*dy*dy + C*dz*dz + D*dx*dy + E*dy*dz + F*dz*dx;

        double b = 2*(A*rx*dx + B*ry*dy + C*rz*dz)
                 + D*(rx*dy + ry*dx) + E*(ry*dz + rz*dy) +  F*(rz*dx + rx*dz)
                 + G*dx + H*dy + I*dz;

        double c = A*rx*rx + B*ry*ry + C*rz*rz
                 + D*rx*ry + E*ry*rz + F*rz*rx
                 + G*rx + H*ry + I*rz + J;


        double discriminant = b*b - 4*a*c;


        if(discriminant < 0.0)
        {
            return -1.0;
        }

        double numerator1 = -b - sqrt(discriminant);
        double numerator2 = -b + sqrt(discriminant);

        double t1 = numerator1/(2.0 * a);
        double t2 = numerator2/(2.0 * a);


        double t = -1.0;

        if(t1 < 0 && t2 < 0)
        {
            return -1.0;
        }

        if(t1 < 0)
        {
            t = t2;
        }

        if(t2 < 0)
        {
            t = t1;
        }

        if(t1 >= 0 && t2 >= 0)
        {
            Point intersecting_point1(rx + t1*dx, ry + t1*dy, rz + t1*dz);
            Point intersecting_point2(rx + t2*dx, ry + t2*dy, rz + t2*dz);

            Point temp1 = intersecting_point1.sub(reference_point);
            Point temp2 = intersecting_point2.sub(reference_point);

            if(length > 0)
            {
                if(abs(temp1.x) > length && abs(temp2.x) > length)
                    return -1.0;

                if(abs(temp1.x) > length)
                    t = t2;

                else if(abs(temp2.x) > length)
                    t = t1;

                else
                    t = min(t1, t2);
            }

            if(width > 0)
            {
                if(abs(temp1.y) > width && abs(temp2.y) > width)
                    return -1.0;

                if(abs(temp1.y) > width)
                    t = t2;

                else if(abs(temp2.y) > width)
                    t = t1;

                else
                    t = min(t1, t2);
            }

            if(height > 0)
            {
                if(abs(temp1.z) > height && abs(temp2.z) > height)
                    return -1.0;

                if(abs(temp1.z) > height)
                    t = t2;

                else if(abs(temp2.z) > height)
                    t = t1;

                else
                    t = min(t1, t2);
            }
        }

        if(level == 0)
            return t;


        Point intersecting_point(ray.start.x + t*ray.dir.x, ray.start.y + t*ray.dir.y, ray.start.z + t*ray.dir.z);

        double color_to_pass[3];

        for(int i=0;i<3;i++)
        {
            color_to_pass[i] = this->color[i] * coEfficients[0];
        }

        Point n = this->normalVector(intersecting_point);

        for(int i=0;i<lights.size();i++)
        {
            Ray light_to_point(lights[i].light_pos, intersecting_point.sub(lights[i].light_pos));

            double t_current = (intersecting_point.x - light_to_point.start.x)/light_to_point.dir.x;

            double* dummy = new double[3];
            bool hidden = false;

            for(int j=0;j<objects.size();j++)
            {
                double t_temp = objects[j]->intersect(light_to_point, dummy, 0);
                if(t_temp > 0 && floor(t_temp) < floor(t_current))
                {
                    hidden = true;
                    break;
                }
            }

            delete dummy;

            if(hidden)
                continue;



            double lambert = n.dotProduct(light_to_point.dir);

//            if(lambert < 0)
//                lambert = 0;

            //Point reflected = n.scale(2*lambert).sub(light_to_point.dir);
            Point reflected = light_to_point.dir.sub(n.scale(2*lambert));

            double phong = -reflected.dotProduct(ray.dir);
            if(phong < 0)
                phong = 0;

            for(int j=0;j<3;j++)
            {

                color_to_pass[j] += lights[i].color[j]
                                            *this->coEfficients[1]
                                            *(lambert)*this->color[j];

                color_to_pass[j] += lights[i].color[j]
                                            *this->coEfficients[2]*pow((phong), shine);


            }


        }

        for(int j=0;j<3;j++)
        {
           color[j] = color_to_pass[j];

        }

        if(level >= level_of_recursion)
            return t;

        double lambert = n.dotProduct(ray.dir);

        Point reflected_direction = ray.dir.sub(n.scale(2*lambert));

        Point dummy_intersection = intersecting_point.add(reflected_direction);

        Ray reflected_ray(dummy_intersection, reflected_direction);

        int nearest = -1;
        int tMin = 1000000;
        double* dummy = new double[3];
        double* color_after_reflected = new double[3];

        for(int i=0;i<objects.size();i++)
        {
            double t_temp = objects[i]->intersect(reflected_ray, dummy, 0);

            if(t_temp > 0 && t_temp < tMin)
            {
                tMin = t_temp;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            tMin = objects[nearest]->intersect(reflected_ray, color_after_reflected, level + 1);

            for(int i=0;i<3;i++)
            {
                color[i] += color_after_reflected[i] * coEfficients[3];
            }
        }

        delete dummy;
        delete color_after_reflected;


        return t;

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
        for(double i=reference_point.x ; i<=floor_size/2 ; i+=length)
        {
            int row = floor((i - reference_point.x)/length);

            for(double j=reference_point.y ; j<=floor_size/2 ; j+=length)
            {
                int col = floor((j - reference_point.y)/length);

                if((row+col)%2 == 0)
                {
                    glColor3f(0.2 , 0.2 , 0.2);
                }
                else
                {
                    glColor3f(0.8 , 0.8 , 0.8);
                }


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

    double intersect(Ray ray, double* color, int level)
    {
        Point p0(0, 0, 0);
        Point n(0, 0, 1);

        double denom = n.dotProduct(ray.dir);

        double t = -1.0;

        if(abs(denom) > 0.000001)
        {
            Point p = p0.sub(ray.start);

            t = p.dotProduct(n)/denom;

            Point intersecting_point(ray.start.x + t*ray.dir.x, ray.start.y + t*ray.dir.y, 0);

            if(abs(intersecting_point.x) > floor_size/2 || abs(intersecting_point.y) > floor_size/2)
                return -1.0;

            int row = floor((intersecting_point.x - reference_point.x)/length);
            int col = floor((intersecting_point.y - reference_point.y)/length);

            if((row+col)%2 == 0)
            {
                for(int i=0;i<3;i++)
                {
                    color[i] = 0.2;
                }
            }
            else
            {
                for(int i=0;i<3;i++)
                {
                    color[i] = 0.8;
                }
            }
        }

        //cout<<"floor intersect: "<<t<<endl;
        if(level == 0)
            return t;

        Point intersecting_point(ray.start.x + t*ray.dir.x, ray.start.y + t*ray.dir.y, ray.start.z + t*ray.dir.z);

        double color_to_pass[3];

        int row = floor((intersecting_point.x - reference_point.x)/length);
        int col = floor((intersecting_point.y - reference_point.y)/length);

        if((row+col)%2 == 0)
        {
            for(int i=0;i<3;i++)
            {
                this->color[i] = 0.2;
            }
        }
        else
        {
            for(int i=0;i<3;i++)
            {
                this->color[i] = 0.8;
            }
        }

        for(int i=0;i<3;i++)
        {
            color_to_pass[i] = this->color[i] * coEfficients[0];
        }

        for(int i=0;i<lights.size();i++)
        {
            Ray light_to_point(lights[i].light_pos, intersecting_point.sub(lights[i].light_pos));

            double t_current = (intersecting_point.x - light_to_point.start.x)/light_to_point.dir.x;

            double* dummy = new double[3];
            bool hidden = false;

            for(int j=0;j<objects.size();j++)
            {
                double t_temp = objects[j]->intersect(light_to_point, dummy, 0);
                if(t_temp > 0 && floor(t_temp) < floor(t_current))
                {
                    hidden = true;
                    break;
                }
            }

            delete dummy;

            if(hidden)
                continue;



            double lambert = n.dotProduct(light_to_point.dir);

//            if(lambert < 0)
//                lambert = 0;

            Point reflected = light_to_point.dir.sub(n.scale(2*lambert));

            double phong = -reflected.dotProduct(ray.dir);
            if(phong < 0)
                phong = 0;

            for(int j=0;j<3;j++)
            {

                color_to_pass[j] += (lights[i].color[j]
                                            *this->coEfficients[1]
                                            *(lambert)*this->color[j]);

                color_to_pass[j] += (lights[i].color[j]
                                            *this->coEfficients[2]*pow((phong), shine));

            }


        }

        for(int j=0;j<3;j++)
        {
           color[j] = color_to_pass[j];
        }

        if(level >= level_of_recursion)
            return t;

        double lambert = n.dotProduct(ray.dir);

        Point reflected_direction = ray.dir.sub(n.scale(2*lambert));

        Point dummy_intersection = intersecting_point.add(reflected_direction);

        Ray reflected_ray(dummy_intersection, reflected_direction);

        int nearest = -1;
        int tMin = 1000000;
        double* dummy = new double[3];
        double* color_after_reflected = new double[3];

        for(int i=0;i<objects.size();i++)
        {
            double t_temp = objects[i]->intersect(reflected_ray, dummy, 0);

            if(t_temp > 0 && t_temp < tMin)
            {
                tMin = t_temp;
                nearest = i;
            }
        }

        if(nearest != -1)
        {
            tMin = objects[nearest]->intersect(reflected_ray, color_after_reflected, level + 1);

            for(int i=0;i<3;i++)
            {
                color[i] += color_after_reflected[i] * coEfficients[3];
            }
        }

        delete dummy;
        delete color_after_reflected;

        //cout<<"floor "<<t<<endl;

        return t;
    }

};



































