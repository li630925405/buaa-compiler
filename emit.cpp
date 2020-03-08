#include "common.h"
#include "lib.h"
#include "error.h"
#include "grammar.h"

stack<string> st;
stack<string> st_iden;
const char* tmp_token[7] = { "t1", "t2", "t3", "t4", "t5", "t6", "t7" };
int tmp_t = 0;
map<string, string> token = { {"VOIDTK", "void"}, {"INTTK", "int"}, {"CHARTK", "char"}, {"MINU", "-"}, {"PLUS", "+"} };
int lab_num = 0;

void brf(string labx) {
	emit("BRF", labx);
}

void brt(string labx) {
	emit("BRT", labx);
}

void br(string labx) {
	emit("BR", labx);
}

void labprod(string labx) {
	fmiddle << labx << ":" << endl;
}

string gen_lab() {
	string labx = "label" + int2str(lab_num);
	lab_num++;
	return labx;
}

bool isChar(string s) {
	if (s[0] == '\'' && s[2] == '\'') {
		return true;
	}
	return false;
}

bool isVoid(string s) {
	return s == "void";
}

void add_tmp_reg() {
	string reg = tmp_token[tmp_t];
	tmp_t = (tmp_t + 1) % 7;
	while (contain_var(reg)) {
		reg = tmp_token[tmp_t];
		tmp_t = (tmp_t + 1) % 7;
	}
}

string int2str(int a) {
	char* buf = (char*)malloc(10);
	memset(buf, 0, 10);
	sprintf(buf, "%d", a);
	return buf;
}

char* expr;
int t = -1;

void emit(string type);
void emitl(string type, int i);
void add();
void subt();
void mult();
void div();
void pushi(int i);
void expression();
void terms();
void term();
void factors();
void factor();

bool is_tmp(string iden) {
	for (int i = 0; i < 7; i++) {
		if (iden == tmp_token[i]) {
			if (!contain_var(iden)) {
				return true;
			}
		}
	}
	return false;
}

void emit(string type) {
	//int a = 0, b = 0;
	/*bool a_char = false, b_char = false;
	if (isChar(st.top())) {
		b_char = true;
		b = st.top()[1];
	}
	else {
		b = atoi(st.top().c_str());
	}*/
	//st.pop();
	string b_iden = st_iden.top();
	st_iden.pop();
	if (is_tmp(b_iden)) {
		tmp_t--;
	}
	/*if (isChar(st.top())) {
		a = st.top()[1];
	}
	else {
		a = atoi(st.top().c_str());
	}
	st.pop();*/
	string a_iden = st_iden.top();
	st_iden.pop();
	if (is_tmp(a_iden)) {
		tmp_t--;
	}
	if (type == "ADD") {
		//st.push(int2str(a + b));
		st_iden.push(tmp_token[tmp_t]);
		fmiddle << tmp_token[tmp_t] << "=" << a_iden << "+" << b_iden << endl;
		tmp_t = (tmp_t + 1) % 7;
	}
	else if (type == "SUBT") {
		//st.push(int2str(a - b));
		st_iden.push(tmp_token[tmp_t]);
		fmiddle << tmp_token[tmp_t] << "=" << a_iden << "-" << b_iden << endl;
		tmp_t = (tmp_t + 1) % 7;
	}
	else if (type == "MULT") {
		//st.push(int2str(a * b));
		st_iden.push(tmp_token[tmp_t]);
		fmiddle << tmp_token[tmp_t] << "=" << a_iden << "*" << b_iden << endl;
		tmp_t = (tmp_t + 1) % 7;
	}
	else if (type == "DIV") {
		//st.push(int2str(a / b));
		st_iden.push(tmp_token[tmp_t]);
		fmiddle << tmp_token[tmp_t] << "=" << a_iden << "/" << b_iden << endl;
		tmp_t = (tmp_t + 1) % 7;
	}
	//else {
	//  if (a_char || b_char) {
	//      error(REQUIRE_ILLEGAL);
	//  }
	//}
	if (type == "EQL") {
		//st.push(int2str(a == b));
		fmiddle << a_iden << "==" << b_iden << endl;
	}
	else if (type == "NEQ") {
		//st.push(int2str(a != b));
		fmiddle << a_iden << "!=" << b_iden << endl;
	}
	else if (type == "LSS") {
		//st.push(int2str(a < b));
		fmiddle << a_iden << "<" << b_iden << endl;
	}
	else if (type == "LEQ") {
		//st.push(int2str(a <= b));
		fmiddle << a_iden << "<=" << b_iden << endl;
	}
	else if (type == "GRE") {
		//st.push(int2str(a > b));
		fmiddle << a_iden << ">" << b_iden << endl;
	}
	else if (type == "GEQ") {
		//st.push(int2str(a >= b));
		fmiddle << a_iden << ">=" << b_iden << endl;
	}
}

void emitl(string type, int i) {
	if (type == "LOADI") { //常量压入操作数栈
		//st.push(int2str(i));
		st_iden.push(int2str(i));
	}
}

void emit(string type, Var var) {
	if (type == "LOAD") {
		//st.push(var.name); //?
		st_iden.push(var.name);
	}
	else if (type == "STO") {
		/*var.value = st.top();
		st.pop();*/
	}
}

void emit(string type, string s) {
	//bool b_char = isChar(st.top());
	//bool b_void = isVoid(st.top());
	if (type == "RET") {
		/*if (!b_void && s == "VOIDTK") {
			error(NON_RETURN);
		}
		else if ((b_char || b_void) && s == "INTTK") {
			error(RE_RETURN);
		}
		else if (!b_char && s == "CHARTK") {
			error(RE_RETURN);
		}
		if (b_void) {
			fmiddle << "RET " << endl;
		}
		else {*/
		if (s != "VOIDTK") {
			string b_iden = st_iden.top();
			st_iden.pop();
			fmiddle << "ret " << b_iden << endl;
		}
		else {
			fmiddle << "ret " << endl;
		}
		//}
	}
	else if (type == "BRF") {
		fmiddle << "BZ " << s << endl;
	}
	else if (type == "BR") {
		fmiddle << "GOTO " << s << endl;
	}
	else if (type == "BRT") {
		fmiddle << "BNZ " << s << endl;
	}
}

void add() {
	emit("ADD");
}

void subt() {
	emit("SUBT");
}

void mult() {
	emit("MULT");
}

void div() {
	emit("DIV");
}

void pushi(int i) {
	emitl("LOADI", i);
}

void ret(string fun_type) {
	emit("RET", fun_type);
}

void jump(string label) {
	fmiddle << "GOTO " << label << endl;
}

int index() {
	int res = 0;
	try {
		res = unsign_int();
	}
	catch (string e) {
		error(INDEX);
	}
	judge_rbrack(getsym());
	return res;
}

void initload(string name_content) {
	Var var = getvar(name_content);
	string iden = st_iden.top();
	st_iden.pop();
	fmiddle << name_content << " = " << iden << endl;
	emit("STO", var);
}

string change(Var i_prev, string op, int step) {
	int value_pre = atoi(i_prev.value.c_str());
	fmiddle << i_prev.name << " = " << i_prev.name << token.at(op) << step << endl;
	if (op == "PLUS") {
		return int2str(value_pre + step);
	}
	else if (op == "MINU") {
		return int2str(value_pre - step);
	}
}

void checktype(int& matched, map<int, Var>::iterator& iter) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	if (matched < 1) {
		error(FUN_NUM_UNMATCH);
		return;
	}
	string t = "INTTK";
	if (isChar(st.top())) {
		t = "CHARTK";
	}
	//调用函数参数入栈
	if (t != iter->second.type) {
		error(FUN_TYPE_UNMATCH);
	}
	matched--;
	iter++;
	return;
}

void checklen(int matched) {
	if (matched != 0) {
		error(FUN_NUM_UNMATCH);
	}
}

void push() {
	string iden = st_iden.top();
	fmiddle << "push " << iden << endl;
}

void call(string fun_name) {
	fmiddle << "call " << fun_name << endl;
}

void assign(Var& var) {
	//string b = st.top();
	//st.pop();
	string b_iden = st_iden.top();
	st_iden.pop();
	//var.value = b;
	fmiddle << var.name << " = " << b_iden << endl;
}

void assign(string array_element) {
	string right = st_iden.top();
	st_iden.pop();
	fetch_array(array_element);
	string left = st_iden.top();
	st_iden.pop();
	fmiddle << left << " = " << right << endl;;
}

void print_const(Var var) {
	fmiddle << "const " << token.at(var.type) << " " << var.name << " = " << var.value << endl;
}

void print_var(string type, string name_content) {
	fmiddle << "var " << token.at(type) << " " << name_content << endl;
}

void set_value(Var& var) {
	if (var.type == "INTTK") {
		try {
			var.value = int2str(integerNum());
		}
		catch (string e) {
			error(CONST_DEFINE);
		}
	}
	else if (var.type == "CHARTK") {
		if (strcmp(getsym(), "CHARCON") != 0) {
			error(CONST_DEFINE);
		} //'char'
		var.value = "'" + name + "'";
	}
}

void print_fun(Func fun) {
	fmiddle << token.at(fun.type) << " " << fun.name << "()" << endl;
	for (auto iter = fun.parameter.begin(); iter != fun.parameter.end(); iter++) {
		fmiddle << "para " << token.at(iter->second.type) << " " << iter->second.name << endl;
	}
}

void write(string content) {
	fmiddle << "printf " << "\"" << content  << "\"" << endl;
}

void write() {
	string iden = st_iden.top();
	st_iden.pop();
	fmiddle << "printf " << iden << endl;
}

void write(string content, string iden) {
	fmiddle << "printf " << "\"" << content << "\", " << iden << endl;
}

void read(string var_name) {
	fmiddle << "scanf " << var_name << endl;
}

//void fetch_array(string name_content, int index) {
//	fmiddle << name_content << "[" << index << "]" << endl;
//}

void fetch_array(string name_content) {
	string iden = st_iden.top();
	st_iden.pop();
	st_iden.push(name_content + "[" + iden + "]");
}

void print_array(Array a) {
	fmiddle << "var " << token.at(a.type) << " " << a.name << "[" << a.size << "]" << endl;
}