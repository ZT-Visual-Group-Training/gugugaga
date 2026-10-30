#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <cstddef>
#include <string>
#include <vector>

// Student 类：保存一个学生的基本信息与若干门成绩，
// 提供信息打印、成绩修改等功能。
class Student {
public:
    Student();
    Student(const std::string& name, const std::string& id);
    Student(const std::string& name, const std::string& id,
            const std::vector<double>& scores);

    // 基本信息
    void setName(const std::string& name);
    void setId(const std::string& id);
    const std::string& getName() const;
    const std::string& getId() const;

    // 成绩管理
    void addScore(double score);                              // 追加一门成绩
    bool modifyScore(std::size_t index, double newScore);     // 修改第 index 门成绩
    bool removeScore(std::size_t index);                      // 删除第 index 门成绩

    // 成绩查询
    std::size_t getScoreCount() const;
    double getScore(std::size_t index) const;
    double getAverage() const;   // 平均分，无成绩时返回 0
    double getMax() const;       // 最高分，无成绩时返回 0
    double getMin() const;       // 最低分，无成绩时返回 0

    // 打印
    void print() const;          // 打印学生全部信息
    void printScores() const;    // 只打印成绩列表

private:
    static constexpr double kMinScore = 0.0;
    static constexpr double kMaxScore = 100.0;

    static bool isValidScore(double score);

    std::string name_;
    std::string id_;
    std::vector<double> scores_;
};

#endif  // STUDENT_HPP
