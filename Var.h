#pragma once
#include <string>

using namespace std;

class Var
{
public:
	string type;
	string name;
	string value;
	Var() {
		value = "0";
	}
	Var(string type, string name) {
		this->type = type;
		this->name = name;
	}
};

