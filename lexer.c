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
    fclose(file);
}

Token getNextToken(){
    Token token;
    FILE* file = fopen("demo.c", "r");
}


int isKeyword(const char* str){
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(keywords[i], str) == 0) {
            return 1;
        }
    }
    return 0;
}
int isOperator(const char* str){
    return strchr(operators, str[0]) != NULL;
}
int isSpecialCharacter(char ch){
    return strchr(specialCharacters, ch) != NULL;
}
int isConstant(const char* str){
    return isdigit(str[0]) != 0;
}
int isIdentifier(const char* str){
    if (!isalpha(str[0]) && str[0] != '_') {
        return 0;
    }
    for (int i = 1; i < strlen(str); i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            return 0;
        }
    }
    return 1;
}