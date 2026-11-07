#ifndef STUDENT_H
#define STUDENT_H

#include <string>

class Student
{
private:
    std::string name_;
    int id_;
    double score_;

public:
    Student(std::string name,int id,double score);
    void printInfo();
    void setScore(double score);

};
#endif
