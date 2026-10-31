#include "student.hpp"

Student::Student(std::string studentName, int studentId, double studentScore) 
    : name(studentName), id(studentId), score(studentScore) {}

void Student::printInfo() const {
    std::cout << "--- 学生信息 ---" << std::endl;
    std::cout << "姓名: " << name << "\n学号: " << id << "\n成绩: " << score << std::endl;
}

void Student::modifyScore(double newScore) {
    score = newScore;
    std::cout << name << " 的成绩已修改为: " << score << std::endl;
}
