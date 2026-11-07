#include "student.hpp"
#include <iostream>
int main()
{
	Student s("2026001", "小王");
	s.addScore("C++", 92.5);
	s.addScore("数学", 88);
	s.modifyScore("数学", 95);
	if (!s.modifyScore("英语", 60)) {
		std::cout << "没有英语这门课，修改失败\n";
	}

	s.print();
	return 0;
}
