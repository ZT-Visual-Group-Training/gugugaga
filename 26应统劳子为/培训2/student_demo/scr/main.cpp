#include "students.hpp"
#include <iostream>

int main()
{
    Student student("张三", 2026001, 88.5);
    student.showInfo();

    student.setScore(95.0);
    std::cout << "\n修改成绩后：" << std::endl;
    student.showInfo();

    return 0;
}
