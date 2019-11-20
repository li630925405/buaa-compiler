#include "grammar.h"

//var_table: var + const, all idfers
map<string, Func> func;
map<string, Var> global_var_table;
map<string, Var> global_const_table;
map<string, Var> fun_var_table;
map<string, Var> fun_const_table;
map<string, Array> array_table;

void print_table(map<string, Var> table) {
	printf("print table:\n");
	for (auto iter = table.begin(); iter != table.end(); iter++) {
		cout << iter->first << endl;
	}
}

void print_table(map<string, Func> func) {
	printf("print func:\n");
	for (auto iter = func.begin(); iter != func.end(); iter++) {
		cout << iter->first << endl;
		iter->second.print_para();
	}
}

void insert_fun(string name, Func fun) {
	if (func.find(name) != func.end() ||
		global_var_table.find(name) != global_var_table.end()) {
		error(RENAME);
		//func.insert(pair<string, Func>(name, fun));
		return;
	}
	else {
		func.insert(pair<string, Func>(name, fun));
	}
}

Var getvar(string name_content) {
	Var var;
	if (fun_var_table.find(name_content) != fun_var_table.end()) {
		var = fun_var_table.at(name_content);
	}
	else if (fun_const_table.find(name_content) != fun_const_table.end()) {
		var = fun_const_table.at(name_content);
	}
	else if (global_var_table.find(name_content) != global_var_table.end()) {
		var = global_var_table.at(name_content);
	}
	else if (global_const_table.find(name_content) != global_const_table.end()) {
		var = global_const_table.at(name_content);
	}
	else {
		error(UNNAME);
	}
	return var;
}

void call_fun(string name) {
	Func fun;
	try {
		fun = func.at(name);
	}
	catch (exception e) {
		error(UNNAME);
		return_call(fun);
		return;
	}
	if (isRetFun(name)) {
		return_call(fun);
	}
	else if (isNonretFun(name)) {
		nonreturn_call(fun);
	}
	if (strcmp(getsym(), "SEMICN") != 0) {
		error_semi();
	} //;
}

void insert_table(string type, string name, map<string, Var>& table) {
	Var var(type, name);
	if (table.find(name) != table.end()) { //函数内的变量可以和全局变量重复？
		error(RENAME);
	}
	else {
		table.insert({ name, var });
	}
}

void insert_var(string type, string name_content, bool fun) {
	if (fun) {
		insert_table(type, name_content, fun_var_table);
	}
	else {
		insert_table(type, name_content, global_var_table);
	}
}

void insert_const(string type, string name_content, bool fun) {
	if (fun) {
		insert_table(type, name_content, fun_const_table);
		insert_table(type, name_content, fun_var_table);
	}
	else {
		insert_table(type, name_content, global_const_table);
		insert_table(type, name_content, global_var_table);
	}
}

void insert_array(string type, string name_content) {
	Array array(name_content, type);
	array_table.insert({ name_content, array });
}

//string get_array_value(string name_content, int index) {
//	Array array = array_table.at(name_content);
//	return array.get_value(index);
//	fetch_array(name_content, index);
//}

string get_array_value(string name_content) {
	fetch_array(name_content);
}

Array get_array(string name_content) {
	return array_table.at(name_content);
}