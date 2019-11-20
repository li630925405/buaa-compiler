#pragma once

#include <string>
// ²»ÄÜinclude"lib.h"?

extern stack<string> st;
extern stack<string> st_iden;

using namespace std;

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

void emit(string type, string fun_type);
void ret(string fun_type);
void jump(string label);
int index();

void brf(string labx);
void br(string labx);
void brt(string labx);
void labprod(string labx);
void initload(string name_content);
string change(Var i_prev, string op, int step);
void checktype(int &matched, map<int, Var>::iterator &iter);
void checklen(int matched);
void push();
void call(string fun_name);
void assign(Var& var);
void print_const(Var var);
void print_var(string type, string name_content);
void set_value(Var& var);
void print_fun(Func fun);
void write(string content);
void write();
void write(string content, string iden);
void read(string var_name);
string gen_lab();
void fetch_array(string name_content);
//void fetch_array(string name_content, int index);
void print_array(Array a);
void assign(string array_element);
