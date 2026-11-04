#include "Student.hpp"
#include <iostream>


int main() {
    student s("Tom", 18, 2026001);
    s.setScore(95);
    s.showInfo();
    return 0;
}
