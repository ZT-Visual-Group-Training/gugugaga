#include "student.hpp"
#include <iostream>
using namespace std;

//构造函数初始化列表
Student::Student(const string& name,const string& id, double score)
    : name_(name), id_(id), score_(score) {}

    //函数的定义
void Student::printInfo() const {
    cout << "姓名: " << name_
              << ", 学号: " << id_
              << ", 成绩: " << score_ << endl;
}

void Student::setScore(double newScore) {
   //范围检验，0-100
    if(newScore >= 0.0 && newScore <= 100.0){
        score_ = newScore;
    }else{
        cout << "成绩必须在 0-100 之间，修改失败！" << endl;
    }
    
    
    
}

double Student::getScore() const {
    return score_;
}