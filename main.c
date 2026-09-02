#include <stdio.h>
#include "lexer.h"

int main(int argc, char *argv[]) {
    FILE* file = initializeLexer(argv[1]);



    // Token token;
    // while ((token = getNextToken()).type != UNKNOWN && token.lexeme[0] != '\0') {
    //     printf("Token: %s, Type: %d\n", token.lexeme, token.type);
    // }

    getNextToken();
    
    closeFile(file);
    return 0;
}
