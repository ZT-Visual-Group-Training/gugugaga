#include "student.hpp"
#include <iostream>

Student::Student() : name_(""), id_(0), score_(0.0) {}

Student::Student(const std::string& name, int id, double score)
: name_(name), id_(id), score_(score) {}

void Student::print() const {
std::cout << "name: " << name_ << ", id: " << id_ << ", score: " << score_ << std::endl;
}

void Student::setScore(double score) {
score_ = score;
}

double Student::getScore() const {
return score_;
}
