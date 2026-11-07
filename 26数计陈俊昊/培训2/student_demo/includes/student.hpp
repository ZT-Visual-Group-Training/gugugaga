//student.hpp
#pragma once
#include <iostream>
#include <string>


class Student
{
public:
    
    std::string name;
    std::string ID;
    double score;
    
    void showstudent();
    void changescore();
};