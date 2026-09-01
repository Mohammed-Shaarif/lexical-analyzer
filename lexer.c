#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>|&";
static const char* specialCharacters = ",;{}()[]";



void initializeLexer(const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    printf("Lexer initialized with file: %s\n", filename);
}