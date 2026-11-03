#include <iostream>
#include <vector>

#include "student.hpp"

int main() {
    // 1. 用带参构造函数创建一个学生
    Student stu("吴希言", "20260001", {85.0, 92.5, 78.0});
    std::cout << "[1] 创建学生并打印信息：\n";
    stu.print();

    // 2. 修改成绩：把第 0 门从 85 改成 90
    std::cout << "\n[2] 把第 0 门成绩改成 90：\n";
    if (stu.modifyScore(0, 90.0)) {
        std::cout << "修改成功。\n";
    }
    stu.print();

    // 3. 追加一门新成绩
    std::cout << "\n[3] 追加一门成绩 88：\n";
    stu.addScore(88.0);
    stu.print();

    // 4. 越界与非法值测试（应该被拒绝）
    std::cout << "\n[4] 故意越界修改第 10 门成绩：\n";
    stu.modifyScore(10, 60.0);
    std::cout << "程序没有崩溃，说明越界检查生效。\n";

    std::cout << "\n[5] 删除第 1 门成绩：\n";
    if (stu.removeScore(1)) {
        std::cout << "删除成功。\n";
    }
    stu.print();

    // 6. setter 测试
    stu.setName("吴希言（改名测试）");
    std::cout << "\n[6] 修改姓名后：\n";
    stu.print();

    return 0;
}
