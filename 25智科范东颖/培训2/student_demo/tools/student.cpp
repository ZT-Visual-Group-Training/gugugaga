#include "student.hpp"
#include <iostream>

Student::Student() {}

Student::Student(const string& id, const string& name)
	: m_id(id), m_name(name) {
}
const string& Student::getId() const {
	return m_id;
}
const string& Student::getName() const {
	return m_name;
}
void Student::setName(const string& name) {
	m_name = name;
}
void Student::addScore(const string& subject, double score) {
	m_scores[subject] = score;
}
bool Student::modifyScore(const string& subject, double score) {
	auto it = m_scores.find(subject);
	if (it != m_scores.end()) {
		it->second = score;
		return true;
	}
	return false;
}

double Student::getScore(const string& subject) const {
	auto it = m_scores.find(subject);
	if (it != m_scores.end())
		return it->second;
	return 0.0;
}

void Student::print() const {
	cout << "学号：" << m_id << "\n";
	cout << "姓名：" << m_name << "\n";
	cout << "各科成绩：\n";
	for (auto& p : m_scores) {
		cout << "  " << p.first << " : " << p.second << "\n";
	}
}