//main.cpp
#include <iostream>
#include "student.hpp"

int main()
{
    Student sb;
    sb.name = "sabi";
    sb.ID = "114514";
    sb.score = 0;
    sb.showstudent();
    sb.changescore();
}