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

void closeFile(FILE* file){
    if(file)
        fclose(file);
}


// Section 2: Token Processing Functions
char *cleanleadingWhitespace(char *str){
    while(isspace((unsigned char)*str))
        str++;
    return str;
}

char *stripsuccedingWhitespace(char *str){
    char *end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)){
        end--;
    }
    *(end + 1) = '\0';
    return str;
}

int preprocessToken(char *token){

    int i = 0;
    int dotflag = 0;
    if (isdigit((unsigned char)token[0]))
    {
        for (i = 1; token[i] != '\0'; i++)
        {
            if (isalnum((unsigned char)token[i]) ||
                token[i] == '_' || token[i] == '.')
            {
                continue;
            }

            /* Allow + and - only after exponent */
            if ((token[i] == '+' || token[i] == '-') &&
                (token[i-1] == 'e' || token[i-1] == 'E'))
            {
                continue;
            }

            return i - 1;
        }

        return i - 1;
    }




    if(isOperator(token)){
        if(isOperator(token+1)){
            return 1;
        }
        return 0;
    }
    if(isSpecialCharacter(token)){
        return 0;
    }
    if(token[0] == stringliteral){
        for(i=1;token[i]!=stringliteral && token[i]!='\0';i++);
        return i;
    }



    if(token[0] == '\''){
        if(token[1] == '\\' && token[3] == '\''){
                return 3;
        }
        else if(token[2] == '\''){
                return 2;
        }
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

Token getNextToken(){
    static long cursor = 0;
    static char buffer[MAX_TOKEN_SIZE];
    static int pos = 0;

    while(buffer[pos] == '\0'){
        if(fseek(file, cursor, SEEK_SET) != 0){
            Token eofToken;
            strcpy(eofToken.lexeme, "EOF");
            eofToken.type = UNKNOWN;
            return eofToken;
        }

        if(fgets(buffer, sizeof(buffer), file) == NULL){
            Token eofToken;
            strcpy(eofToken.lexeme, "EOF");
            eofToken.type = UNKNOWN;
            return eofToken;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(buffer, cleanleadingWhitespace(buffer));
        //printf("Read line: %s\n", buffer);
        if (buffer[0] == '\0' || buffer[0] == '/' || buffer[0] == '#'){
            buffer[0] = '\0';
        }

        pos = 0;
        cursor = ftell(file);
    }

    while(isspace((unsigned char)buffer[pos]))
        pos++;
    if(buffer[pos] == '\0'){
        buffer[0] = '\0';
        return getNextToken();
    }

    int index = preprocessToken(&buffer[pos]);
    char curr_token[MAX_TOKEN_SIZE];
    strncpy(curr_token, &buffer[pos], index+1);
    curr_token[index+1] = '\0';
    strcpy(curr_token, stripsuccedingWhitespace(curr_token));

    Token token;
    strcpy(token.lexeme, curr_token);
    token.type = UNKNOWN;
    categorizeToken(&token);

    pos += index+1;
    return token;
}

// Section 3: Token Categorization Functions
void categorizeToken(Token* token){
    if(isKeyword(token->lexeme)){
        token->type = KEYWORD;
    } else if(isOperator(token->lexeme)){
        token->type = OPERATOR;
    } else if(isSpecialCharacter(token->lexeme)){
        token->type = SPECIAL_CHARACTER;
    } else if(isConstant(token->lexeme)){
        token->type = CONSTANT;
    } else if(isIdentifier(token->lexeme)){
        token->type = IDENTIFIER;
    } else if(isStringLiteral(token->lexeme)){
        token->type = LITERAL;
    } else if((token->lexeme[0] == '\'' && token->lexeme[2] == '\'') || (token->lexeme[0] == '\'' && token->lexeme[1] == '\\' && token->lexeme[3] == '\'')){
        token->type = LITERAL;
    }   
    else{
        token->type = UNKNOWN;
    }
    
}


// Section 4: Token Type Checking Functions
int isKeyword(const char* str){
    for(int i=0;i<MAX_KEYWORDS;i++){
        if(strcmp(str, keywords[i]) == 0){
            return 1;
        }
    }
    return 0;
}

int isOperator(const char* str){
    return strchr(operators, str[0]) != NULL;
}

int isStringLiteral(const char* str){
    if(str[0] == '"' && str[strlen(str)-1] == '"'){
        return 1;
    }
    return 0;
}

int isSpecialCharacter(const char* str){
    return strchr(specialCharacters, str[0]) != NULL;
}

int isConstant(const char *str){
    int i = 0;
    int beforeDot = 0, afterDot = 0;
    int hasDot = 0, hasExponent = 0;

    if (str == NULL || str[0] == '\0')
        return 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
        i = 2;
        int digits = 0;

        while(isxdigit((unsigned char)str[i])){
            digits++;
            i++;
        }

        if(!digits)
            return 0;

        goto integer_suffix;
    }

    if(str[0] == '0' && (str[1] == 'b' || str[1] == 'B')){
        i = 2;
        int digits = 0;

        while(str[i] == '0' || str[i] == '1'){
            digits++;
            i++;
        }

        if(!digits)
            return 0;

        goto integer_suffix;
    }

    /* 3. Decimal / Octal / Float */

    while(isdigit((unsigned char)str[i])){
        beforeDot++;
        i++;
    }

    if(str[i] == '.'){
        hasDot = 1;
        i++;

        while(isdigit((unsigned char)str[i])){
            afterDot++;
            i++;
        }
    }

    if(beforeDot == 0 && afterDot == 0)
        return 0;

    /* 4. Exponential notation */
    if(str[i] == 'e' || str[i] == 'E'){
        hasExponent = 1;
        i++;

        if(str[i] == '+' || str[i] == '-')
            i++;

        int expDigits = 0;

        while(isdigit((unsigned char)str[i])){
            expDigits++;
            i++;
        }

        if(!expDigits)
            return 0;
    }

    /* 5. Floating-point suffix */
    if(hasDot || hasExponent){
        if (str[i] == 'f' || str[i] == 'F' || str[i] == 'l' || str[i] == 'L'){
            i++;
        }

        return str[i] == '\0';
    }

    /* 6. Octal validation */
    if(str[0] == '0' && str[1] != '\0'){
        for (int j = 1; j < i; j++){
            if (str[j] < '0' || str[j] > '7')
                return 0;
        }
    }

integer_suffix:{
        int hasU = 0;
        int lCount = 0;
        char lCase = '\0';

        while (str[i] != '\0'){
            if(str[i] == 'u' || str[i] == 'U'){
                if (hasU)
                    return 0;

                hasU = 1;
            }
            else if(str[i] == 'l' || str[i] == 'L'){
                if(lCount == 0)
                    lCase = str[i];
                else if(lCount == 1 && str[i] != lCase)
                    return 0;

                lCount++;

                if(lCount > 2)
                    return 0;
            }
            else{
                return 0;
            }

            i++;
        }
    }

    return 1;
}

int isIdentifier(const char* str){
    if(!isalpha(str[0]) && str[0] != '_')
        return 0;
    for(int i=1;str[i];i++){
        if(!isalnum(str[i]) && str[i] != '_')
            return 0;
    }
    return 1;
}


// Section 5: Errors in file Checking Function
int checkFileError(FILE *fp){
    int c, next;
    int line = 1;

    int top = -1;
    char stack[4096];

    fseek(fp, 0, SEEK_SET);

    while ((c = fgetc(fp)) != EOF){
        /* 1. Ignore whitespace */
        if(isspace((unsigned char)c)){
            if (c == '\n')
                line++;

            continue;
        }

        /* 2. Handle comments */
        if (c == '/'){
            next = fgetc(fp);

            /* Single-line comment */
            if(next == '/'){
                while ((c = fgetc(fp)) != EOF && c != '\n');

                if(c == '\n')
                    line++;

                continue;
            }

            /* Multiline comment */
            if(next == '*'){
                int closed = 0;
                int prevStar = 0;

                while((c = fgetc(fp)) != EOF){
                    if (c == '\n')
                        line++;

                    if(prevStar && c == '/'){
                        closed = 1;
                        break;
                    }

                    prevStar =(c == '*');
                }

                if(!closed){
                    printf("Error: Unterminated multiline comment.\n");
                    return 1;
                }

                continue;
            }

            if(next != EOF)
                ungetc(next, fp);
        }

        /* 3. String literal */
        if(c == '"'){
            int escaped = 0;
            int closed = 0;

            while((c = fgetc(fp)) != EOF && c != '\n'){
                if(c == '"' && !escaped){
                    closed = 1;
                    break;
                }

                if(c == '\\' && !escaped)
                    escaped = 1;
                else
                    escaped = 0;
            }

            if(!closed){
                printf("Error at line %d: String literal not closed.\n",line);
                return 1;
            }

            continue;
        }

        /* 4. Character literal */
        if(c == '\''){
            int valid = 1;

            c = fgetc(fp);

            if(c == '\\'){
                c = fgetc(fp);

                if(c == EOF || c == '\n')
                    valid = 0;
            }
            else if(c == EOF || c == '\n' || c == '\''){
                valid = 0;
            }

            if(valid){
                c = fgetc(fp);

                if (c != '\'')
                    valid = 0;
            }

            if(!valid){
                printf("Error at line %d: Invalid character literal.\n",line);
                return 1;
            }

            continue;
        }

        /* 5. Delimiter validation */
        if(c == '(' || c == '[' || c == '{'){
            if(top >= (int)sizeof(stack) - 1){
                printf("Error: Delimiter nesting limit exceeded.\n");
                return 1;
            }

            stack[++top] = c;
        }
        else if(c == ')' || c == ']' || c == '}'){
            if(top == -1){
                printf("Error at line %d: Unmatched closing delimiter.\n",
                       line);
                return 1;
            }

            if((c == ')' && stack[top] != '(') || (c == ']' && stack[top] != '[') || (c == '}' && stack[top] != '{')){
                printf("Error at line %d: Mismatched delimiter.\n",line);
                return 1;
            }

            top--;
        }

        /* 6. Number and invalid identifier validation */
        if(isdigit((unsigned char)c)){
            char buffer[100];
            int i = 0;
            int overflow = 0;
            int prev = c;

            buffer[i++] = c;

            while((c = fgetc(fp)) != EOF){
                if(isalnum((unsigned char)c) || c == '_' || c == '.'){
                    if (i < sizeof(buffer) - 1)
                        buffer[i++] = c;
                    else
                        overflow = 1;
                }
                else if((c == '+' || c == '-') && (prev == 'e' || prev == 'E')){
                    if (i < sizeof(buffer) - 1)
                        buffer[i++] = c;
                    else
                        overflow = 1;
                }
                else{
                    ungetc(c, fp);
                    break;
                }

                prev = c;
            }

            buffer[i] = '\0';

            if(overflow || !isConstant(buffer)){
                int invalidIdentifier = 0;

                for(int j = 1; buffer[j] != '\0'; j++){
                    if (isalpha((unsigned char)buffer[j]) || buffer[j] == '_'){
                        invalidIdentifier = 1;
                        break;
                    }
                }

                if(invalidIdentifier)
                    printf("Error at line %d: Invalid identifier: %s\n",line, buffer);
                else
                    printf("Error at line %d: Invalid number: %s\n",line, buffer);

                return 1;
            }
        }
    }

    /* 7. Check remaining opening delimiters */
    if(top != -1){
        printf("Error: Unmatched opening delimiter '%c'.\n",stack[top]);
        return 1;
    }

    fseek(fp, 0, SEEK_SET);

    return 0;
}


