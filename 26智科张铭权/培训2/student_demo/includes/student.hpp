#ifndef STUDENT_H
#define STUDENT_H
#include <iostream>
#include <string>
using namespace std;
class Student
{
    public:
    string name;
    double score;
    Student(string n, double s);
    void showInfo();
    void changeScore(double newScore);

};
#endif