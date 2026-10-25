//student.cpp
#include <iostream>
#include <string>
#include "student.hpp"


void Student::showstudent()
    {
        std::cout << "姓名:" << name << std::endl;
        std::cout << "学号" << ID << std::endl;
        std::cout << "成绩" << score << std::endl;
    }

void Student::changescore()
    {
        double s = 0;
        std::cout << "请输入修改后成绩:";
        std::cin >> s;
        if (s >= 0 && s <= 100)
        {
            score = s;
            std::cout << "修改成功" << std::endl;
        } else{
            std::cout << "输入成绩不合法， 修改失败" << std::endl;
        }
    }