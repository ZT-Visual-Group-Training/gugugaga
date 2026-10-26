#include "Student.hpp"
#include <iostream>

Student::Student(std::string name, int id,double score) : name_(name), id_(id), score_(score) {}

void Student::printInfo()  
{
    std::cout << "姓名: " << name_ << "\n"
              << "学号: " << id_ << "\n"
              << "成绩: " << score_ << "\n";
}

void Student::setScore(double score) {
    score_ = score;
}