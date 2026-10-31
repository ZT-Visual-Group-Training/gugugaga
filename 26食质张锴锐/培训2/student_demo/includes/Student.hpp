#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>

class student {
public:
    student(std::string name, int age, int id);
    void setScore(int score);
    void showInfo() const;

private:
    std::string name_;
    int age_;
    int id_;
    int score_;
};

#endif
