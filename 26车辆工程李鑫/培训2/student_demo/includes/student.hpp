#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>
#include <iostream>

class Student {
private:
    std::string name;
    int id;
    double score;

public:
    Student(std::string studentName, int studentId, double studentScore);
    void printInfo() const;
    void modifyScore(double newScore);
};

#endif
