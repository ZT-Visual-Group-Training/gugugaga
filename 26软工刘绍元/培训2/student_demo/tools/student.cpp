#include "student.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace std;

Student::Student(const string &name, const string &studentId, double score)
    : name_(name), studentId_(studentId), score_(0) {
    setScore(score);
}

void Student::printInfo() const {
    cout << "姓名=" << name_ << " 学号=" << studentId_ << " 成绩=" << score_ << '\n';
}

void Student::setScore(double score) {
    if (!isfinite(score) || score < 0 || score > 100)
        throw invalid_argument("成绩必须是0到100之间的有限数字");
    score_ = score;
}

double Student::getScore() const {
    return score_;
}
