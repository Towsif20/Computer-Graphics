#include <iostream>
#include <bits/stdc++.h>
#include<cmath>
#include <vector>
#include "bitmap_image.hpp"
using namespace std;



class Point{

public:
    double x;
    double y;
    double z;

    Point(double px, double py, double pz){
        x=px;
        y=py;
        z=pz;
    }

    Point(){
        x=y=z=0;
    }

    void setValue(double px, double py, double pz){
        x=px;
        y=py;
        z=pz;
    }

    void printPoint(){

        printf("( %lf %lf %lf ) \n",x,y,z);

    }



    Point unitVector(){
        Point result;

        double mod= sqrt( x*x+y*y+z*z );

        if(mod==0){
            mod=1;
        }

        result.x=(x*1.0)/mod;
        result.y=(y*1.0)/mod;
        result.z=(z*1.0)/mod;

        return result;
    }

    Point operator+(Point obj)
    {
        Point result;
        result.x=x + obj.x;
        result.y=y + obj.y;
        result.z=z + obj.z;
        return result;
    }

    Point operator-(Point obj)
    {
        Point result;
        result.x=x - obj.x;
        result.y=y - obj.y;
        result.z=z - obj.z;
        return result;
    }

    Point multiplyScaler (double val)
    {
        Point result;
        result.x=x * val;
        result.y=y * val;
        result.z=z * val;
        return result;
    }

    double dotProduct (Point obj){

        double result= x*obj.x+y*obj.y+z*obj.z;
        return result;
    }

    Point crossProduct (Point obj){
        Point result( y*obj.z - z*obj.y, z*obj.x - x*obj.z, x*obj.y - y*obj.x );
        return result;
    }





};




class RGB{

public:
    double red;
    double green;
    double blue;

    RGB(){
        red=0;
        green=0;
        blue=0;
    }

    RGB(double r, double g, double b){
        red = r ;
        green = g ;
        blue = b ;
    }

    void setValue(double r, double g, double b){
        red = r ;
        green = g ;
        blue = b ;
    }

    void printRGB(){
        printf("rgb ( %lf %lf %lf )\n",red,green,blue);
    }


    RGB operator+(RGB obj)
    {
        RGB result;
        result.red=red + obj.red;
        result.blue=blue + obj.blue;
        result.green=green + obj.green;
        return result;
    }

    RGB operator*(RGB obj)
    {
        RGB result;
        result.red=red * obj.red;
        result.blue=blue * obj.blue;
        result.green=green * obj.green;
        return result;
    }


    RGB multiplyScaler (double val)
    {
        RGB result;
        result.red  =red * val;
        result.green=green * val;
        result.blue =blue * val;
        return result;
    }

};


class Light
{
    public:

    Point light_pos;
    RGB color;

    Light(){

    }

    Light(Point l){
        light_pos = l;
    }

    /*
    void setLightPos(Point p)
    {
        light_pos.x = p.x;
        light_pos.y = p.y;
        light_pos.z = p.z;
    }
    */

    /*
    void setColor(double r, double g, double b)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }
    */

    void draw()
    {
        // change
        glPointSize(5);
        glColor3f(color.red , color.green, color.blue);
        glBegin(GL_POINTS);
        glVertex3f(light_pos.x , light_pos.y , light_pos.z);
        glEnd();
    }

    void print()
    {
        cout<<"Light position:";
        light_pos.printPoint();
        color.printRGB();
    }
};



class Ray{
public:
    Point start;
    Point dir;

    Ray(Point st, Point dr){
        start = st;
        dir = dr;

        dir.normalize();
    }

};


class Object
{
public:
    Point reference_point;
    double height, width, length;
    RGB color;
    double coEfficients[4];
    int shine;

    object(){}
    virtual void draw(){}
    virtual void print(){}

    virtual double intersect(Ray r, double *color, int level)
    {
        return -1.0;
    }


    void setCoEfficients(double ambient, double diffuse, double specular, double recusiveReflectionLCofficient){
        coEfficients[0] = ambient ;
        coEfficients[1] = diffuse ;
        coEfficients[2] = specular ;
        coEfficients[3] = recusiveReflectionLCofficient ;
    }

    void setColor(double red, double green, double blue){
        color.red = red ;
        color.blue = green ;
        color.green = blue ;
    }

    void setShine(int s){
        shine = s;
    }



    void printObject(){
        cout<<"print Reference Point: ";
        reference_point.printPoint();

        printf(" length: %lf\n width: %lf\n height: %lf \n",length,width,height);

        color.printRGB();

        printf("ambient=%lf\n",coEfficients[0]);
        printf("diffuse=%lf\n",coEfficients[1]);
        printf("specular=%lf\n",coEfficients[2]);
        printf("recusive ReflectionL Cofficient=%lf\n",coEfficients[3]);

        printf("shininess=%d\n",shine);

    }

};





class Sphere : public Object{
public:

    Sphere(Point c, double r){

        reference_point=c;
        length = r;
    }

    void draw(){

        glPushMatrix();
        glColor3f(color.red, color.green, color.blue);
        glTranslated(reference_point.x, reference_point.y , reference_point.z);
        glutSolidSphere(length, 80, 80);
        glPopMatrix();

    }

    Point unitVector(Point point){
        Point result( point.x-reference_point.x, point.y-reference_point.y , point.z-reference_point.z );
        result=result.unitVector();
        return result;
    }


    double intersect(Ray ray, double *color, int level){

        double a = ray.dir.dotProduct(ray.dir) ;
        double b = (ray.start-reference_point).dotProduct(ray.dir)*2 ;
        double c = (ray.start-reference_point).dotProduct(ray.start-reference_point)-length*length;

        double d = b*b - 4*a*c;

        double res = -1;
        double sq=sqrt(d);

        if(d<0) return -1;

        if( (-b- sq) > 0)
            res = (-b- sq) / (2*a);

        else
        {
            if( (-b + sq) > 0)
                res = (-b + sq) / (2.0 * a);
            else
                return -1;
        }

        if(level == 0)
            return res;

        Point intersectPoint= ray.start + ray.dir.multiplyScaler(res) ;


        RGB passedColor;

        passedColor= color.multiplyScaler(coEfficients[0]);


        Point normal=unitVector(intersectPoint)

        for(int i=0;i<lights.size();i++)
        {
            Ray light_Point( lights[i].light_pos, intersectPoint-lights[i].light_pos);

            bool obscured = false;
            RGB temp1;
            for(int j=0;j<objects.size();j++)
            {
                Object *obj = objects[i];
                double temp = (*obj).intersect(light_Point, temp1, 0);
                if( floor((intersectPoint.x - light_Point.start.x)/light_Point.dir.x) > floor(temp) && temp > 0 )
                {
                    obscured = true;
                    break;
                }
            }


            if(!obscured){

                double lambertValue = normal.dotProduct(light_Point.dir);
                double phong = max(0, -light_Point.dir-normal.multiplyScaler(lambertValue*2).dotProduct(ray.dir) )  ;
                passedColor = passedColor + lights[i].color*color.multiplyScaler(coEfficients[1]).multiplyScaler(lambertValue) ;
                passedColor = passedColor + lights[i].color*color.multiplyScaler(coEfficients[2]).multiplyScaler( pow( (phong),shine) ) ;

            }


        }

        color = passedColor;

        double lambertValue = normal.dotProduct(ray.dir);
        Point direct = ray.dir-normal.multiplyScaler(lambertValue*2);

        if( level_of_recursion>level){

            int nearest = -1;
            int tMin = 1000000;
            RGB afterColor;
            RGB temp4;

            Ray refRay( intersectPoint+direct , direct );


            for(int i=0;i<objects.size();i++)
            {
                Object *obj=objects[i];
                double temp = (*obj).intersect(refRay, temp4, 0);

                if(temp < tMin && temp > 0)
                {
                    tMin = temp;
                    nearest = i;
                }
            }

            if(nearest != -1)
            {
                tMin = objects[nearest]->intersect(refRay, afterColor, level + 1);

                color=color+afterColor.multiplyScaler(coEfficients[3])
            }


            return res;


        }else{
            return res;
        }

    }

    void print()
    {
        reference_point.print();
        cout<<length<<endl;
        printBasic();
    }
};





/*
class OtherComponent{
public:
    double ambient;
    double diffuse;
    double specular;
    double recusiveReflectionLCofficient;
    int shininess;

    OtherComponent(){

    }

    void setValue(double a, double d, double sp,double r ,int sh){
        ambient=a;
        diffuse=d;
        specular=sp;
        recusiveReflectionLCofficient=r;
        shininess=sh;
    }

    void printOther(){
        printf("ambient=%lf\n",ambient);
        printf("diffuse=%lf\n",diffuse);
        printf("specular=%lf\n",specular);
        printf("recusive ReflectionL Cofficient=%lf\n",recusiveReflectionLCofficient);
        printf("shininess=%d\n",shininess);
    }


};





class Triangle{
public:
    Point p[3];
    RGB color;
    OtherComponent otherComponent;

    Triangle(){

    }

    Triangle(Point pa, Point pb, Point pc, RGB col, double a, double d, double sp,double r ,int sh  ){
        p[0]=pa;
        p[1]=pb;
        p[2]=pc;
        color=col;
        otherComponent.setValue(a,d,sp,r,sh);
    }

    void printTriangle(){
        cout<<"printing triangle"<<endl;
        p[0].printPoint();
        p[1].printPoint();
        p[2].printPoint();
        color.printRGB();
        otherComponent.printOther();
        cout<<endl;
    }

};


class Sphere{
public:
    Point center;
    double radius;
    RGB color;
    OtherComponent otherComponent;

    Sphere(){

    }

    Sphere(Point pc,double rad, RGB col, double a, double d, double sp,double r ,int sh  ){
        center=pc;
        radius=rad;
        color=col;
        otherComponent.setValue(a,d,sp,r,sh);
    }

    void printSphere(){
        cout<<"printing Sphere"<<endl;
        cout<<"center: ";
        center.printPoint();
        printf("radius: %lf\n",radius);
        color.printRGB();
        otherComponent.printOther();
        cout<<endl;
    }

};


class General{
public:
    double coefficent[10];
    Point refPoint;
    double length, width,height;
    RGB color;
    OtherComponent otherComponent;

    General(){

    }

    General(double co[],Point rP, double len,double wid,double hei,RGB col, double a, double d, double sp,double r ,int sh  ){
        for(int i=0;i<10;i++){
            coefficent[i]=co[i];
        }
        refPoint= rP;
        length=len;
        width=wid;
        height=hei;
        color=col;
        otherComponent.setValue(a,d,sp,r,sh);
    }

    void printGeneral(){
        cout<<"printing General"<<endl;
        cout<<" ( ";
        for(int i=0;i<10;i++){
            cout<<coefficent[i]<<" ";
        }

        cout<<" ) "<<endl;
        cout<<"ref Point: ";
        refPoint.printPoint();
        printf(" length: %lf\n width: %lf\n height: %lf \n",length,width,height);
        color.printRGB();
        otherComponent.printOther();
        cout<<endl;
    }

};

class LightSource{

public:
    Point position;
    RGB color;

    LightSource(){

    }

    LightSource(Point p, RGB c){
        position=p;
        color=c;
    }

    void printLightSource(){
        cout<<"print LightSource: "<<endl;
        cout<<"position: ";
        position.printPoint();
        cout<<"light: ";
        color.printRGB();
    }
};

-*

