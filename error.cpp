#include "grammar.h"

void error_semi() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	backtrack(1);
	error(SEMI);
}

void new_line() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	backtrack(1);
	char c = fgetc(in);
	while (c != '\n') {
		c = fgetc(in);
	}
	for (int i = 0; i < 3; i++) {
		is_newline[i] = true;
	}
	line++;
}

// only one error in one line
void jump_till(string str) {
	backtrack(1);
	string sym = getsym();
	while (sym != str) {
		sym = getsym();
	}
	backtrack(1);
}


void readString() {
	if (debug == 1) {
		printf("%s\n", __func__);
		if (pause)
			system("pause");
	}
	//getsym(); //"
	getsym(); //STRINGTK
}

Var judge_unname(string name_content) {
	if (global_var_table.find(name_content) == global_var_table.end()
		&& fun_var_table.find(name_content) == fun_var_table.end()) {
		error(UNNAME);
		cout << "-----unname var: " << name_content << endl;
		return Var();
	}
	else {
		if (fun_var_table.find(name_content) == fun_var_table.end()) {
			return global_var_table.at(name_content);
		}
		else {
			return fun_var_table.at(name_content);
		}
	}
}

Var assign_judge(string name_content) {
	Var var = judge_unname(name_content);
	if (fun_var_table.find(name_content) == fun_var_table.end()
		&& global_const_table.find(name_content) != global_const_table.end()) {
		error(CONST_CHANGE);
	}
	else if (fun_const_table.find(name_content) != fun_const_table.end()) {
		error(CONST_CHANGE);
	}
	return var;
}

void judge_rparent(string sym) {
	if (sym != "RPARENT") {
		error(RPARENT);
		backtrack(1);
	} //)
}

void judge_semi(string sym) {
	if (sym != "SEMICN") {
		error_semi();
	} //;
}

void judge_parse(string sym, string should) {
	if (sym != should) {
		throw "parse error";
	}
}

void judge_while(string sym) {
	if (sym != "WHILETK") {
		error(DO_WHILE);
		backtrack(1);
	}
}

void judge_rbrack(string sym) {
	if (sym != "RBRACK") {
		error(RBRACK);
		backtrack(1);
	}
}


int unsign_int() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (strcmp(getsym(), "INTCON") != 0) {
		throw string("not int error");
	}
	return atoi(name.c_str());
}

int integerNum() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	int res = 0;
	char op = 0;
	if (isPlusMinus(getsym())) {
		if (name == "-") {
			op = '-';
		}
		try {
			res = unsign_int();
		}
		catch (string & e) {
			throw e;
		}
	}
	else {
		backtrack(1);
		try {
			res = unsign_int();
		}
		catch (string & e) {
			throw e;
		}
	}
	if (op == '-') {
		res *= -1;
	}
	return res;
}

//void judge_illegal(char* sym, string &type, string &name_content) {
//  while (strcmp(sym, "IDENFR") != 0 && strcmp(sym, "MAINTK") != 0) {
//      error(ILLEGAL);
//      new_line();
//      type = getsym();
//      strcpy(sym, getsym());
//      name_content = name;
//  } //IDENFR
//}    非常量引用的初始值不能是左值