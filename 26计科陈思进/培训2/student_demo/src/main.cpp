#include "student.hpp"
#include <iostream>
#include <cstdlib>
using namespace std;
int main() {
system("chcp 65001");//中文乱码，我用ai搜了一下，添加这串代码
    Student s("张三", 4270712, 85.5);//创建成员s——姓名，学号（本来想写04270712的，结果显示八进制，输出的不对），原始成绩）
    cout << "--- 初始信息 ---" << endl;
    s.printInfo();//调用函数，打印修改前的信息


    cout << "\n--- 修改成绩为 92.0 ---" << endl;
    s.setScore(92.0);
    s.printInfo();//打印修改后的信息

    return 0;
}