#include "student.hpp"

#include <iomanip>
#include <iostream>
#include <numeric>

Student::Student() = default;

Student::Student(const std::string& name, const std::string& id)
    : name_(name), id_(id) {}

Student::Student(const std::string& name, const std::string& id,
                 const std::vector<double>& scores)
    : name_(name), id_(id) {
    for (double score : scores) {
        addScore(score);
    }
}

void Student::setName(const std::string& name) { name_ = name; }

void Student::setId(const std::string& id) { id_ = id; }

const std::string& Student::getName() const { return name_; }

const std::string& Student::getId() const { return id_; }

bool Student::isValidScore(double score) {
    return score >= kMinScore && score <= kMaxScore;
}

void Student::addScore(double score) {
    if (!isValidScore(score)) {
        std::cerr << "[警告] 成绩 " << score << " 不在 0~100 之间，已忽略。\n";
        return;
    }
    scores_.push_back(score);
}

bool Student::modifyScore(std::size_t index, double newScore) {
    if (index >= scores_.size()) {
        std::cerr << "[错误] 成绩下标 " << index << " 越界（共 "
                  << scores_.size() << " 门）。\n";
        return false;
    }
    if (!isValidScore(newScore)) {
        std::cerr << "[错误] 新成绩 " << newScore << " 不在 0~100 之间。\n";
        return false;
    }
    scores_[index] = newScore;
    return true;
}

bool Student::removeScore(std::size_t index) {
    if (index >= scores_.size()) {
        std::cerr << "[错误] 成绩下标 " << index << " 越界（共 "
                  << scores_.size() << " 门）。\n";
        return false;
    }
    scores_.erase(scores_.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

std::size_t Student::getScoreCount() const { return scores_.size(); }

double Student::getScore(std::size_t index) const {
    if (index >= scores_.size()) {
        return 0.0;
    }
    return scores_[index];
}

double Student::getAverage() const {
    if (scores_.empty()) {
        return 0.0;
    }
    const double sum = std::accumulate(scores_.begin(), scores_.end(), 0.0);
    return sum / static_cast<double>(scores_.size());
}

double Student::getMax() const {
    if (scores_.empty()) {
        return 0.0;
    }
    double maxScore = scores_.front();
    for (double score : scores_) {
        if (score > maxScore) {
            maxScore = score;
        }
    }
    return maxScore;
}

double Student::getMin() const {
    if (scores_.empty()) {
        return 0.0;
    }
    double minScore = scores_.front();
    for (double score : scores_) {
        if (score < minScore) {
            minScore = score;
        }
    }
    return minScore;
}

void Student::printScores() const {
    if (scores_.empty()) {
        std::cout << "  （暂无成绩）\n";
        return;
    }
    std::cout << std::fixed << std::setprecision(1);
    for (std::size_t i = 0; i < scores_.size(); ++i) {
        std::cout << "  [" << i << "] " << scores_[i] << '\n';
    }
}

void Student::print() const {
    std::cout << "====================\n";
    std::cout << "姓名: " << name_ << '\n';
    std::cout << "学号: " << id_ << '\n';
    std::cout << "成绩:\n";
    printScores();
    if (!scores_.empty()) {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "平均分: " << getAverage() << '\n';
        std::cout << "最高分: " << getMax() << '\n';
        std::cout << "最低分: " << getMin() << '\n';
    }
    std::cout << "====================\n";
}
