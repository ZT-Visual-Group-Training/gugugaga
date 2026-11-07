#include "student.hpp"
#include <iostream>

int main() {
Student s("xiaoyu", 1001, 95.5);
s.print();
s.setScore(88.0);
s.print();
return 0;
}
