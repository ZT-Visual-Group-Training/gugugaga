#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <map>
#include <string>
using namespace std;
class Student {
public:
	Student();
	Student(const string& id, const string& name);

	const string& getId() const;
	const string& getName() const;

	void setName(const string& name);
	void addScore(const string& subject, double score);
	bool modifyScore(const string& subject, double score);
	double getScore(const string& subject) const;
	void print() const;
private:
	string m_id;
	string m_name;
	std::map<string, double> m_scores;
};
#endif