#include "student.hpp"
int main()
{
	Student s("2026001", "小王");
	s.addScore("C++", 92.5);
	s.addScore("数学", 88);
	s.modifyScore("数学", 95);
	s.modifyScore("英语", 60);

	s.print();
	return 0;
}
