#pragma once
#include "lib.h"
#include "common.h"
#include "error.h"
#include "emit.h"
#include "Array.h"

extern map<string, Func> func;
extern map<string, Var> global_var_table;
extern map<string, Var> global_const_table;
extern map<string, Var> fun_var_table;
extern map<string, Var> fun_const_table;

void readString();
void const_define(bool fun);
void const_state(bool fun);
int unsign_int();
int integerNum();
void var_define(string type, bool fun); //int/char IDENFR
void var_state(string type, bool fun);
void state_head();
void program();
void returnFun_define(Func& fun);
void nonreturnFun_define(Func& fun);
void com_statement(bool fun);
void paratable(Func& fun);
void MAIN();
void expression();
void term();
void factor();
void statement();
void assign_statement(string name_content);
void if_statement();
bool require();
void while_statement();
void return_call(Func fun);
void nonreturn_call(Func fun);
void value_paratable(Func fun);
void exprs(int matched, map<int, Var>::iterator iter);
void statement_list();
void read_statement();
void write_statement();
void return_statement();
int step_size();
int isFunType(string str);
int isTypeIden(const char* str);
int isMultDiv(const char* str);
int isRelaString(const char* str);
int isPlusMinus(const char* str);
int isPlusMinus(char c);
int isRetFun(string str);
int isNonretFun(string str);
//int isNum(char c);
int isUnsignNum(const char* str);
bool isChar(string s);
void new_line();
void error_semi();
void jump_till(string str);
void insert_fun(string type, Func fun);
void call_fun(string name);
void insert_table(string type, string name, map<string, Var>& table);
void insert_const(string type, string name_content, bool fun);
void insert_var(string type, string name_content, bool fun);

bool contain_var(string name_content);
Var getvar(string name_content);
Array get_array(string name_content);
string get_array_value(string name_content);
void insert_array(string type, string name_content);


void generate();