#pragma once
#include "Func.h"
#include "Var.h"
#include "Array.h"
#include "judge.h"

using namespace std;

void backtrack(int times);
const char* getsym();
void error(char c);
char reread();
int isNum(char c);
string int2str(int a);

extern FILE* in;
extern FILE* out;
extern FILE* err;
extern FILE* mips;
extern string name;
extern string pre_name;
extern ofstream fmiddle;
extern ofstream fmips;

#define maxLength 1000
#define windows 0
#define debug 0
#define pause 0

extern FILE* in;
extern FILE* out;
extern FILE* err;
extern FILE* middle;
extern FILE* mips;
extern string name;

extern map<string, Func> func;
extern map<string, Var> global_var_table;
extern map<string, Var> global_const_table;
extern map<string, Var> fun_var_table;
extern map<string, Var> fun_const_table;

extern int line;
extern int is_newline[3];