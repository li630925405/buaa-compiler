#pragma once
#include "lib.h"
#include "common.h"

bool isOpe(char c);
int isTypeIden(const char* str);
int isFunType(string str);
int isMultDiv(const char* str);
int isRelaString(const char* str);
int isPlusMinus(const char* str);
int isPlusMinus(char c);
int isRetFun(string str);
int isNonretFun(string str);
int isUnsignNum(const char* str);
int isParents(char c);
int isRelaChar(char c);
int isLetter(char c);
int isNum(char c);
int isLegal(char c);
bool is_jump(string sym);
bool is_var(string sym);