#include "Func.h"

#include <iostream>
using namespace std;

void Func::add_para(Var var) {
	this->parameter.insert(pair<int, Var>(this->para_num++, var));
}

void Func::add_para(string para_type, string para_name) {
	Var var(para_type, para_name);
	this->parameter.insert(pair<int, Var>(this->para_num++, var));
}

void Func::print_para() {
	for (auto iter = parameter.begin(); iter != parameter.end(); iter++) {
		cout << "name: " << iter->second.name << " type: " << iter->second.type << endl;
	}
}