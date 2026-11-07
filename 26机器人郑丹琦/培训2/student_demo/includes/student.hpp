#pragma once
#include <string>

class Student {
public:
Student();
Student(const std::string& name, int id, double score);
void print() const;
void setScore(double score);
double getScore() const;
private:
std::string name_;
int id_;
double score_;
};
