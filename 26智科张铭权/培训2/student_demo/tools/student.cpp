#include "../includes/student.hpp"
#include <string>
#include <iostream>
Student::Student(std::string n, double s) : name(n), score(s)
{
}
void Student::showInfo()
{
    std::cout << "姓名：" << name << " 成绩：" << score << std::endl;
}
    void Student::changeScore(double newScore)
{
    score = newScore;
    std::cout << "成绩修改完毕！" << std::endl;
}
