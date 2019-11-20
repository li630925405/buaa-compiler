#pragma once

#define ILLEGAL 'a'
#define RENAME 'b'
#define UNNAME 'c'
#define FUN_NUM_UNMATCH 'd'
#define FUN_TYPE_UNMATCH 'e'
#define REQUIRE_ILLEGAL 'f'
#define NON_RETURN 'g'
#define RE_RETURN 'h'
#define INDEX 'i'
#define CONST_CHANGE 'j'
#define SEMI 'k'
#define RPARENT 'l'
#define RBRACK 'm'
#define DO_WHILE 'n'
#define CONST_DEFINE 'o'

void error_semi();
void new_line();
void jump_till(string str);
void readString();
Var assign_judge(string name_content);
Var judge_unname(string name_content);
void judge_rparent(string sym);
void judge_semi(string sym);
void judge_parse(string sym, string should);
void judge_while(string sym);
void judge_rbrack(string sym);
void judge_illegal(char* sym, string& type, string& name_content);