#include "judge.h"

bool isOpe(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/';
}

int isTypeIden(const char* str) {
	return strcmp(str, "INTTK") == 0
		|| strcmp(str, "CHARTK") == 0;
}

int isFunType(string str) {
	return str == "INTTK" || str == "CHARTK" || str == "VOIDTK";
}

int isMultDiv(const char* str) {
	return strcmp(str, "MULT") == 0
		|| strcmp(str, "DIV") == 0;
}

int isRelaString(const char* str) {
	return strcmp(str, "LSS") == 0
		|| strcmp(str, "LEQ") == 0
		|| strcmp(str, "GRE") == 0
		|| strcmp(str, "GEQ") == 0
		|| strcmp(str, "EQL") == 0
		|| strcmp(str, "NEQ") == 0;
}

int isPlusMinus(const char* str) {
	return strcmp(str, "PLUS") == 0
		|| strcmp(str, "MINU") == 0;
}

int isPlusMinus(char c) {
	return c == '+' || c == '-';
}

int isRetFun(string str) {
	if (debug == 1) {
		printf("%s\n", __func__);
		if (pause)
			system("pause");
	}
	string type = func.at(str).type;
	return type == "INTTK" || type == "CHARTK";
}

int isNonretFun(string str) {
	if (debug == 1) {
		printf("%s\n", __func__);
		if (pause)
			system("pause");
	}
	return func.at(str).type == "VOIDTK";
}

int isUnsignNum(const char* str) {
	const char* head = str;
	for (int i = 0; i < strlen(str); i++) {
		if (!isNum(*head)) {
			return 0;
		}
		head++;
	}
	return 1;
}


int isParents(char c) {
	return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

int isRelaChar(char c) {
	return c == '<' || c == '>' || c == '=' || c == '!';
}

int isLetter(char c) {
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isNum(char c) {
	return c >= '0' && c <= '9';
}

bool is_jump(string sym) {
	return sym == "BZ" || sym == "GOTO" || sym == "BNZ";
}

int isLegal(char c) {
	return isOpe(c) || isRelaChar(c) || isLetter(c) ||
		isNum(c) || isParents(c) || c == ',' || c == ';' || c == '"' || c == '\'';
}

bool is_var(string sym) {
	return sym == "VARTK" || sym == "CONSTTK";
}