#pragma once
#include <string>
class student
{
    public:
    student(std::string n, std::string i, int s);

    void setscore(int new_s);
    void print() const;

    private:
    std::string name;
    std::string id;
    int score;
};
