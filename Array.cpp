#include "Array.h"


Array::Array() {
	size = 0;
	memset(int_array, 0, 100);
	memset(char_array, 0, 100);
}

Array::Array(string name, string type) {
	this->name = name;
	this->type = type;
	size = 0;
	memset(int_array, 0, 100);
	memset(char_array, 0, 100);
}

void Array::set_value(int index, string value) {
	if (type == "INTTK") {
		int_array[index] = atoi(value.c_str());
	}
	else if (type == "CHARTK") {
		char_array[index] = value[1];
	}
}
string Array::get_value(int index) {
	if (type == "INTTK") {
		return int2str(int_array[index]);
	}
	else if (type == "CHARTK") {
		char* s = (char*)malloc(4);
		memset(s, 0, 4);
		s[0] = '\'';
		s[1] = char_array[index];
		s[2] = '\'';
		return s;
	}
}