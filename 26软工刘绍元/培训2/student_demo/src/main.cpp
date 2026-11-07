#include "student.hpp"
#include <iostream>
using namespace std;

int main() {
    Student student("小明", "2026001", 85);
    cout << "修改前\n";
    student.printInfo();
    student.setScore(92.5);
    cout << "修改后\n";
    student.printInfo();
    return 0;
}
