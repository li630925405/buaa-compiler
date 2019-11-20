#include "lexing.h"

FILE* in;
FILE* out;
FILE* err;
FILE* mips;
string name;
string pre_name;

ofstream fmiddle;
ofstream fmips;

// max back step: 3
int step[3] = { 0 };
int last_tell = 0;
int backlen[3] = { 0 };
int is_newline[3] = {0};
int line = 1;

void program();

void error(char c) {
	fprintf(err, "%d %c\n", line, c);
}

char* integer(FILE* in) {
	fseek(in, -1, SEEK_CUR);
	char* res = (char*)malloc(maxLength);
	memset(res, 0, maxLength);
	char* head = res;
	char c = fgetc(in);
	//if (c == '0') {
	//	*res = c;
	//	return res;
	//}
	while (isNum(c)) {
		*res = c;
		res++;
		c = fgetc(in);
	}
	if (head[0] == '0' && strlen(head) != 1) {
		error(ILLEGAL);
	}
	if (!feof(in)) {
		fseek(in, -1, SEEK_CUR);
	}
	return head;
}

char* str(FILE* in) {
	char* res = (char*)malloc(maxLength);
	memset(res, 0, maxLength);
	char* head = res;
	char c = fgetc(in);
	while (c != '"') {
		if (c == '\n') {
			if (windows) {
				fseek(in, -4, SEEK_CUR); 
			} else {
				fseek(in, -3, SEEK_CUR); 
			}
			error(ILLEGAL);
			return head;
		}
		if (c != 32 && c != 33 && (c < 35 || c > 126)) {
			error(ILLEGAL);
		}
		*res = c;
		c = fgetc(in);
		res++;
	}
	return head;
}

const char* ope(FILE* in) {
	fseek(in, -1, SEEK_CUR);
	char c = fgetc(in);
	if (c == '<') {
		c = fgetc(in);
		if (c == '=') {
			return "<=";
		}
		else {
			fseek(in, -1, SEEK_CUR);
			return "<";
		}
	}
	else if (c == '>') {
		c = fgetc(in);
		if (c == '=') {
			return ">=";
		}
		else {
			fseek(in, -1, SEEK_CUR);
			return ">";
		}
	}
	else if (c == '!') {
		fgetc(in);
		return "!=";
		/*c = fgetc(in);
		if (c == '=') {
			return "!=";
		}
		else {
			error();
		}*/
	}
	else if (c == '=') {
		c = fgetc(in);
		if (c == '=') {
			return "==";
		}
		else {
			fseek(in, -1, SEEK_CUR);
			return "=";
		}
	}
}

char* inden(FILE* p) {
	fseek(p, -1, SEEK_CUR);
	char* res;
	//    printf("ftell: %d\n", ftell(p));
	char c = fgetc(p);
	//    printf("ftell: %d\n", ftell(p));
	res = (char*)malloc(maxLength);
	memset(res, 0, maxLength);
	char* head = res;
	int i = 0;
	while (isNum(c) || isLetter(c)) {
		res[i++] = c;
		c = fgetc(p);
	}
	if (!feof(p)) {
		fseek(p, -1, SEEK_CUR);
	}
	return head;
}

void backtrack(int times) {
	if (debug == 1) {
		printf("%s\n", __func__);
	}
	int sl = 0;
	int bl = 0;
	for (int i = 0; i < times; i++) {
		sl += step[i]; 
		bl += backlen[i];
	}
	if (is_newline[times-1]) {
		line -= is_newline[times-1];
		for (int i = 0; i < times; i++) {
			is_newline[i] = 0;
		}
	}
	for (int i = 0; i < 3; i++) {
		step[i] = step[i + 1];
		backlen[i] = backlen[i + 1];
	} // 连续两次backtrack(1)
	name = pre_name; //times == 1
	fseek(in, -sl, SEEK_CUR);
	fseek(out, -bl, SEEK_CUR);
}

char reread() {
	char c = 0;
	c = fgetc(in);
	while (isspace(c)) {
		c = fgetc(in);
	}
	char res = c;
	fseek(in, -1, SEEK_CUR);
	return res;
}

const char* getsym() {
	char c = 0;
	int start = ftell(in);
	char tmp[maxLength] = { 0 };
	char* head = tmp;
	char* classToken = (char*)malloc(10);
	c = fgetc(in);
	int flag = 0;
	while (isspace(c)) {
		if (c == '\n') {
			flag++;
			line++;
		}
		c = fgetc(in);
	}
	is_newline[2] = is_newline[1];
	is_newline[1] = is_newline[0];
	is_newline[0] = flag;
	//printf("******%c********\n", c);
	//system("pause");
	strcpy(classToken, "");
	if (c == EOF) {
		return "END"; //?
	}
	while (!isLegal(c)) {
		error(ILLEGAL); //strange character
		c = fgetc(in);
		while (isspace(c)) {
			if (c == '\n') {
				flag++;
				line++;
			}
			c = fgetc(in);
		}
	}
	if (isLetter(c)) {
		strcpy(tmp, inden(in));
		if (strcmp(tmp, "const") == 0) {
			strcpy(classToken, "CONSTTK");
		}
		else if (strcmp(tmp, "var") == 0) {
			strcpy(classToken, "VARTK");
		}
		else if (strcmp(tmp, "para") == 0) {
			strcpy(classToken, "PARATK");
		}
		else if (strcmp(tmp, "ret") == 0) {
			strcpy(classToken, "RETTK");
		}
		else if (strcmp(tmp, "push") == 0) {
			strcpy(classToken, "PUSHTK");
		}
		else if (strcmp(tmp, "call") == 0) {
			strcpy(classToken, "CALLTK");
		}
		else if (is_jump(tmp)) {
			strcpy(classToken, tmp);
		}
		else if (strcmp(tmp, "int") == 0) {
			strcpy(classToken, "INTTK");
		}
		else if (strcmp(tmp, "void") == 0) {
			strcpy(classToken, "VOIDTK");
		}
		else if (strcmp(tmp, "char") == 0) {
			strcpy(classToken, "CHARTK");
		}
		else if (strcmp(tmp, "main") == 0) {
			strcpy(classToken, "MAINTK");
		}
		else if (strcmp(tmp, "if") == 0) {
			strcpy(classToken, "IFTK");
		}
		else if (strcmp(tmp, "else") == 0) {
			strcpy(classToken, "ELSETK");
		}
		else if (strcmp(tmp, "do") == 0) {
			strcpy(classToken, "DOTK");
		}
		else if (strcmp(tmp, "while") == 0) {
			strcpy(classToken, "WHILETK");
		}
		else if (strcmp(tmp, "while") == 0) {
			strcpy(classToken, "WHILETK");
		}
		else if (strcmp(tmp, "for") == 0) {
			strcpy(classToken, "FORTK");
		}
		else if (strcmp(tmp, "scanf") == 0) {
			strcpy(classToken, "SCANFTK");
		}
		else if (strcmp(tmp, "printf") == 0) {
			strcpy(classToken, "PRINTFTK");
		}
		else if (strcmp(tmp, "return") == 0) {
			strcpy(classToken, "RETURNTK");
		}
		else {
			strcpy(classToken, "IDENFR");
		}
	}
	else if (c == '+') {
		strcpy(classToken, "PLUS");
	}
	else if (c == '-') {
		strcpy(classToken, "MINU");
	}
	else if (c == '*') {
		strcpy(classToken, "MULT");
	}
	else if (c == '/') {
		strcpy(classToken, "DIV");
	}
	else if (c == ';') {
		strcpy(classToken, "SEMICN");
	}
	else if (c == ',') {
		strcpy(classToken, "COMMA");
	}
	else if (c == '(') {
		strcpy(classToken, "LPARENT");
	}
	else if (c == ')') {
		strcpy(classToken, "RPARENT");
	}
	else if (c == '[') {
		strcpy(classToken, "LBRACK");
	}
	else if (c == ']') {
		strcpy(classToken, "RBRACK");
	}
	else if (c == '{') {
		strcpy(classToken, "LBRACE");
	}
	else if (c == '}') {
		strcpy(classToken, "RBRACE");
	}
	else if (isRelaChar(c)) {
		strcpy(tmp, ope(in));
		if (strcmp(tmp, "<") == 0) {
			strcpy(classToken, "LSS");
		}
		else if (strcmp(tmp, "<=") == 0) {
			strcpy(classToken, "LEQ");
		}
		else if (strcmp(tmp, ">") == 0) {
			strcpy(classToken, "GRE");
		}
		else if (strcmp(tmp, ">=") == 0) {
			strcpy(classToken, "GEQ");
		}
		else if (strcmp(tmp, "==") == 0) {
			strcpy(classToken, "EQL");
		}
		else if (strcmp(tmp, "!=") == 0) {
			strcpy(classToken, "NEQ");
		}
		else if (strcmp(tmp, "=") == 0) {
			strcpy(classToken, "ASSIGN");
		}
	}
	else if (c == '\'') {
		c = fgetc(in);
		if (!isNum(c) && !isOpe(c) && !isLetter(c)) {
			error(ILLEGAL); //strange character
		}
		//有无形如 'aaa' ？
		if (fgetc(in) != '\'') {
			error(ILLEGAL);
			fseek(in, -1, SEEK_CUR);
		} //delete right '
		strcpy(classToken, "CHARCON");
	}
	else if (c == '"') {
		strcpy(tmp, str(in));
		strcpy(classToken, "STRCON");
	}
	else if (isNum(c)) {
		strcpy(tmp, integer(in));
		strcpy(classToken, "INTCON");
	}

	pre_name = name;

	if (strlen(tmp) == 0) {
		//if (ftell(in) != last_tell)
		name = c;
		fprintf(out, "%s %s\n", classToken, name.c_str());
		backlen[2] = backlen[1];
		backlen[1] = backlen[0];
		if (windows) {
			backlen[0] = strlen(classToken) + 1 + 3; 
		}
		else {
			backlen[0] = strlen(classToken) + 1 + 2;
		}
		//windows: /r/n, linux: /n
	}
	else {
		name = tmp;
		fprintf(out, "%s %s\n", classToken, tmp);
		backlen[2] = backlen[1];
		backlen[1] = backlen[0];
		if (windows) {
			backlen[0] = strlen(classToken) + strlen(tmp) + 3; 
		}
		else {
			backlen[0] = strlen(classToken) + strlen(tmp) + 2;
		}
	}
	int end = ftell(in);
	step[2] = step[1];
	step[1] = step[0];
	step[0] = end - start;
	last_tell = end;
	return classToken;
}




