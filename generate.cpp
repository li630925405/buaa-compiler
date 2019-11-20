#include "lib.h"
#include "common.h"
#include "lexing.h"
#include "grammar.h"

enum rela { EQL, NEQ, LSS, LEQ, GRE, GEQ };
map<string, rela> relaMap = { {"EQL", EQL}, {"NEQ", NEQ}, {"LSS", LSS}, {"LEQ", LEQ}, {"GRE", GRE}, {"GEQ", GEQ} };

const char* s_reg[8] = { "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7" };
int s_used = 0;
const char* t_reg[10] = { "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9" };
int t_used = 0;
const char* a_reg[4] = { "$a0", "$a1", "$a2", "$a3" };
int a_used = 0;
map<string, string> regMap;
map<string, string> fun_regMap;
map<string, int> stackMap;
int loc_num = 4;

void macro();
void save_var(string type);
void save_fun();
void mips_add(string left, string right1, string right2);
void mips_sub(string left, string right1, string right2);
void mips_mult(string left, string right1, string right2);
void mips_div(string left, string right1, string right2);
void li(string reg, int i);
void lw(string rt, string offset, string base);
void mips_expr(string left, string right1, string op);
void move(string rt, string rs);
void compare(string rs);
void assign_comp();
void assign_array(string base, string offset);
void gen_assign(string left);
void mips_ret();
void mips_jump();
void mips_print();
void mips_scanf();
void mips_push();
void mips_call();
void generate();
string char2str(char c);
string alloc_treg();
string alloc_sreg();
string name2reg(string name_content);

void macro() {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, ".macro read_char(%%x)\n");
	fprintf(mips, "    li $v0 12\n");
	fprintf(mips, "    syscall\n");
	fprintf(mips, "    move %%x $v0\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro read_int(%%x)\n");
	fprintf(mips, "    li $v0 5\n");
	fprintf(mips, "    syscall\n");
	fprintf(mips, "    move %%x $v0\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro print_int(%%x)\n");
	fprintf(mips, "    push($a0)\n");
	fprintf(mips, "    move $a0 %%x\n");
	fprintf(mips, "    li $v0 1\n");
	fprintf(mips, "    syscall\n");
	fprintf(mips, "    pop($a0)\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro print_char(%%x)\n");
	fprintf(mips, "    push($a0)\n");
	fprintf(mips, "    move $a0 %%x\n");
	fprintf(mips, "    li $v0 11\n");
	fprintf(mips, "    syscall\n");
	fprintf(mips, "    pop($a0)\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro print_str(%%str)\n");
	fprintf(mips, "    push($a0)\n");
	fprintf(mips, "    li $v0 4\n");
	fprintf(mips, ".data\n");
	fprintf(mips, "mylabel:.asciiz %%str\n");
	fprintf(mips, ".text\n");
	fprintf(mips, "    la $a0 mylabel\n");
	fprintf(mips, "    syscall\n");
	fprintf(mips, "    pop($a0)\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro push(%%x)\n");
	fprintf(mips, "    sw %%x ($sp)\n");
	fprintf(mips, "    subi $sp $sp 4\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro pop(%%x)\n");
	fprintf(mips, "    addi $sp $sp 4\n");
	fprintf(mips, "    lw %%x ($sp)\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro exit\n");
	fprintf(mips, "    li $v0 10\n");
	fprintf(mips, "    syscall\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".macro offset(%%x)\n");
	fprintf(mips, "    push($t0)\n");
	fprintf(mips, "    li $t0 4\n");
	fprintf(mips, "    mult $t0 %%x\n");
	fprintf(mips, "    mflo %%x\n");
	fprintf(mips, "    pop($t0)\n");
	fprintf(mips, ".end_macro\n");
	fprintf(mips, ".text\n");
}

string char2str(char c) {
	string s;
	s[0] = c;
	s[1] = 0;
	return s;
}

void save_var(string type) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	if (type == "CONSTTK") {
		string sym = getsym(); // int or char
		getsym(); // name
		string var_name = name;
		getsym(); // =
		string reg = alloc_sreg();
		if (sym == "INTTK") {
			int value = integerNum();
			fprintf(mips, "li %s %d\n", reg.c_str(), value);
		}
		else if (sym == "CHARTK") {
			getsym();
			string value = name;
			fprintf(mips, "li %s \'%s\'\n", reg.c_str(), value.c_str());
		}
		regMap.insert({ var_name, reg });
	}
	else if (type == "VARTK") {
		getsym(); //int or char
		getsym(); //name
		string var_name = name;
		if (strcmp(getsym(), "LBRACK") == 0) {
			getsym(); //int
			string size = name;
			fprintf(mips, ".data\n");
			fprintf(mips, "%s: .space %d\n", var_name.c_str(), atoi(size.c_str()) * 4);
			fprintf(mips, ".text\n");
			getsym(); // ]
		}
		else {
			backtrack(1);
		}
	}
	char* tmp = (char*)malloc(100);
	fgets(tmp, 100, in);
	for (int i = 0; i < 3; i++) {
		is_newline[i] = true;
	}
	line++;  //new_line 不对
	//cout << "tmp: " << tmp << endl;
}

string reg2name(string reg) {
	for (auto iter = regMap.begin(); iter != regMap.end(); iter++) {
		if (iter->second == reg) {
			return iter->first;
		}
	}
	return "not found";
}

string alloc_areg() {
	string res = a_reg[a_used];
	a_used = (a_used + 1)/* % 4*/;
	if (a_used >= 4) {
		fprintf(mips, "sw %s %d($sp)\n", a_reg[a_used % 4], loc_num);
		regMap.erase(reg2name(a_reg[a_used % 4]));
		stackMap.insert({ res, loc_num });
		loc_num += 4;
	}
	return res;
}

void save_fun() {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	getsym();
	string fun_name = name;
	fprintf(mips, "\n%s:\n", fun_name.c_str());
	Func fun = func.at(fun_name);
	for (auto iter = fun.parameter.begin(); iter != fun.parameter.end(); iter++) {
		string reg = alloc_areg();
		fprintf(mips, "pop(%s)\n", reg.c_str());
		fun_regMap.insert({ iter->second.name, reg });
	}
	getsym(); //(
	getsym(); //)
	string sym = getsym();
	while (sym == "PARATK") {
		new_line();
		sym = getsym();
	}
	backtrack(1);
}

void mips_addi(string rt, string rs, int imm) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "addi %s %s %d\n", rt.c_str(), rs.c_str(), imm);
}

void mips_subi(string rt, string rs, int imm) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "subi %s %s %d\n", rt.c_str(), rs.c_str(), imm);
}

void mips_add(string left, string right1, string right2) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "add %s %s %s\n", left.c_str(), right1.c_str(), right2.c_str());
}

void mips_sub(string left, string right1, string right2) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "sub %s %s %s\n", left.c_str(), right1.c_str(), right2.c_str());
}

void mips_mult(string left, string right1, string right2) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "mult %s %s\n", right1.c_str(), right2.c_str());
	fprintf(mips, "mflo %s\n", left.c_str());
}

void mips_div(string left, string right1, string right2) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "div %s %s\n", right1.c_str(), right2.c_str());
	fprintf(mips, "mflo %s\n", left.c_str());
}

void li(string reg, int i) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "li %s %d\n", reg.c_str(), i);
}

void lw(string rt, string offset, string base) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "lw %s %s(%s)\n", rt.c_str(), offset.c_str(), base.c_str());
}

void sw(string rt, string offset, string base) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "sw %s %s(%s)\n", rt.c_str(), offset.c_str(), base.c_str());
}

void mips_expr(string left, string right1, string op) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	string sym = getsym();
	string right2 = name;
	if (sym == "INTCON") {
		int right2_int = atoi(right2.c_str());
		if (op == "+") {
			mips_addi(left, right1, right2_int);
		}
		else if (op == "-") {
			mips_subi(left, right1, right2_int);
		}
		else if (op == "*") {
			string reg = alloc_treg();
			li(reg, right2_int);
			mips_mult(left, reg, right1);
		}
		else if (op == "/") {
			string reg = alloc_treg();
			li(reg, right2_int);
			mips_div(left, right1, reg);
		}
	}
	else if (sym == "IDENFR") {
		if (op == "+") {
			mips_add(left, right1, name2reg(right2));
		}
		else if (op == "-") {
			mips_sub(left, right1, name2reg(right2));
		}
		else if (op == "*") {
			mips_mult(left, right1, name2reg(right2));
		}
		else if (op == "/") {
			mips_div(left, right1, name2reg(right2));
		}
	}
}

void mips_expr(string left, int right1, string op) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	string sym = getsym();
	string right2 = name;
	if (sym == "IDENFR") {
		if (op == "+") {
			mips_addi(left, name2reg(right2), right1);
		}
		else if (op == "-") {
			mips_subi(left, name2reg(right2), right1);
		}
		else if (op == "*") {
			string reg = alloc_treg();
			li(reg, right1);
			mips_mult(left, reg, name2reg(right2));
		}
		else if (op == "/") {
			string reg = alloc_treg();
			li(reg, right1);
			mips_div(left, reg, name2reg(right2));
		}
	}
	else if (sym == "INTCON") {
		int right2_int = atoi(right2.c_str());
		if (op == "+") {
			int mid = right2_int + right1;
			li(left, mid);
		}
		else if (op == "-") {
			int mid = right1 - right2_int;
			li(left, mid);
		}
		else if (op == "*") {
			int mid = right2_int * right1;
			li(left, mid);
		}
		else if (op == "/") {
			int mid = right1 / right2_int;
			li(left, mid);
		}
	}
}

void move(string rt, string rs) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "move %s %s\n", rt.c_str(), rs.c_str());
}

void compare(string rs) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	string rela = getsym();
	string sym = getsym();
	string rt;
	if (sym == "INTCON") {
		rt = alloc_treg();
		li(rt, atoi(name.c_str()));
	}
	else {
		rt = name2reg(name);
	}
	string jump = getsym();
	getsym();
	string label = name;
	if (jump == "BZ") {
		switch (relaMap.at(rela)) {
		case EQL:
			fprintf(mips, "bne %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case NEQ:
			fprintf(mips, "beq %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case LSS:
			fprintf(mips, "bge %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case LEQ:
			fprintf(mips, "bgt %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case GRE:
			fprintf(mips, "ble %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case GEQ:
			fprintf(mips, "blt %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		}
	}
	else if (jump == "BNZ") {
		switch (relaMap.at(rela)) {
		case EQL:
			fprintf(mips, "beq %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case NEQ:
			fprintf(mips, "bne %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case LSS:
			fprintf(mips, "blt %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case LEQ:
			fprintf(mips, "ble %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case GRE:
			fprintf(mips, "bgt %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		case GEQ:
			fprintf(mips, "bge %s %s %s\n", rs.c_str(), rt.c_str(), label.c_str());
			break;
		}
	}
}

string alloc_treg() {
	string res = t_reg[t_used];
	t_used = (t_used + 1)/* % 10*/;
	if (t_used >= 10) {
		throw "out_size";
	}
	return res;
}

string alloc_sreg() {
	string res = s_reg[s_used];
	s_used = (s_used + 1)/* % 8*/;
	if (s_used >= 8) {
		throw "out_size";
	}
	return res;
}

void assign_comp() {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	getsym();
	string left = name;
	string sym = getsym();
	if (sym == "ASSIGN") {
		gen_assign(left);
	}
	else if (isRelaString(sym.c_str())) {
		backtrack(1);
		compare(name2reg(left));
	}
	else if (sym == "LBRACK") {
		getsym();
		string base = name;
		assign_array(left, name2reg(base));
	}
}

bool name_saved(string name_content) {
	return fun_regMap.find(name_content) != fun_regMap.end()
		|| regMap.find(name_content) != regMap.end();
}

string name2reg(string name_content) {
	if (name_content == "RET") {
		return "$v0";
	}
	if (fun_regMap.find(name_content) == fun_regMap.end()) {
		if (regMap.find(name_content) == regMap.end()) {
			if (stackMap.find(name_content) == stackMap.end()) {
				cout << name_content << endl;
				cout << "??????????" << endl;
				return "$0";
			}
			else {
				string reg = alloc_treg();
				lw(reg, int2str(stackMap.at(name_content)), "$sp");
				stackMap.erase(name_content);
			}
		}
		else {
			return regMap.at(name_content);
		}
	}
	else {
		return fun_regMap.at(name_content);
	}
}

void assign_array(string offset, string base) {
	string reg = alloc_treg();
	li(reg, 2);
	mips_mult(base, reg, base);
	getsym(); //]
	string sym = getsym();
	if (sym == "ASSIGN") {
		sym = getsym();
		string right1 = name;
		getsym();
		string op = name;
		string rt = alloc_treg();
		if (sym == "IDENFR") {
			if (isOpe(op[0])) {
				mips_expr(rt, name2reg(right1), op);
			}
			else {
				move(rt, name2reg(right1));
				backtrack(1);
			}
		}
		else if (sym == "INTCON") {
			int right1_int = atoi(right1.c_str());
			if (isOpe(op[0])) {
				mips_expr(rt, right1_int, op);
			}
			else {
				li(rt, right1_int);
				backtrack(1);
			}
		}
		sw(rt, offset, base);
	}
	else if (isRelaString(sym.c_str())) {
		backtrack(1);
		string tmp = alloc_treg();
		lw(tmp, offset, base);
		compare(tmp);
	}
}

void gen_assign(string left) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	if (regMap.find(left) == regMap.end()) {
		string reg = alloc_treg();
		regMap.insert({ left, reg });
	}
	string sym = getsym();
	string right1 = name;
	getsym();
	string op = name;
	if (sym == "IDENFR") {
		if (isOpe(op[0])) {
			mips_expr(name2reg(left), name2reg(right1), op);
		}
		else {
			move(name2reg(left), name2reg(right1));
			backtrack(1);
		}
	}
	else if (sym == "INTCON") {
		int right1_int = atoi(right1.c_str());
		if (isOpe(op[0])) {
			mips_expr(name2reg(left), right1_int, op);
		}
		else {
			li(name2reg(left), right1_int);
			backtrack(1);
		}
	}
}

void mips_ret() {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	int cur_line = line;
	getsym();
	int next_line = line;
	if (cur_line == next_line) {
		string v0 = name;
		fprintf(mips, "move $v0 %s\n", name2reg(v0).c_str());
	}
	else {
		backtrack(1);
	}
	fun_regMap.clear();
	fprintf(mips, "jr $ra\n");
}

void mips_jump() {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	getsym();
	string label = name;
	fprintf(mips, "j %s\n", label.c_str());
}

void mips_print() {
	string sym = getsym();
	if (sym == "STRCON") {
		fprintf(mips, "print_str(\"%s\")\n", name.c_str());
	}
	else if (sym == "IDENFR") {
		Var var = getvar(name);
		if (var.type == "CHARTK") {
			fprintf(mips, "print_char(%s)\n", name2reg(name).c_str());
		}
		else {
			//如t1 是否没有存在var里
			fprintf(mips, "print_int(%s)\n", name2reg(name).c_str()); //value 2 reg
		}
	}
	sym = getsym();
	if (sym == "END") {
		fprintf(mips, "print_str(\"\\n\")\n");
		return;
	}
	if (sym == "COMMA") {
		getsym();
		Var var = getvar(name);
		if (var.type == "CHARTK") {
			fprintf(mips, "print_char(%s)\n", name2reg(name).c_str());
		}
		else {
			//如t1 是否没有存在var里
			fprintf(mips, "print_int(%s)\n", name2reg(name).c_str()); //value 2 reg
		}
	}
	else {
		backtrack(1);
	}
	fprintf(mips, "print_str(\"\\n\")\n");
}

void mips_scanf() {
	string sym = getsym();
	Var var = getvar(name);
	string reg;
	if (name_saved(name)) {
		reg = name2reg(name);
	}
	else {
		reg = alloc_treg();
		regMap.insert({ name, reg });
	}
	if (var.type == "CHARTK") {
		fprintf(mips, "read_char(%s)\n", reg.c_str());
	}
	else if (var.type == "INTTK") {
		fprintf(mips, "read_int(%s)\n", reg.c_str());
	}
}

void mips_push() {
	string sym = getsym();
	string reg;
	if (sym == "INTCON") {
		reg = alloc_treg();
		li(reg, atoi(name.c_str()));
	}
	else if (sym == "IDENFR") {
		reg = name2reg(name);
	}
	fprintf(mips, "push(%s)\n", reg.c_str());
}

void mips_call() {
	getsym();
	string fun_name = name;
	fprintf(mips, "jal %s\n", fun_name.c_str());
}

void mips_main() {
	fprintf(mips, "\nmain:\n");
}

void generate() {
	line = 1;
	bool first_fun = true;
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	macro();
	in = fopen("middle.txt", "r");
	string sym;
	while (1) {
		try {
			sym = getsym();
		}
		catch (exception e) {
			break;
		}
		//cout << "-----------sym: " << sym << "name: " << name << "-------------" << endl;
		//cout << "line: " << line << endl;
		if (is_var(sym)) {
			save_var(sym);
		}
		else if (isFunType(sym.c_str())) {
			if (first_fun) {
				first_fun = false;
				fprintf(mips, "\njal main\n");
				fprintf(mips, "li $v0 10\n");
				fprintf(mips, "syscall\n");
			}
			save_fun();
		}
		else if (sym == "IDENFR") {
			char c = fgetc(in);
			if (c == ':') {
				fprintf(mips, "%s:\n", name.c_str());
			}
			else {
				fseek(in, -1, SEEK_CUR);
				backtrack(1);
				assign_comp();
			}
		}
		else if (sym == "RETTK") {
			mips_ret();
		}
		else if (sym == "GOTO") {
			mips_jump();
		}
		else if (sym == "PRINTFTK") {
			mips_print();
		}
		else if (sym == "SCANFTK") {
			mips_scanf();
		}
		else if (sym == "PUSHTK") {
			mips_push();
		}
		else if (sym == "CALLTK") {
			mips_call();
		}
		else if (sym == "END") {
			break;
		}
		else if (sym == "MAINTK") {
			if (first_fun) {
				first_fun = false;
				fprintf(mips, "\njal main\n");
				fprintf(mips, "li $v0 10\n");
				fprintf(mips, "syscall\n");
			}
			mips_main();
		}
	}
	fprintf(mips, "jr $ra\n");
}