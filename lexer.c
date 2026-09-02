#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include <stdlib.h>
#include <unistd.h>

const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

const char* operators = "+-*/%=!<>|&";
const char* specialCharacters = ",;{}()[]";

const char stringliteral = '"';
const char characterliteral = '\'';

static FILE* file = NULL;


// Section 1: File Handling Functions
FILE* initializeLexer(const char* filename){
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    file = fp;
    printf("Lexer initialized with file: %s\n", filename);
    return fp;
}

void closeFile(FILE* file) {
    if (file) {
        fclose(file);
    }
}


// Section 2: Token Processing Functions
char *cleanleadingWhitespace(char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

int preprocessToken(char *token){

    int i = 0;
    int dotflag = 0;
    if (token[0] >= '0' && token[0] <= '9') {
        for (i = 1;token[i] != '\0'; i++) {
            if (token[i] == '.') {
                if (dotflag) {
                    return i - 1;
                }
                dotflag = 1;
            } else if (!(token[i] >= '0' && token[i] <= '9')) {
                return i - 1;
            }
        }
        return i - 1;
    }




    if(isOperator(token)){
        return 0;
    }
    if(isSpecialCharacter(token)){
        return 0;
    }
    if(token[0] == stringliteral){
        for(i=1;token[i]!=stringliteral && token[i]!='\0';i++);
        return i;
    }



    if(token[0] == '\'' && token[2] == '\''){
        return 2;
    }


    for(i=0;token[i]!=' ' && token[i]!='\0';i++);
    //return i;
    int keyflag = 0;
    for(int j=0;j<i;j++){
        if(isalpha(tolower(token[j]))){
            keyflag = 1;
        }

        if(keyflag && !isalnum(tolower(token[j]))){
            return j-1;
        }
        
    }
}

Token getNextToken() {
    static long cursor = 0;
    char buffer[MAX_TOKEN_SIZE];
    printf("Current cursor position: %ld\n", cursor);
    fseek(file, cursor, SEEK_SET);

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';

        if (buffer[0] == '\0' || buffer[0] == '/' || buffer[0] == '#') {
            //printf("Skipping line: %s\n", buffer);
            cursor = ftell(file);
        } else {
            //printf("Read token: %s\n", buffer);
            char *token = buffer;
            while (isspace((unsigned char)*token)) token++;
            if (*token == '\0') {
                cursor = ftell(file);
                continue;
            }
            strcpy(buffer, cleanleadingWhitespace(buffer));
            int index = preprocessToken(buffer);

            char curr_token[MAX_TOKEN_SIZE];
            strncpy(curr_token, buffer, index+1);
            curr_token[index+1] = '\0';

            printf("Processed token: %s\n", curr_token);
            cursor = ftell(file)-( strlen(buffer) - index +1);
            fseek(file, cursor, SEEK_SET);
            sleep(1);
        }
    }
}

// Section 3: Token Categorization Functions
void categorizeToken(Token* token){
    
}


// Section 4: Token Type Checking Functions
// int isKeyword(const char* str);
int isOperator(const char* str){
    return strchr(operators, str[0]) != NULL;
}
int isStringLiteral(const char* str){
    if(str[0] != '"')
        return 0;
    for(int i=1;str[i]!='"';i++){
        if(str[i] == stringliteral)
            return i+1;
    }
    return 0;
}
int isSpecialCharacter(const char* str){
    return strchr(specialCharacters, str[0]) != NULL;
}
int isConstant(const char* str){
    for(int i=0;(isdigit(str[i]) || str[i]=='.');i++){
        return i+1;
    }
    return 0;
}
// int isIdentifier(const char* str);

