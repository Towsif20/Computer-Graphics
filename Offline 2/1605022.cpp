#include<stdio.h>
#include<iostream>
#include<stack>
#include<math.h>
#include<fstream>
#include<string>
#include<cstdlib>
#include<vector>
#include<time.h>
#include<algorithm>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))

ifstream in("scene.txt");
ifstream config("config.txt");
ofstream stage1Output("stage1.txt");
ofstream stage2Output("stage2.txt");
ofstream stage3Output("stage3.txt");
ofstream z_bufferfile("z_buffer.txt");

double screen_width,screen_height;
double left_limit_of_X,right_limit_of_X,bottom_limit_of_Y,top_limit_of_Y;
double front_limit_of_Z,rear_limit_of_Z;
double dx,dy;
double Top_Y,Bottom_Y,Left_X,Right_X;

double zMax;
double** zbuffer;
double s_line;

double max_x_boundary,max_y_boundary,min_x_boundary,min_y_boundary;

int triangle_start_row,triangle_end_row,triangle_start_column,triangle_end_column;
double triangle_top_scaline,triangle_bottom_scaline;

double left_column,right_column,left_column_z,right_column_z;

double current_column;










void skipNFileLines(int N)
{

    string s;
    s.reserve(30);

   //skip N lines
    for(int i = 0; i < N; ++i)
        getline(in, s);
   

}

class Color
{
    public:
    double R,G,B;

};

Color** frame_buffer;


struct point
{
    double x,y,z;
    point(){

    }
    point(double xx,double yy,double zz)
    {
        x=xx;
        y=yy;
        z=zz;
    }


};

struct point_matrix
{
    double arr[4][1];
     point_matrix(){

    }
    point_matrix(struct point p)
    {
      arr[0][0]=p.x;
      arr[1][0]=p.y;
      arr[2][0]=p.z;
      arr[3][0]=1.0;
    }


};

struct Vec
{
    double x,y,z;
    Vec(){

    }
    Vec(double xx,double yy,double zz)
    {
        x=xx;
        y=yy;
        z=zz;
    }
};


struct matrix
{
    double arr[4][4];
};

struct Triangle
{
    point points[3];
    int color[3];

};

void printPoint(struct point p)
{
    cout<<"POINT"<<endl;
    cout<<p.x<<" ";
    cout<<p.y<<" ";
    cout<<p.z<<" ";

    cout<<endl;

}

void printMatirx(struct matrix m)
{
    cout<<"Matrix"<<endl;

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            cout<<m.arr[i][j]<<" ";
        }
        cout<<endl;
    }

}

void printPointMatirx(struct point_matrix m)
{
    cout<<"POINTMatrix"<<endl;
    

    for(int i=0;i<4;i++)
    {
       // //for(int j=0;j<4;j++)
      //  {
            cout<<m.arr[i][0]<<" ";
       // }
       // cout<<endl;
    }
    cout<<endl;


}

void printVec( struct Vec v)
{
    cout<<"Vec"<<endl;
    cout<<v.x<<" ";
    cout<<v.y<<" ";
    cout<<v.z<<" ";

    cout<<endl;

}

void printBuffer()
{
    cout<<"zbuffer"<<endl;
    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            if(zbuffer[i][j]<rear_limit_of_Z)
            {
                cout<<zbuffer[i][j]<<" ";
                z_bufferfile<<zbuffer[i][j]<<" ";
            }
        }
        z_bufferfile<<endl;
        cout<<endl;
    }
}

struct point pointFromPointMatrix(struct point_matrix p)
{
    struct point returnPoint;
    returnPoint.x=p.arr[0][0];
    returnPoint.y=p.arr[1][0];
    returnPoint.z=p.arr[2][0];
    return returnPoint;

}



stack<matrix> matrixStack;
stack<int> stackSizeStoreStack;

 vector<Triangle> Triangles;


struct Vec normalizeVec(struct Vec v)
{
    Vec returnVec;
    double VecValue=sqrt(pow(v.x,2)+pow(v.y,2)+pow(v.z,2));
    returnVec.x=v.x/VecValue;
    returnVec.y=v.y/VecValue;
    returnVec.z=v.z/VecValue;
    return returnVec;




}



struct Vec crossMultiply(struct Vec v1,struct Vec v2)
{
    struct Vec crossProductVec;

    crossProductVec.x = v1.y * v2.z - v1.z * v2.y;
    crossProductVec.y = v1.z * v2.x - v1.x * v2.z;
    crossProductVec.z = v1.x * v2.y - v1.y * v2.x;

    return crossProductVec; 


}


double dotMultiply(struct Vec v1, struct Vec v2)
{

    double product = 0;
    product = product + (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);
    return product;
}


struct Vec rotateVec(struct Vec  axis,struct Vec v,double rotation_angle)
{
    Vec returnVec;
    double dotProduct=dotMultiply(axis,v);
   // cout<<"dot "<<dotProduct<<endl;
    Vec crossProduct=crossMultiply(v,axis);
 //   cout<<"cross P"<<endl;
   // printVec(crossProduct);

  //  cout<<"sxis Vec"<<endl;
  //  printVec(axis);

   // cout<<"a Vec normalized"<<endl;
  //  printVec(v);



   // cout<<"angle "<<cos(rotation_angle)<<endl;

    double s_theta=sin(rotation_angle);
    double c_theta=sqrt(1-pow(s_theta,2));
    double one_minus_c_theta=1-c_theta;

    returnVec.x=axis.x*c_theta+crossProduct.x*s_theta+(dotProduct)*v.x*one_minus_c_theta;
    returnVec.y=axis.y*c_theta+crossProduct.y*s_theta+(dotProduct)*v.y*one_minus_c_theta;
    returnVec.z=axis.z*c_theta+crossProduct.z*s_theta+(dotProduct)*v.z*one_minus_c_theta;
   // cout<<"returnVec Inside RotateVec"<<endl;
  //  printVec(returnVec);
    return returnVec; 

}


matrix generateIdentityMatrix()
{
    matrix I;
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            if(i==j)
            {

                I.arr[i][j]=1;
            }
            else
            {
                I.arr[i][j]=0;
            }
        }

    }
    return I;


}

matrix generateTranslationMatrix(double tx,double ty,double tz)
{
    matrix T=generateIdentityMatrix();
    T.arr[0][3]=tx;
    T.arr[1][3]=ty;
    T.arr[2][3]=tz;

    return T;



}
matrix generateScalingMatrix(double sx,double sy,double sz)
{
    matrix S=generateIdentityMatrix();
    S.arr[0][0]=sx;
    S.arr[1][1]=sy;
    S.arr[2][2]=sz;

    return S;



}

matrix generateRotationMatrix(double rotation_angle,double ax,double ay,double az)
{
    rotation_angle = rotation_angle * (pi/180);
    Vec a(ax,ay,az);
    Vec normlized_a=normalizeVec(a);
    matrix R=generateIdentityMatrix();
    Vec i_Vec(1,0,0),j_Vec(0,1,0),k_Vec(0,0,1);
    Vec c1=rotateVec(i_Vec,normlized_a,rotation_angle);
   // cout<<"C1"<<endl;
    //printVec(c1);
    Vec c2=rotateVec(j_Vec,normlized_a,rotation_angle);
   // cout<<"C2"<<endl;
    //printVec(c2);
    Vec c3=rotateVec(k_Vec,normlized_a,rotation_angle);
  //  cout<<"C3"<<endl;
    //printVec(c3);
    R.arr[0][0]=c1.x;
    R.arr[0][1]=c2.x;
    R.arr[0][2]=c3.x;

    R.arr[1][0]=c1.y;
    R.arr[1][1]=c2.y;
    R.arr[1][2]=c3.y;

    R.arr[2][0]=c1.z;
    R.arr[2][1]=c2.z;
    R.arr[2][2]=c3.z;

   // cout<<"inside genR"<<endl;

  //  printMatirx(R);

    return R;



}

struct point_matrix pointTranformationMatrixMultiply(struct matrix m1,struct point_matrix m2)
{
   // printMatirx(m1);
    struct point_matrix returnMatrix;
     for(int i=0; i<4; ++i)
     {
         for(int j=0; j<1; ++j)
         {
            returnMatrix.arr[i][j] = 0;
            for(int k=0; k<4; ++k) 
            {
               returnMatrix.arr[i][j]+=m1.arr[i][k]*m2.arr[k][j];
            }
         }
     }

     return returnMatrix;
}


struct matrix matrixMultiply(struct matrix m1,struct  matrix m2)
{
    struct matrix returnMatrix;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            returnMatrix.arr[i][j] = 0;
            for (int k = 0; k < 4; k++)
                returnMatrix.arr[i][j] += m1.arr[i][k] * m2.arr[k][j];
        }
    }
    return returnMatrix;

}


struct point transformPoint(struct point p,struct matrix m)
{
    struct point_matrix p_matrix(p);
   // printPointMatirx(p_matrix);
    struct point_matrix transformedPointMatrix=pointTranformationMatrixMultiply(m,p_matrix);
    //printPointMatirx(transformedPointMatrix);
    struct point transformedPoint=pointFromPointMatrix(transformedPointMatrix);

    transformedPoint.x/=transformedPointMatrix.arr[3][0];
    transformedPoint.y/=transformedPointMatrix.arr[3][0];
    transformedPoint.z/=transformedPointMatrix.arr[3][0];

    //printPoint(transformedPoint);

    return transformedPoint;

}

struct Vec VecAddition(struct Vec v1,struct Vec v2,int addOrSubtract)
{
    struct Vec returnVec;
    returnVec.x=v1.x+(addOrSubtract)*v2.x;
    returnVec.y=v1.y+(addOrSubtract)*v2.y;
    returnVec.z=v1.z+(addOrSubtract)*v2.z;
    return returnVec;


}

void printTriangle(Triangle T)
{
    for(int i=0;i<3;i++)
    {
        cout<<T.points[i].x<<" "<<T.points[i].y<<" "<<T.points[i].z<<endl;
    }
    for(int i=0;i<3;i++)
    {
        cout<<T.color[i]<<" ";
    }
    cout<<endl;
}

void readConfig()
{
    string fileLine;

//read screen width and height
    getline(config,fileLine);
    size_t pos = 0;
    pos = fileLine.find(" ");
    screen_width = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 
    pos = fileLine.find(" ");
    screen_height = stod(fileLine.substr(0,pos));

//read left and right limit of X
    getline(config,fileLine);
    pos = 0;
    pos = fileLine.find(" ");
    left_limit_of_X = stod(fileLine.substr(0,pos));
    right_limit_of_X=-left_limit_of_X;


    getline(config,fileLine);
    pos = 0;
    pos = fileLine.find(" ");
    bottom_limit_of_Y = stod(fileLine.substr(0,pos));
    top_limit_of_Y=-bottom_limit_of_Y;

//front and rear limits of Z

    getline(config,fileLine);
    pos = 0;
    pos = fileLine.find(" ");
    front_limit_of_Z = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 
    pos = fileLine.find(" ");
    rear_limit_of_Z = stod(fileLine.substr(0,pos));

    dx = (right_limit_of_X - left_limit_of_X) / screen_width;
    dy = (top_limit_of_Y - bottom_limit_of_Y) /screen_height;
   // cout<<"dx "<<dx<<endl;
   // cout<<"dy "<<dy<<endl;
    
    Top_Y = top_limit_of_Y - dy/2;
    Bottom_Y = bottom_limit_of_Y + dy/2;
    Left_X = left_limit_of_X + dx/2;
    Right_X = right_limit_of_X - dx/2;
    zMax=rear_limit_of_Z-front_limit_of_Z;

    // cout<<"Top Y"<<Top_Y<<endl;
    //  cout<<"Bottom Y"<<Bottom_Y<<endl;
    //  cout<<"left x"<<Left_X<<endl;
    //  cout<<"right x"<<Right_X<<endl;


}

void Zbuffer_init()
{
    zbuffer = new double*[(int)screen_height];

    for(int i=0;i<screen_height;i++)
    {
        zbuffer[i] = new double[(int)screen_width];
    }

    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            zbuffer[i][j] = rear_limit_of_Z;
        }
    }
   
}


void frame_buffer_init()
{

    cout<<"bal2"<<endl;
    frame_buffer=new Color*[(int)screen_height];

    cout<<"bal3"<<endl;

    for(int i=0;i<(int)screen_height;i++)
    {
        frame_buffer[i] = new Color[(int)screen_width];
    }

    cout<<screen_height<<screen_width<<endl;

    for(int i=0;i<(int)screen_height;i++)
    {
        for(int j=0;j<(int)screen_width;j++)
        {
           frame_buffer[i][j].R=0;
           frame_buffer[i][j].G=0;
           frame_buffer[i][j].B=0;
         //  cout<<i<<endl;

        }
    }

    cout<<"bal6"<<endl;

}



double getTriangleMax_XCoordinate(struct Triangle triangle)
{
    double a =triangle.points[0].x;
    double b=triangle.points[1].x;
    double c=triangle.points[2].x;
    if(a>=b && a>=c)
    {
        return a;
    }
    else if (b>=a && b>=c)
    {
        return b;
    }
    else return c;
    
}

double getTriangleMax_YCoordinate(struct Triangle triangle)
{
    double a =triangle.points[0].y;
    double b=triangle.points[1].y;
    double c=triangle.points[2].y;
    if(a>=b && a>=c)
    {
        return a;
    }
    else if (b>=a && b>=c)
    {
        return b;
    }
    else return c;
    
}

double getTriangleMin_XCoordinate(struct Triangle triangle)
{
    double a =triangle.points[0].x;
    double b=triangle.points[1].x;
    double c=triangle.points[2].x;
    if(a<=b && a<=c)
    {
        return a;
    }
    else if (b<=a && b<=c)
    {
        return b;
    }
    else return c;
    
}

double getTriangleMin_YCoordinate(struct Triangle triangle)
{
    double a =triangle.points[0].y;
    double b=triangle.points[1].y;
    double c=triangle.points[2].y;
    if(a<=b && a<=c)
    {
        return a;
    }
    else if (b<=a && b<=c)
    {
        return b;
    }
    else return c;
    
}

double ScanLineTriangleSideIntersect_X(struct Triangle T,int point1,int point2)
{
        double x1 = T.points[point1].x;
        double y1 = T.points[point1].y;

        double x2 = T.points[point2].x;
        double y2 = T.points[point2].y;

        if(y1 == y2) return getTriangleMax_XCoordinate(T)+10;

        double ans = x1 + ((s_line-y1) / (y1-y2)) * (x1-x2);

        return ans;
}


    double ScanLineTriangleSideIntersect_Z(struct Triangle T,int point1,int point2)
    {
        double z1 = T.points[point1].z;
        double y1 = T.points[point1].y;

        double z2 = T.points[point2].z;
        double y2 = T.points[point2].y;

        if(y1 == y2)  return 0;


        return  z1 + ((s_line-y1) / (y1-y2)) * (z1-z2);
    }


void setColumnBoundaries(double xa,double xb,double xc,double za,double zb,double zc)
{
     if( xa>max_x_boundary || xa<min_x_boundary)
     {
         if(xc < xb)
        {
            left_column = xc;
            right_column = xb;

            left_column_z = zc;
            right_column_z = zb;
        }
        else
        {
            left_column = xb;
            right_column = xc;

            left_column_z = zb;
            right_column_z = zc;
        }

     }

     else if( xb>max_x_boundary || xb<min_x_boundary)
     {
         if(xa < xc)
        {
            left_column = xa;
            right_column = xc;

            left_column_z = za;
            right_column_z = zc;
        }
        else
        {
            left_column = xc;
            right_column = xa;

            left_column_z = zc;
            right_column_z = za;
        }

     }

     else 
     {
         if(xa < xb)
        {
            left_column = xa;
            right_column = xb;

            left_column_z = za;
            right_column_z = zb;
        }
        else
        {
            left_column = xb;
            right_column = xa;

            left_column_z = zb;
            right_column_z = za;
        }

     }


}



void processTriangle()
{
     for(auto i = begin(Triangles); i  != end(Triangles); i++){
        
        Triangle t=*i;
       // cout<<"max_x "<<getTriangleMax_XCoordinate(t)<<" min_x "<<getTriangleMin_XCoordinate(t)<<" max_y "<<getTriangleMax_YCoordinate(t)<<" min_y "<<getTriangleMin_YCoordinate(t)<<endl;

         max_x_boundary=getTriangleMax_XCoordinate(t);
         max_y_boundary=getTriangleMax_YCoordinate(t);
         min_x_boundary=getTriangleMin_XCoordinate(t);
         min_y_boundary=getTriangleMin_YCoordinate(t);

       // cout<<"max_x "<<max_x_boundary<<" min_x "<<min_x_boundary<<" max_y "<<max_y_boundary<<" min_y "<<min_y_boundary<<endl;


       
        if(max_x_boundary < Top_Y)
        {
            triangle_start_row = ceil((Top_Y - max_y_boundary)/dy);
            

            triangle_top_scaline = Top_Y - (triangle_start_row * dy);

        }
        else
        {
            triangle_start_row=0;
            triangle_top_scaline=Top_Y;
        }

     

        if(min_y_boundary >Bottom_Y )
        {
            
            triangle_end_row= floor((Top_Y - min_y_boundary)/dy);

            triangle_bottom_scaline = Top_Y -(triangle_end_row * dy);
        }
        else
        {
            triangle_end_row=screen_height-1;
            triangle_bottom_scaline=Bottom_Y;
        }

        // cout<<"Triangle start row "<<triangle_start_row<<endl;
        // cout<<"Triangle end row "<<triangle_end_row<<endl;
        // cout<<"Triangle top_scaline "<<triangle_top_scaline<<endl;
        // cout<<"Triangle bottom_scaline "<<triangle_bottom_scaline<<endl;

        for(int row=triangle_start_row;row<=triangle_end_row;row++)
        {
            s_line=Top_Y - (row * dy);
           // cout<<"scan line "<<s_line<<endl;

            double xa = ScanLineTriangleSideIntersect_X(t, 0,1);
            double xb = ScanLineTriangleSideIntersect_X(t, 0,2);
            double xc = ScanLineTriangleSideIntersect_X(t, 1,2);

            double za =ScanLineTriangleSideIntersect_Z(t,0,1);
            double zb = ScanLineTriangleSideIntersect_Z(t,0,2);
            double zc = ScanLineTriangleSideIntersect_Z(t,1,2);

           // cout<<"xa "<<xa<<" xb "<<xb<<" xc "<<xc<<endl;
           // cout<<"za "<<za<<" zb "<<zb<<" zc "<<zc<<endl;

            setColumnBoundaries(xa,xb,xc,za,zb,zc);

            

           

        //   cout<<"left column "<<left_column<<" right column "<<right_column<<endl;
        //   cout<<"left column Z "<<left_column_z<<" right column Z "<<right_column_z<<endl;

    

          //  cout<<"left column "<<left_column<<endl;
          //  cout<<"left X "<<Left_X<<endl;

            // cout<<"right column "<<right_column<<endl;
         //   cout<<"right X "<<Right_X<<endl;

            if(left_column > Left_X)
            {
                triangle_start_column = round((left_column - Left_X)/dx);
            }
            else
            {
                triangle_start_column=0;
            }

            if(right_column < Right_X)
            {
                triangle_end_column = round((right_column - Left_X)/dx);
            }
            else
            {
                triangle_end_column=screen_width - 1;
            }

        //    cout<<"Triangle start column "<<triangle_start_column<<endl;
        //    cout<<"Triangle end column "<<triangle_end_column<<endl;

            for(int column=triangle_start_column;column<=triangle_end_column;column++)
            {
                //je column e  asi ekhn setar x coordinate calcul;ation er jnne
                current_column = Left_X + (column * dx);
               // cout<<" current column "<<current_column<<endl;

              // cout<<"r c z "<<right_column_z<<" l c z "<<left_column_z<<endl;
             //  cout<<"r c "<<right_column<<" l c  "<<left_column<<endl;
             //  cout<<"current col "<<current_column<<endl;
                double current_pixel_z = right_column_z -
                           ((right_column_z - left_column_z) / (right_column - left_column)) * (right_column - current_column);

               // cout<<"current pixel z "<<current_pixel_z<<endl;

                if(current_pixel_z >= front_limit_of_Z && current_pixel_z < zbuffer[row][column])
                {
                    //cout<<"jeta bosabo "<<current_pixel_z<<endl; 
                    zbuffer[row][column] = current_pixel_z;
                    frame_buffer[row][column].R=t.color[0];
                    frame_buffer[row][column].G=t.color[1];
                    frame_buffer[row][column].B=t.color[2];

                }

                
           }
      

        }

     }

    


}

void generateImage()
{
    bitmap_image image(screen_width,screen_height);
    for(int i=0;i<screen_height;i++)
    {
        for(int j=0;j<screen_width;j++)
        {
            image.set_pixel(j, i, frame_buffer[i][j].R, frame_buffer[i][j].G, frame_buffer[i][j].B);
        }
    }

    image.save_image("output.bmp");

}




int main()
{
   
    matrix I=generateIdentityMatrix();
    matrixStack.push(I);
    string command;
   // skipNFileLines(4);
    string fileLine;


    getline(in,fileLine);
    size_t pos = 0;
    pos = fileLine.find(" ");
    double eyex = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double eyey = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double eyez = stod(fileLine.substr(0,pos));

    Vec eyeVec(eyex,eyey,eyez);

    getline(in,fileLine);
    pos = fileLine.find(" ");
    double lookx = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double looky = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double lookz = stod(fileLine.substr(0,pos));

    Vec lookVec(lookx,looky,lookz);


    getline(in,fileLine);
    pos = fileLine.find(" ");
    double upx = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double upy = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double upz = stod(fileLine.substr(0,pos));

    Vec upVec(upx,upy,upz);   


    // printVec(eyeVec);
    // printVec(lookVec);
    // printVec(upVec); 
    
    Vec l=VecAddition(lookVec,eyeVec,-1);
    l=normalizeVec(l);
    Vec r=crossMultiply(l,upVec);
    r=normalizeVec(r);
    Vec u=crossMultiply(r,l);

    struct matrix T=generateIdentityMatrix();
    T.arr[0][3]=-eyex;
    T.arr[1][3]=-eyey;
    T.arr[2][3]=-eyez;

    struct matrix R;
    R=generateIdentityMatrix();
   // double arr[4][4]={{r.x,r.y,r.z,0},{u.x,u.y,u.z,0},{-l.x,-l.y,-l.z,0},{0,0,0,1}};
    //R.arr=arr;
    R.arr[0][0]=r.x;
    R.arr[0][1]=r.y;
    R.arr[0][2]=r.z;

    R.arr[1][0]=u.x;
    R.arr[1][1]=u.y;
    R.arr[1][2]=u.z;

    R.arr[2][0]=-l.x;
    R.arr[2][1]=-l.y;
    R.arr[2][2]=-l.z;

    struct matrix V=matrixMultiply(R,T); 


    getline(in,fileLine);
    pos = fileLine.find(" ");
    double fovY = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double aspectratio = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double near = stod(fileLine.substr(0,pos));
    fileLine.erase(0, pos + 1); 

    pos = fileLine.find(" ");
    double far = stod(fileLine.substr(0,pos));

   // cout<<fovY<<" "<<aspectratio<<" "<<near<<" "<<far;
    double fovX=fovY*aspectratio;
    double t = near * tan((fovY/2) * (pi/180));
    double rr = near * tan((fovX/2) * (pi/180));

    struct matrix P;//projection matrix

    P.arr[0][0]=near/rr;
    P.arr[0][1]=0;
    P.arr[0][2]=0;
    P.arr[0][3]=0;

    P.arr[1][0]=0;
    P.arr[1][1]=near/t;
    P.arr[1][2]=0;
    P.arr[1][3]=0;

    P.arr[2][0]=0;
    P.arr[2][1]=0;
    P.arr[2][2]=-(far+near)/(far-near);
    P.arr[2][3]=-(2*far*near)/(far-near);

    P.arr[3][0]=0;
    P.arr[3][1]=0;
    P.arr[3][2]=-1;
    P.arr[3][3]=0;

    //printMatirx(P);

    

    


    while (true)
   {
        
        getline(in,command);
        if(command=="triangle")
        {
          //  stage1Output<<"bal1"<<endl;
            string fileLine;

            struct point trianglePoints[3],stage1TransformedTrianglePoints[3],stage2TransformedTrianglePoints[3],stage3TransformedTrianglePoints[3];
            for(int i=0;i<3;i++)
            {

                getline(in,fileLine);
                size_t pos = 0;
                pos = fileLine.find(" ");
                double x = stod(fileLine.substr(0,pos));
                fileLine.erase(0, pos + 1); 

                pos = fileLine.find(" ");
                double y = stod(fileLine.substr(0,pos));
                fileLine.erase(0, pos + 1); 

                pos = fileLine.find(" ");
                double z = stod(fileLine.substr(0,pos));

                trianglePoints[i].x=x;
                trianglePoints[i].y=y;
                trianglePoints[i].z=z;
            


            }



            for(int i=0;i<3;i++)
            {
               /// struct matrix m=matrixStack.top();
               // printMatirx(m);
          
                stage1TransformedTrianglePoints[i]=transformPoint(trianglePoints[i],matrixStack.top());
                stage1Output<<stage1TransformedTrianglePoints[i].x<<" ";
                stage1Output<<stage1TransformedTrianglePoints[i].y<<" ";
                stage1Output<<stage1TransformedTrianglePoints[i].z<<" ";
                stage1Output<<endl;
              
                 

            }
            stage1Output<<endl;
            for(int i=0;i<3;i++)
            {
               /// struct matrix m=matrixStack.top();
               // printMatirx(m);
          
                stage2TransformedTrianglePoints[i]=transformPoint(stage1TransformedTrianglePoints[i],V);
                stage2Output<<stage2TransformedTrianglePoints[i].x<<" ";
                stage2Output<<stage2TransformedTrianglePoints[i].y<<" ";
                stage2Output<<stage2TransformedTrianglePoints[i].z<<" ";
                stage2Output<<endl;
              
                 

            }
            stage2Output<<endl;  

          //  point points[3];
            struct Triangle T;

            for(int i=0;i<3;i++)
            {
               /// struct matrix m=matrixStack.top();
              // printMatirx(P);
        
               // cout<<"stage 2 point\n";
                //printPoint(stage2TransformedTrianglePoints[i]);
          
                stage3TransformedTrianglePoints[i]=transformPoint(stage2TransformedTrianglePoints[i],P);
                stage3Output<<stage3TransformedTrianglePoints[i].x<<" ";
                stage3Output<<stage3TransformedTrianglePoints[i].y<<" ";
                stage3Output<<stage3TransformedTrianglePoints[i].z<<" ";
                stage3Output<<endl;
              
                point p(stage3TransformedTrianglePoints[i].x,stage3TransformedTrianglePoints[i].y,stage3TransformedTrianglePoints[i].z);
                T.points[i]=p;
                T.color[i]=(rand()*i)%255;


            }
            Triangles.push_back(T);
            stage3Output<<endl;         


        }

        else if(command=="translate")
        {
           // stage1Output<<"bal2"<<endl;
            string fileLine;

            struct point translatePoints[3],transformedTranslatePoints[3];
            
     

            getline(in,fileLine);
            size_t pos = 0;
            pos = fileLine.find(" ");
            double x = stod(fileLine.substr(0,pos));
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double y = stod(fileLine.substr(0,pos));
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double z = stod(fileLine.substr(0,pos));

            struct matrix T=generateTranslationMatrix(x,y,z);
            matrixStack.push(matrixMultiply(matrixStack.top(),T));


        }

        else if(command=="scale")
        {
           // stage1Output<<"bal3"<<endl;
            string fileLine;

            struct point scalePoints[3],transformedScalePoints[3];
    

            getline(in,fileLine);
            size_t pos = 0;
            pos = fileLine.find(" ");
            double x = stod(fileLine.substr(0,pos));
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double y = stod(fileLine.substr(0,pos));
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double z = stod(fileLine.substr(0,pos));
            struct matrix S=generateScalingMatrix(x,y,z);
            matrixStack.push(matrixMultiply(matrixStack.top(),S));


        }


        else if(command=="rotate")
        {
           // stage1Output<<"bal4"<<endl;
            string fileLine;

        
            getline(in,fileLine);
            size_t pos = 0;
            pos = fileLine.find(" ");
            double angle = stod(fileLine.substr(0,pos));
           // cout<<angle<<endl;
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double x = stod(fileLine.substr(0,pos));
           // cout<<x<<endl;
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double y = stod(fileLine.substr(0,pos));
           // cout<<y<<endl;
            fileLine.erase(0, pos + 1); 

            pos = fileLine.find(" ");
            double z = stod(fileLine.substr(0,pos));
          //  cout<<z<<endl;

           // stage1Output<<"bal5"<<endl;

            struct matrix R=generateRotationMatrix(angle,x,y,z);
           // cout<<"Rotation matrix";
           // printMatirx(R);
            matrixStack.push(matrixMultiply(matrixStack.top(),R));



        }

        else if(command=="push")
        {
          //  stage1Output<<"bal push"<<endl;
            stackSizeStoreStack.push(matrixStack.size());   
        }

        else if(command=="pop")
        {
           // stage1Output<<"bal pop"<<endl;
            int popped_length=stackSizeStoreStack.top();
            stackSizeStoreStack.pop();
            while(matrixStack.size()!=popped_length)
            {
                matrixStack.pop();
            }

        }

        else if(command=="end")
        {
            break;
        }
   }



readConfig();
Zbuffer_init();
frame_buffer_init();
processTriangle();
printBuffer();
generateImage();



return 0;

}

