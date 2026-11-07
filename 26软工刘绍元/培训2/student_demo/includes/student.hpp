#pragma once
#include <string>

class Student {
  public:
    Student(const std::string &name, const std::string &studentId, double score);
    void printInfo() const;
    void setScore(double score);
    double getScore() const;

  private:
    std::string name_;
    std::string studentId_;
    double score_;
};
