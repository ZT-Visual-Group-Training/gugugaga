#include "student.hpp"

#include <iostream>

int main()
{
    // 带参构造，创建的时候直接给值
    Student s1("ZhangSan", 2026001, 78.5);

    // 无参构造，之后再一个一个 set
    Student s2;
    s2.setName("LiSi");
    s2.setId(2026002);
    s2.setScore(92.0);

    s1.print();
    s2.print();

    // 把 ZhangSan 的成绩改一下
    s1.setScore(88.5);
    s1.print();

    std::cout << "LiSi 的成绩是: " << s2.getScore() << std::endl;
    std::cout << "两人平均分: " << (s1.getScore() + s2.getScore()) / 2 << std::endl;

    return 0;
}
