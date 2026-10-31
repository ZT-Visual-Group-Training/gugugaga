#include "students.hpp"
#include <iostream>

Student::Student(std::string name, int id, double score)
    : m_name(name), m_id(id), m_score(score)
{
}

void Student::showInfo() const
{
    std::cout << "====学生信息====" << std::endl;
    std::cout << "姓名：" << m_name << std::endl;
    std::cout << "学号：" << m_id << std::endl;
    std::cout << "成绩：" << m_score << std::endl;
}

void Student::setScore(double newScore)
{
    m_score = newScore;
}

double Student::getScore() const
{
    return m_score;
}
