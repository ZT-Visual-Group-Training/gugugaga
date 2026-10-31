#ifndef STUDENT_HPP/*头文件保护符
*/
#define STUDENT_HPP

#include <iostream>

#include <string>
//创建student类
class Student {
public:
//类函数的声明（姓名，学号，分数）
    Student(const std::string& name,const std::string& id, double score);

//打印信息
    void printInfo() const;

//创建分数，因为分数是私人权限，无法直接赋值
    void setScore(double newScore);

//得到分数
    double getScore() const;

private:
   std::string name_;
   std::string id_;
    double score_;
};

#endif//结束判断（判断是否读过头文件）