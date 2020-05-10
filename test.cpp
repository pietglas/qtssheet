#include <string>
#include <set>
#include <map>
#include <iostream>

std::map<std::string, std::set<std::string>> depends_on_;

bool checkCircularityHelper(const std::string & lhs, const std::string & index,
	const std::set<std::string> & depends_on) {
	// see if current index depends on lhs
	if (depends_on.find(lhs) != depends_on.end())
		return true;
	for (auto ind : depends_on) {
		if (checkCircularityHelper(lhs, ind, depends_on_[ind]))
			return true;
	}
	return false;
}

bool checkCircularity(const std::string & lhs, 
					const std::set<std::string> & indices_rhs) {
	std::map<std::string, std::set<std::string>> new_depends = depends_on_;
	new_depends.insert({lhs, indices_rhs});
	if (!checkCircularityHelper(lhs, lhs, indices_rhs)) {
		depends_on_ = new_depends;
		return false;
	}
	return true;
}

int main () {
	std::string lhs = "A1";
	std::string rhs = "B1";
	std::set<std::string> indices;
	indices.insert(rhs);
	if (checkCircularity(lhs, indices))
		std::cout << "circular declaration 0 "<< std::endl;
	std::string lhs1 = "B1";
	std::string rhs1 = "C1";
	std::set<std::string> indices1;
	indices1.insert(rhs1);
	if (checkCircularity(lhs1, indices1))
		std::cout << "circular declaration 1 " << std::endl;
	std::string lhs2 = "C1";
	std::string rhs2 = "A1";
	std::set<std::string> indices2;
	indices2.insert(rhs2);
	if (checkCircularity(lhs2, indices2))
		std::cout << "circular declaration 2 " << std::endl;
}

