#include <iostream>
#include "Student.hpp"

int main()
{
    Student s("张三",1001,58);
    s.printInfo();

    s.setScore(99);
    std::cout<<"修改成绩后:\n";
    s.printInfo();

    return 0;

}