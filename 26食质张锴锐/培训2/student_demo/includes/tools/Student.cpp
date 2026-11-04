#include "Student.hpp"
#include <iostream>

student::student(std::string name, int age, int id)
    : name_(name), age_(age), id_(id), score_(0) {}

void student::setScore(int score) {
    score_ = score;
}

void student::showInfo() const {
    std::cout << "Name: " << name_
              << ", Age: " << age_
              << ", ID: " << id_
              << ", Score: " << score_ << std::endl;
}
