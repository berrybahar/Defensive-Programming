#ifndef MY_VEC_H
#define MY_VEC_H

#include <iostream>

class my_vec
{
    double _x, _y, _z;
public:
    my_vec() :_x(0.0) ,_y(0.0) ,_z(0.0) {}
    my_vec(double x, double y, double z) :_x(x) ,_y(y) ,_z(z) {}
    my_vec(const my_vec& vec) :_x(vec._x) ,_y(vec._y) ,_z(vec._z) {}
    double getX() { return _x; }
    double getY() { return _y; }
    double getZ() { return _z; }
    void setX(double x) { _x = x; }
    void setY(double y) { _y = y; }
    void setZ(double z) { _z = z; }
    my_vec& operator+(const my_vec& vec);
    my_vec& operator-(const my_vec& vec);
    my_vec& operator*(const double num);
    double operator*(const my_vec& vec);
    friend std::ostream& operator<<(std::ostream& os, my_vec& vec);
    friend my_vec& operator*(const double num, my_vec& vec);
};


#endif