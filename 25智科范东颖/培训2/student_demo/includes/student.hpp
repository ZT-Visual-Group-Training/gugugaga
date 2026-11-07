#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <map>
#include <string>

class Student {
public:
	Student();
	Student(const std::string& id, const std::string& name);

	const std::string& getId() const;
	const std::string& getName() const;

	void setName(const std::string& name);
	void addScore(const std::string& subject, double score);
	bool modifyScore(const std::string& subject, double score);
	double getScore(const std::string& subject) const;
	void print() const;
private:
	std::string m_id;
	std::string m_name;
	std::map<std::string, double> m_scores;
};
#endif
