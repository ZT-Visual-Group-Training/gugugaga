#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>

class Student
{
private:
    std::string m_name;
    int m_id;
    double m_score;

public:
    Student(std::string name, int id, double score);

    void showInfo() const;
    void setScore(double newScore);
    double getScore() const;
};

#endif
