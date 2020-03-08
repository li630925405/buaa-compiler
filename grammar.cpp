#include "grammar.h"

string fun_type;
bool first;

void const_define(bool fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	// const
	string type = getsym();  // int or char
	string sym = getsym();
	while (sym != "IDENFR") {
		error(ILLEGAL);
		jump_till("SEMICN");
		return;
	} // IDENFR
	string name_content = name;
	insert_const(type, name_content, fun);
	Var var = getvar(name_content);
	getsym(); // ASSIGN
	set_value(var);
	print_const(var);
	while (strcmp(getsym(), "COMMA") == 0) {
		sym = getsym();
		if (sym != "IDENFR") {
			error(ILLEGAL);
			jump_till("SEMICN");
			return;
		} // IDENFR
		name_content = name;
		insert_const(type, name_content, fun);
		Var var = getvar(name_content);
		getsym(); //ASSIGN
		set_value(var);
		print_const(var);
	}
	backtrack(1);
}

void const_state(bool fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	const_define(fun);
	judge_semi(getsym());
	while (strcmp(getsym(), "CONSTTK") == 0) {
		const_define(fun);
		judge_semi(getsym());
	}
	backtrack(1);
}

//变量在定义时加入符号表，常量在声明时加入符号表 
void var_define(string type, bool fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	string name_content;
	//getsym(); // int or char
	//getsym(); //IDENFR
	do {
		if (strcmp(getsym(), "IDENFR") != 0) {
			error(ILLEGAL);
			return;
		} //IDENFR
		name_content = name;
		insert_var(type, name_content, fun);
		if (strcmp(getsym(), "LBRACK") == 0) {
			Array a(name_content, type);
			int size = index(); //unfinished push_stack?
			a.size = size;
			print_array(a);
		}
		else {
			print_var(type, name_content);
			backtrack(1);
		}
	} while (strcmp(getsym(), "COMMA") == 0);
	backtrack(1);
}

void var_state(string type, bool fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	var_define(type, fun);
	judge_semi(getsym());
	type = getsym();
	string sym;
	string name_content;
	if (!isTypeIden(type.c_str())) {
		//int or char
		backtrack(1);
		return;
	}
	sym = getsym();
	name_content = name;
	sym = getsym(); //(
	backtrack(1); //(
	while (sym != "LPARENT") {
		backtrack(1); //name
		var_define(type, fun);
		judge_semi(getsym());
		type = getsym();
		if (!isTypeIden(type.c_str())) {
			//int or char
			backtrack(1);
			return;
		}
		getsym(); //name
		sym = getsym(); //(
		backtrack(1); //(
	}
	backtrack(2);
}

void state_head() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	//getsym();  // int or char
	//getsym(); //IDENFR
}

void program() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	string type;
	string name_content;
	char sym[100] = { 0 };
	if (strcmp(getsym(), "CONSTTK") == 0) {
		const_state(false);
	}
	else {
		backtrack(1);
	}
	type = getsym(); //int or char or void
	strcpy(sym, getsym());
	name_content = name;
	while (strcmp(sym, "IDENFR") != 0 && strcmp(sym, "MAINTK") != 0) {
		error(ILLEGAL);
		new_line();
		type = getsym();
		strcpy(sym, getsym());
		name_content = name;
	} //IDENFR
	strcpy(sym, getsym()); // (
	backtrack(1);
	if (strcmp(sym, "LPARENT") != 0) {
		backtrack(1);
		if (!isTypeIden(type.c_str())) {
			throw "parse error";
		}
		insert_table(type, name_content, global_var_table);
		var_state(type, false);
		type = getsym();
		if (!isTypeIden(type.c_str()) && type != "VOIDTK") {
			//int or char or void
			throw "parse error";
		}
		strcpy(sym, getsym());
		while (strcmp(sym, "IDENFR") != 0 && strcmp(sym, "MAINTK") != 0) {
			error(ILLEGAL);
			new_line();
			type = getsym();
			strcpy(sym, getsym());
			name_content = name;
		} //IDENFR
	}
	while (name != "main") {
		name_content = name;
		Func tmp(name_content, type);
		insert_fun(name_content, tmp);
		if (isTypeIden(type.c_str())) {
			returnFun_define(tmp);
		}
		else if (type == "VOIDTK") {
			nonreturnFun_define(tmp);
		}
		else {
			throw "parse error";
		}
		try {
			func.at(name_content) = tmp;
		}
		catch (exception e) {
			;
		}
		type = getsym();
		if (!isTypeIden(type.c_str()) && type != "VOIDTK") {
			//int or char or void
			throw "parse error";
		}
		strcpy(sym, getsym());
		while (strcmp(sym, "IDENFR") != 0 && strcmp(sym, "MAINTK") != 0) {
			error(ILLEGAL);
			new_line();
			type = getsym();
			strcpy(sym, getsym());
			name_content = name;
		} //IDENFR
	}
	MAIN();
}

void returnFun_define(Func& fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	fun_type = fun.type;
	first = true;
	state_head();
	judge_parse(getsym(), "LPARENT");
	paratable(fun);
	print_fun(fun);
	func.at(fun.name) = fun;
	judge_rparent(getsym());
	judge_parse(getsym(), "LBRACE");
	com_statement(true);
	judge_parse(getsym(), "RBRACE");
	fun_const_table.clear();
	fun_var_table.clear();
}

void nonreturnFun_define(Func& fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	fun_type = "VOIDTK";
	first = true;
	// void
	judge_parse(getsym(), "LPARENT");
	paratable(fun);
	print_fun(fun);
	func.at(fun.name) = fun;
	judge_rparent(getsym());
	judge_parse(getsym(), "LBRACE");
	com_statement(true);
	judge_parse(getsym(), "RBRACE");
	ret("VOIDTK"); // 
	fun_const_table.clear();
	fun_var_table.clear();
}

void com_statement(bool fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (strcmp(getsym(), "CONSTTK") == 0) {
		const_state(fun);
	}
	else { backtrack(1); }
	string type = getsym();
	if (isTypeIden(type.c_str())) {
		while (strcmp(getsym(), "IDENFR") != 0) {
			error(ILLEGAL);
			new_line();
			type = getsym();
			if (!isTypeIden(type.c_str())) {
				backtrack(1);
				break;
			}
			//getsym();  //IDENFR
		}
		string name_content = name;
		backtrack(1);
		var_state(type, fun);
	}
	else {
		backtrack(1);
	}
	statement_list();
}

void paratable(Func& fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	string type = getsym();
	if (type == "LBRACE") {
		error(RPARENT);
		backtrack(1);
		return;
	}
	if (type != "RPARENT") {
		//backtrack(1);
		if (strcmp(getsym(), "IDENFR") != 0) {
			error(ILLEGAL);
			jump_till("RPARENT");
			return;
		} // IDENFR
		Var var(type, name);
		fun.add_para(var);
		fun_var_table.insert(pair<string, Var>(name, var));
		while (strcmp(getsym(), "COMMA") == 0) {
			type = getsym();
			if (!isTypeIden(type.c_str())) {
				throw "parse error";
			} // type iden
			if (strcmp(getsym(), "IDENFR") != 0) {
				error(ILLEGAL);
				jump_till("RPARENT");
				return;
			} // IDENFR
			Var var(type, name);
			fun.add_para(var);
			if (fun_var_table.find(name) != fun_var_table.end()) {
				error(RENAME);
			}
			else {
				fun_var_table.insert(pair<string, Var>(name, var));
			}
		}
	}
	backtrack(1);
}

void MAIN() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	fmiddle << "main" << endl; //?
	fun_type = "VOIDTK";
	first = false;
	//getsym(); // void
	//getsym(); // main
	judge_parse(getsym(), "LPARENT");
	judge_rparent(getsym());
	judge_parse(getsym(), "LBRACE");
	com_statement(false);
	judge_parse(getsym(), "RBRACE");
}

void expression() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (reread() == '+' || reread() == '-') {
		st.push("0");
		st_iden.push("0");
	}
	else {
		term();
	}
	terms();
}

void  terms() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (reread() == '+') {
		getsym();
		term();
		add();
		terms();
	}
	else if (reread() == '-') {
		getsym();
		term();
		subt();
		terms();
	}
}

void term() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	factor();
	factors();
}

void factors() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (reread() == '*') {
		getsym();
		factor();
		mult();
		factors();
	}
	else if (reread() == '/') {
		getsym();
		factor();
		div();
		factors();
	}
}

void factor() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	char sym[100] = { 0 };
	strcpy(sym, getsym());
	if (strcmp(sym, "LPARENT") == 0) {
		expression();
		getsym();
	}
	else if (strcmp(sym, "INTCON") == 0 || isPlusMinus(sym)) {
		backtrack(1);
		pushi(integerNum());
	}
	else if (strcmp(sym, "CHARCON") == 0) {
		st.push("'" + name + "'");
		st_iden.push("'" + name + "'");
	}
	else {
		string name_content = name;
		strcpy(sym, getsym());
		if (strcmp(sym, "LBRACK") == 0) {
			Array a = get_array(name_content);
			expression();
			judge_rbrack(getsym());
			fetch_array(name_content);
		}
		else if (strcmp(sym, "LPARENT") == 0) {
			call_fun(name_content);
		}
		else {
			backtrack(1);
			Var var = getvar(name_content);
			st.push(var.value);
			st_iden.push(var.name);
		}
	}
}

void statement() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	string name_content;
	char sym[100] = { 0 };
	strcpy(sym, getsym());
	name_content = name;
	if (strcmp(sym, "IFTK") == 0) {
		backtrack(1);
		if_statement();
	}
	else if (strcmp(sym, "WHILETK") == 0
		|| strcmp(sym, "FORTK") == 0
		|| strcmp(sym, "DOTK") == 0) {
		backtrack(1);
		while_statement();
	}
	else if (strcmp(sym, "LBRACE") == 0) {
		statement_list();
		judge_parse(getsym(), "RBRACE");
	}
	else if (strcmp(sym, "SCANFTK") == 0) {
		backtrack(1);
		read_statement();
		judge_semi(getsym());
	}
	else if (strcmp(sym, "PRINTFTK") == 0) {
		write_statement();
		judge_semi(getsym());
	}
	else if (strcmp(sym, "RETURNTK") == 0) {
		backtrack(1);
		return_statement();
		judge_semi(getsym());
	}
	else if (strcmp(sym, "IDENFR") == 0) {
		strcpy(sym, getsym());
		if (strcmp(sym, "LPARENT") == 0) {
			call_fun(name_content);
		}
		else if (strcmp(sym, "ASSIGN") == 0 || strcmp(sym, "LBRACK") == 0) {
			backtrack(1);
			assign_statement(name_content);
			judge_semi(getsym());
		}
		else {
			throw "parse error";
		}
	}
	else if (strcmp(sym, "SEMICN") == 0) {
		// ;
	}
	else {
		cout << "----sym: " << sym << endl;
		throw "parse error";
	}
}

void assign_statement(string name_content) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	char sym[100] = { 0 };
	strcpy(sym, getsym());
	if (strcmp(sym, "ASSIGN") == 0) {
		Var var = assign_judge(name_content);
		expression();
		assign(var); //这不用写&var, 只要定义函数的参数有&就行了
	}
	else if (strcmp(sym, "LBRACK") == 0) {
		/*if (expression() != "INTTK") {
			error(INDEX);
		}*/
		expression();
		judge_rbrack(getsym());
		judge_parse(getsym(), "ASSIGN");
		expression();
		assign(name_content);  //数组怎么存?
	}
}

void if_statement() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	string if_false = gen_lab();
	string if_end = gen_lab();
	judge_parse(getsym(), "IFTK");
	judge_parse(getsym(), "LPARENT");
	require();
	judge_rparent(getsym());
	brf(if_false);
	statement();
	br(if_end);
	labprod(if_false);
	if (strcmp(getsym(), "ELSETK") == 0) {
		statement();
	}
	else {
		backtrack(1);
	}
	labprod(if_end);
}

bool require() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	bool res;
	string rela;
	expression();
	rela = getsym();
	if (isRelaString(rela.c_str())) {
		expression();
	}
	else {
		backtrack(1);
	}
	emit(rela);
	res = atoi(st.top().c_str());
	st.pop();
	return res;
}

void while_statement() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	char sym[100] = { 0 };
	strcpy(sym, getsym());
	if (strcmp(sym, "WHILETK") == 0) {
		string _while = gen_lab();
		judge_parse(getsym(), "LPARENT");
		bool r = require();
		judge_rparent(getsym());
		brf(_while);
		statement();
		labprod(_while);
	}
	else if (strcmp(sym, "DOTK") == 0) {
		string do_while = gen_lab();
		labprod(do_while);
		statement();
		judge_while(getsym());
		judge_parse(getsym(), "LPARENT");
		bool r = require();
		judge_rparent(getsym());
		brt(do_while);
	}
	else if (strcmp(sym, "FORTK") == 0) {
		string for_end = gen_lab();
		string for_begin = gen_lab();
		judge_parse(getsym(), "LPARENT");
		judge_parse(getsym(), "IDENFR");
		string id = name;
		judge_unname(id);
		judge_parse(getsym(), "ASSIGN");
		expression();
		judge_semi(getsym());
		initload(id);
		labprod(for_begin);
		bool r = require();
		judge_semi(getsym());
		judge_parse(getsym(), "IDENFR");
		judge_unname(name);
		Var i = getvar(name);
		judge_parse(getsym(), "ASSIGN");
		judge_parse(getsym(), "IDENFR");
		judge_unname(name);
		Var i_prev = getvar(name);
		string op = getsym();
		if (!isPlusMinus(op.c_str())) {
			throw "parse error";
		} //+ -
		int step = step_size();
		judge_rparent(getsym());
		brf(for_end);
		statement();
		//change id
		i.value = change(i_prev, op, step);
		br(for_begin);
		labprod(for_end);
	}
}

int step_size() {
	return unsign_int();
}

void return_call(Func fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	//IDENFR
	//(
	value_paratable(fun);
	judge_rparent(getsym());
	call(fun.name);
	st_iden.push("RET");
}

void nonreturn_call(Func fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	//IDENFR
	//(
	value_paratable(fun);
	judge_rparent(getsym());
	call(fun.name);
}

void value_paratable(Func fun) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	string tmp = getsym();
	int matched = fun.para_num;
	if (tmp == "RPARENT" || tmp == "SEMICN") {
		backtrack(1);
		checklen(matched);
		if (tmp == "SEMICN") {
			error(RPARENT);
		}
		return;
	}
	backtrack(1);
	map<int, Var>::iterator iter = fun.parameter.begin();
	expression();
	push(); //?
	checktype(matched, iter);
	exprs(matched, iter);
	// jsr 返回地址的下一句语句放入操作数栈
}

void exprs(int matched, map<int, Var>::iterator iter) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (strcmp(getsym(), "COMMA") == 0) {
		expression();
		push(); //?
		checktype(matched, iter);
		exprs(matched, iter);
	}
	else {
		backtrack(1);
		checklen(matched);
	}
}

void statement_list() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	//语句列结束即复合语句结束 都是}
	while (strcmp(getsym(), "RBRACE") != 0) {
		backtrack(1);
		statement();
	}
	backtrack(1);
}

void read_statement() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	getsym(); //scanf
	judge_parse(getsym(), "LPARENT");
	judge_parse(getsym(), "IDENFR");
	judge_unname(name);
	read(name);
	string sym = getsym();
	while (sym == "COMMA") {
		judge_parse(getsym(), "IDENFR");
		judge_unname(name);
		read(name);
		sym = getsym();
	}
	judge_rparent(sym);
	//backtrack
	//)
}

void write_statement() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	//getsym(); //pritnf
	judge_parse(getsym(), "LPARENT");
	char c = fgetc(in);
	while (isspace(c)) {
		c = fgetc(in);
	}
	if (c == '"') {
		fseek(in, -1, SEEK_CUR);
		readString();
		string string_content = name;
		if (strcmp(getsym(), "COMMA") == 0) {
			expression();
			string iden = st_iden.top();
			st_iden.pop();
			write(string_content, iden);
		}
		else {
			write(string_content);
			backtrack(1);
		}
	}
	else {
		fseek(in, -1, SEEK_CUR);
		expression();
		write();
	}
	judge_rparent(getsym());
}

void return_statement() {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	getsym(); //return
	if (strcmp(getsym(), "LPARENT") == 0) {
		if (first) {
			//ret = expression();
			expression();
			ret(fun_type);
			first = false;
		}
		else {
			expression();
		}
		judge_rparent(getsym());
	}
	else {
		st.push("void");
		ret(fun_type);
		backtrack(1);
	}
}