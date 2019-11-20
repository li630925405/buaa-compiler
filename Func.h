#pragma once
#include <string>
#include <list>
#include "Var.h"
#include <map>
using namespace std;

class Func
{	
public:
	string name;
	string type;
	map<int, Var> parameter;
	int para_num;
public:
	Func() {
		// this->parameter = {}; ?
		this->para_num = 0;
	}
	Func(string name, string type, map<int, Var> parameter) {
		this->name = name;
		this->type = type;
		this->parameter = parameter;
		this->para_num = 0;
	}
	Func(string name, string type) {
		this->name = name;
		this->type = type;
		this->para_num = 0;
		//this->parameter = NULL;
	}
	void add_para(string para_type, string para_name);
	void add_para(Var var);
	void print_para();
};

