#include "student.hpp"

int main() {
    Student s1("张三", 1001, 85.5);
    std::cout << "--- 初始信息 ---" << std::endl;
    s1.printInfo();

    s1.setScore(95.0);
    std::cout << "--- 修改成绩后 ---" << std::endl;
    s1.printInfo();

    return 0;
}
