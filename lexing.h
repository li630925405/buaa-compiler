#pragma once
#include "lib.h"
#include "common.h"
#include "error.h"

void error(char c);
char* integer(FILE* in);
char* str(FILE* in);
const char* ope(FILE* in); // get operator string
char* inden(FILE* p);
void backtrack(int times);