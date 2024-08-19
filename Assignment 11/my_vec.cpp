#include "my_vec.h"

std::ostream& operator<<(std::ostream& os, my_vec& vec)
{
    os<<'('<<vec._x<<" ,"<<vec._y<<" ,"<<vec._z<<')';
    return os;
}

my_vec& my_vec::operator+(const my_vec& vec)
{
    my_vec* sum = new my_vec;
    *sum = my_vec(this->_x + vec._x, this->_y + vec._y, this->_z + vec._z);
    return *sum;
}

my_vec& my_vec::operator-(const my_vec& vec)
{
    my_vec* deduct = new my_vec;
    *deduct = my_vec(this->_x - vec._x, this->_y - vec._y, this->_z - vec._z);
    return *deduct;
}

my_vec& my_vec::operator*(const double num)
{
    my_vec* mult = new my_vec;
    *mult = my_vec(this->_x * num, this->_y * num, this->_z * num);
    return *mult;
}

my_vec& operator*(const double num, my_vec& vec)
{
    my_vec* mult = new my_vec;
    *mult = my_vec(vec._x * num, vec._y * num, vec._z * num);
    return *mult;
}

double my_vec::operator*(const my_vec& vec)
{
    return (this->_x * vec._x) + (this->_y * vec._y) + (this->_z * vec._z);
}


int main()
{
    my_vec a(1, -4, 6);
    my_vec b(0, -8, 6);
    my_vec z;
    std::cout<<"a + b is: "<<a+b<<std::endl;
    z.setY(14);
    std::cout<<"b * 8 is: "<<b*8<<std::endl;
    std::cout<<"but z * b is: "<<z*b<<std::endl;

    return 0;
}