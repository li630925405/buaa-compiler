#include "lexing.h"
#include "grammar.h"

int main() {
	if (debug == 1) {
		in = fopen("testfile.txt", "r");
		//out = fopen("E:\\c\\lexing\\lexing\\output.txt", "w");
		out = stdout;
		err = stdout;
		mips = fopen("mips.txt", "w");
		fmiddle.open("middle.txt");
	}
	else if (debug == 2) {
		in = fopen("testfile.txt", "r");
		//out = fopen("E:\\c\\lexing\\lexing\\output.txt", "w");
		out = fopen("output.txt", "w");
		err = fopen("error.txt", "w");
		mips = stdout;
		fmiddle.open("middle.txt");
	}
	else {
		fmiddle.open("middle.txt");
		in = fopen("testfile.txt", "r");
		out = fopen("output.txt", "w");
		err = fopen("error.txt", "w");
		mips = fopen("mips.txt", "w");
		//fmips.open("middle.txt");
	}
	program();
	generate();
	return 0;
}