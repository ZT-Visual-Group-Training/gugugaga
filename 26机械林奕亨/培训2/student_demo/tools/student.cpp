#include "student.hpp"

Student::Student(std::string n, int i, double s) : name(n), id(i), score(s) {}

void Student::printInfo() const {
    std::cout << "姓名: " << name << ", 学号: " << id << ", 成绩: " << score << std::endl;
}

void Student::setScore(double s) {
    score = s;
}
