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
map<string, int> stackMap;
int loc_num = 4;

bool glob = true;
int str_num = 0;

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
void assign_array(string base);
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
//string alloc_sreg();
//string alloc_sreg();
string addreg(string name_content);
void erase_treg();

string addreg(string name_content) {
		if (name_content == "RET") {
		return "$v0";
	}
	string reg;
	if (contain_var(name_content)) {
		reg = alloc_treg();
		if (stackMap.find(name_content) != stackMap.end()) {
			int locate = stackMap.find(name_content)->second;
			fprintf(mips, "lw %s %d($fp)\n", reg.c_str(), locate);
		}
		else {
			fprintf(mips, "lw %s %s\n", reg.c_str(), name_content.c_str());
		}
	}
	else {
		if (regMap.find(name_content) == regMap.end()) {
			cout << name_content << endl;
			cout << "??????????" << endl;
			reg = t_reg[t_used];
			regMap.insert({ name_content, reg.c_str() });
			t_used = (t_used + 1) % 10;
		}
		else {
			reg = regMap.at(name_content);
		}
	}
	return reg;
}

string char2str(char c) {
	string s;
	s[0] = c;
	s[1] = 0;
	return s;
}

void store_var(string name_content, int value, int loc) {
	string reg = t_reg[t_used];
	fprintf(mips, "li %s %d\n", reg.c_str(), value);
	fprintf(mips, "sw %s %d($fp)\n", reg.c_str(), loc);
}

void store_var(string name_content, string value, int loc) {
	string reg = t_reg[t_used];
	fprintf(mips, "li %s \'%s\'\n", reg.c_str(), value.c_str());
	fprintf(mips, "sw %s %d($fp)\n", reg.c_str(), loc);
}

void save_var(string type) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	string var_type;
	if (glob) {
		while (is_var(type)) {
			fprintf(mips, ".data\n");
			if (type == "CONSTTK") {
				var_type = getsym(); // int or char
				getsym(); // name
				string var_name = name;
				getsym(); // =
				string value;
				if (var_type == "INTTK") {
					value = int2str(integerNum());
				}
				else if (var_type == "CHARTK") {
					getsym();
					string c = name;
					value = "\'" + c + "\'";
				}
				fprintf(mips, "		%s: .space 4\n", var_name.c_str());
				fprintf(mips, ".text\n");
				string tmp_reg = t_reg[t_used];
				fprintf(mips, "li %s %s\n", tmp_reg.c_str(), value.c_str());
				fprintf(mips, "sw %s %s\n", tmp_reg.c_str(), var_name.c_str());
			}
			else if (type == "VARTK") {
				getsym(); // int or char
				getsym(); //name
				string var_name = name;
				if (strcmp(getsym(), "LBRACK") == 0) {
					getsym(); //int
					string size = name;
					fprintf(mips, "		%s: .space %d\n", var_name.c_str(), atoi(size.c_str()) * 4);
					getsym(); // ]
				}
				else {
					fprintf(mips, "		%s: .space 4\n", var_name.c_str());
					backtrack(1);
				}
			}
			type = getsym();
		}
		fprintf(mips, ".text\n");
		backtrack(1);
		return;
	}
	if (type == "CONSTTK") {
		string sym = getsym(); // int or char
		getsym(); // name
		string var_name = name;
		getsym(); // =
		if (type == "INTTK") {
			integerNum();
		}
		else if (type == "CHARTK") {
			getsym();
		}
		stackMap.insert({ var_name, loc_num });
	}
	else if (type == "VARTK") {
		getsym(); //int or char
		getsym(); //name
		string var_name = name;
		if (strcmp(getsym(), "LBRACK") == 0) {
			getsym(); //int
			string size = name;
			stackMap.insert({ var_name, loc_num });
			loc_num += 4 * atoi(size.c_str());
			getsym(); // ]
		}
		else {
			stackMap.insert({ var_name, loc_num });
			loc_num += 4;
			backtrack(1);
		}
	}
	string sym = getsym();
	if (is_var(sym)) {
		save_var(sym);
	}
	else {
		fprintf(mips, "subi $sp $sp %d\n", loc_num + 8);
		fprintf(mips, "sw	$fp %d($sp)\n", loc_num + 4);
		fprintf(mips, "move	$fp $sp\n");
		for (auto iter = stackMap.begin(); iter != stackMap.end(); iter++) {
			string var_name = iter->first;
			string type = getvar(var_name).type;
			if (type == "INTTK") {
				store_var(var_name, atoi(getvar(var_name).value.c_str()), loc_num);
				loc_num += 4;
			}
			else if (type == "CHARTK") {
				getsym();
				string value = name;
				store_var(var_name, getvar(var_name).value, loc_num);
				loc_num += 4;
			}
		}
		loc_num = 0;
		backtrack(1);
	}
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
		fprintf(mips, "sw %s %d($fp)\n", a_reg[a_used % 4], loc_num);
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
		regMap.insert({ iter->second.name, reg });
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

void lw(string rt, int offset, string base) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "lw %s %s(%s)\n", rt.c_str(), offset, base.c_str());
}

void lw(string rt, string offset, string base) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "lw %s %s(%s)\n", rt.c_str(), offset.c_str(), base.c_str());
}

void sw(string rt, int offset, string base) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	fprintf(mips, "sw %s %d(%s)\n", rt.c_str(), offset, base.c_str());
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
			string reg = t_reg[t_used];
			li(reg, right2_int);
			mips_mult(left, reg, right1);
		}
		else if (op == "/") {
			string reg = t_reg[t_used];
			li(reg, right2_int);
			mips_div(left, right1, reg);
		}
	}
	else if (sym == "IDENFR") {
		if (op == "+") {
			string tmp_reg = addreg(right2);
			mips_add(left, right1, tmp_reg);
		}
		else if (op == "-") {
			string tmp_reg = addreg(right2);
			mips_sub(left, right1, tmp_reg);
		}
		else if (op == "*") {
			string tmp_reg = addreg(right2);
			mips_mult(left, right1, tmp_reg);
		}
		else if (op == "/") {
			string tmp_reg = addreg(right2);
			mips_div(left, right1, tmp_reg);
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
			string tmp_reg = addreg(right2);
			mips_addi(left, tmp_reg, right1);
		}
		else if (op == "-") {
			string tmp_reg = addreg(right2);
			mips_subi(left, tmp_reg, right1);
		}
		else if (op == "*") {
			string reg = t_reg[t_used];
			li(reg, right1);
			string tmp_reg = addreg(right2);
			mips_mult(left, reg, tmp_reg);
		}
		else if (op == "/") {
			string reg = t_reg[t_used];
			li(reg, right1);
			string tmp_reg = addreg(right2);
			mips_div(left, reg, tmp_reg);
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

void compare(string left) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	string rs;
	string rt;
	rs = addreg(left);
	string rela = getsym();
	string sym = getsym();
	string right = name;
	if (sym == "INTCON") {
		rt = t_reg[t_used];
		li(rt, atoi(name.c_str()));
	}
	else {
		rt = addreg(right);
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
	if (contain_var(right)) {
		erase_treg();
	}
	if (contain_var(left)) {
		erase_treg();
	}
}

string alloc_treg() {
	string res = t_reg[t_used];
	// 只有中间变量存在t寄存器中，生命周期很短
	t_used = (t_used + 1) % 10;
	//if (t_used >= 10) {
	//	throw "out_size";
	//}
	return res;
}

//string alloc_sreg() {
//  string res = s_reg[s_used];
//  s_used = (s_used + 1)/* % 8*/;
//  if (s_used >= 8) {
//      throw "out_size";
//  }
//  return res;
//}

void assign_comp() {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	getsym();
	string left = name;
	string sym = getsym();
	string reg;
	if (sym == "ASSIGN") {
		if (!contain_var(left)) {
			reg = addreg(left); //下面还会用
			gen_assign(reg);
		}
		else {
			reg = alloc_treg();
			gen_assign(reg);
			if (stackMap.find(left) != stackMap.end()) {
				int locate = stackMap.find(left)->second;
				sw(reg, locate, "$fp");
			}
			else {
				fprintf(mips, "sw %s %s\n", reg.c_str(), left.c_str());
			}
			erase_treg();  //以后不会再用.变量再用再取
		}
	}
	else if (isRelaString(sym.c_str())) {
		backtrack(1);
		compare(left);
	}
	else if (sym == "LBRACK") {
		assign_array(left);
	}
}

//string name2reg(string name_content) {
//  if (name_content == "RET") {
//      return "$v0";
//  }
//  if (fun_regMap.find(name_content) == fun_regMap.end()) {
//      if (regMap.find(name_content) == regMap.end()) {
//          cout << name_content << endl;
//          cout << "??????????" << endl;
//          return "$0";
//      }
//      else {
//          return regMap.at(name_content);
//      }
//  }
//  else {
//      return fun_regMap.at(name_content);
//  }
//}

void erase_treg() {
	t_used--;
}

void assign_array(string array_name) {
	getsym();
	int index = atoi(name.c_str());
	string reg = alloc_treg();
	if (stackMap.find(array_name) != stackMap.end()) {
		int locate = stackMap.find(array_name)->second + index * 4;
		getsym(); //]
		string sym = getsym();
		if (sym == "ASSIGN") {
			gen_assign(reg);
			sw(reg, locate, "$fp");
		}
		else if (isRelaString(sym.c_str())) {
			backtrack(1);
			lw(reg, locate, "$fp");
			compare(reg);
		}
		erase_treg();
	}
	else {
		getsym(); //]
		string sym = getsym();
		if (sym == "ASSIGN") {
			gen_assign(reg);
			string num_reg = t_reg[t_used];
			fprintf(mips, "li %s %d\n", num_reg.c_str(), index * 4);
			fprintf(mips, "sw %s %s(%s)\n", reg.c_str(), array_name.c_str(), num_reg.c_str());
			erase_treg();
		}
		else if (isRelaString(sym.c_str())) {
			backtrack(1);
			string num_reg = t_reg[t_used];
			fprintf(mips, "li %s %d\n", num_reg.c_str(), index * 4);
			fprintf(mips, "lw %s %s(%s)\n", reg.c_str(), array_name.c_str(), num_reg.c_str());
			compare(reg);
			erase_treg();
		}
	}
}

void gen_assign(string reg) {
	if (debug == 2) {
		printf("%s\n", __func__);
	}
	string sym = getsym();
	string right1 = name;
	getsym();
	string op = name;
	if (sym == "IDENFR") {
		string rt;
		rt = addreg(right1);
		if (isOpe(op[0])) {
			mips_expr(reg, rt, op);
		}
		else {
			move(reg, rt);
			backtrack(1);
		}
	}
	else if (sym == "INTCON") {
		int right1_int = atoi(right1.c_str());
		if (isOpe(op[0])) {
			mips_expr(reg, right1_int, op);
		}
		else {
			li(reg, right1_int);
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
		string v0 = addreg(name);
		fprintf(mips, "move $v0 %s\n", v0.c_str());
	}
	else {
		backtrack(1);
	}
	regMap.clear();
	fprintf(mips, "move    $sp $fp\n");
	fprintf(mips, "lw  $fp %d($sp)\n", loc_num + 4);
	fprintf(mips, "addiu   $sp $sp %d\n", loc_num + 8);
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

void print_str(string name_content) {
	fprintf(mips, "li $v0 4\n");
	fprintf(mips, ".data\n");
	fprintf(mips, "mylabel%d:.asciiz \"%s\"\n", str_num, name_content.c_str());
	fprintf(mips, ".text\n");
	fprintf(mips, "la $a0 mylabel%d\n", str_num++);
	fprintf(mips, "syscall\n");
}

void print_linefeed() {
	fprintf(mips, "li $v0 4\n");
	fprintf(mips, "la $a0 line_feed\n");
	fprintf(mips, "syscall\n");
}

void print_char(string name_content) {
	string tmp_reg = addreg(name_content);
	fprintf(mips, "move $a0 %s\n", tmp_reg.c_str());
	fprintf(mips, "li $v0 11\n");
	fprintf(mips, "syscall\n");
}

void print_int(string name_content) {
	string tmp_reg = addreg(name_content);
	fprintf(mips, "move $a0 %s\n", tmp_reg.c_str()); //value 2 reg
	fprintf(mips, "li $v0 1\n");
	fprintf(mips, "syscall\n");
}

void mips_print() {
	string sym = getsym();
	if (sym == "STRCON") {
		print_str(name);
	}
	else if (sym == "IDENFR") {
		Var var = getvar(name);
		if (var.type == "CHARTK") {
			print_char(name);
		}
		else {
			//如t1 是否没有存在var里
			print_int(name);
		}
	}
	sym = getsym();
	if (sym == "END") {
		print_linefeed();
		return;
	}
	if (sym == "COMMA") {
		getsym();
		Var var = getvar(name);
		if (var.type == "CHARTK") {
			print_char(name);
		}
		else {
			//如t1 是否没有存在var里
			print_int(name);
		}
	}
	else {
		backtrack(1);
	}
	print_linefeed();
}

void mips_scanf() {
	string sym = getsym();
	Var var = getvar(name);
	string reg;
	int locate = stackMap.find(name)->second;
	reg = alloc_treg();
	if (var.type == "CHARTK") {
		fprintf(mips, "li $v0 12\n");
	}
	else if (var.type == "INTTK") {
		fprintf(mips, "li $v0 5\n");
	}
	fprintf(mips, "syscall\n");
	fprintf(mips, "move %s $v0\n", reg.c_str());
	erase_treg();
	sw(reg, locate, "$fp");
}

void mips_push() {
	//string sym = getsym();
	//string reg;
	//if (sym == "INTCON") {
	//	reg = alloc_treg();
	//	li(reg, atoi(name.c_str()));
	//}
	//else if (sym == "IDENFR") {
	//	reg = addreg(name);
	//}
	//fprintf(mips, "push(%s)\n", reg.c_str());
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
	in = fopen("middle.txt", "r");
	fprintf(mips, ".data\n");
	fprintf(mips, "line_feed:.asciiz \"\\n\"\n");
	fprintf(mips, "padding:.asciiz \" \"\n");
	fprintf(mips, ".text\n");
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
			stackMap.clear();
			save_var(sym);
		} 
		glob = false;
		if (isFunType(sym.c_str())) {
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