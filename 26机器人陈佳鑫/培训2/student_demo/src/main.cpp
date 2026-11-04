#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "student.hpp"
using namespace std;
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    string a, b;
    int c = 0;
    cout << "请输入学生名字，学号，分数：" << endl;
    cin >> a >> b >> c;
    while(cin.fail() || c < 0 || c > 100){
        if(cin.eof()){
            cout << "输入结束，程序退出。" << endl;
            return 0;
        }
        cin.clear();
        cin.ignore(1024, '\n');
        cout << "输入无效，请重新输入分数（0-100）：";
        cin >> c;
    }
    student s1(a, b, c);
    s1.print();

    int new_s = 0;
    cout << "请输入你要修改的分数：" << endl;
    cin >> new_s;
    while(cin.fail() || new_s < 0 || new_s > 100){
        if(cin.eof()){
            cout << "输入结束，程序退出。" << endl;
            return 0;
        }
        cin.clear();
        cin.ignore(1024, '\n');
        cout << "输入无效，请重新输入分数（0-100）：";
        cin >> new_s;
    }
    s1.setscore(new_s);
    s1.print();
    return 0;
}
