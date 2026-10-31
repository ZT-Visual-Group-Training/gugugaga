#include "student.hpp"
#include <iostream>
using namespace std;

//构造函数初始化列表
Student::Student(const string& name, int id, double score)
    : name_(name), id_(id), score_(score) {}

    //函数的定义
void Student::printInfo() const {
    cout << "姓名: " << name_
              << ", 学号: " << id_
              << ", 成绩: " << score_ << endl;
}

void Student::setScore(double newScore) {
    score_ = newScore;
}

double Student::getScore() const {
    return score_;
}