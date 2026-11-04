#include "student.hpp"
#include <iostream>
using namespace std;

student::student(string n, string i, int s){
    name = n;
    id = i;
    if(s >= 0 && s <= 100){
        score = s;
    }else{
        cout << "成绩必须在0-100之间！" << endl;
        score = 0;
    }
}

void student::setscore(int new_s){
    if(new_s >= 0 && new_s <= 100){
        score = new_s;
        cout << name << "的分数修改完毕。" << endl;
    }else{
        cout << "成绩必须在0-100之间，修改失败！" << endl;
    }
}

void student::print() const{
    cout << "学生信息如下：" << endl;
    cout << "姓名：" << name << endl;
    cout << "学号：" << id << endl;
    cout << "分数：" << score << endl;
}
