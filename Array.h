#pragma once
#include "lib.h"
#include "common.h"

class Array
{
public:
	string name;
	string type;
	int size;
	int int_array[100];
	char char_array[100];
public:
	Array();
	Array(string name, string type);
	void set_value(int index, string value);
	string get_value(int index);
};

