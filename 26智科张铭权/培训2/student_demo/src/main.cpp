#include "../includes/student.hpp"
#include <iostream>

int main()
{
    Student s("小明", 80.5);
    std::cout << "修改前：" << std::endl;
    s.showInfo();

    s.changeScore(93.0);

    std::cout << "修改后：" << std::endl;
    s.showInfo();
    return 0;
}
