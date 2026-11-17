#include <stdio.h>
#include "lexer.h"
#include <string.h>

int main(int argc, char *argv[]) {
    FILE* file = initializeLexer(argv[1]);

    if (file == NULL) {
        printf("Error: Failed to initialize lexer.\n");
        return 1;
    }

    if (checkFileError(file)) {
        return 1;

    }

    Token token;
    printf("|%-25s|%-25s|\n", "TOKEN","TYPE");
    printf("|%-25s|%-25s|\n", "-------------------------","-------------------------");
    while (1) {
        token = getNextToken();
        //printf("Token: %s, Type: %d\n", token.lexeme, token.type);
        if (token.type == UNKNOWN && strcmp(token.lexeme, "EOF") == 0) {
            break;
        }
        
         switch (token.type)
            {
                case 0:
                    printf("|%-25s|%-25s|\n", token.lexeme, "KEYWORD");
                    break;
                case 1:
                    printf("|%-25s|%-25s|\n", token.lexeme, "OPERATOR");
                    break;
                case 2:
                    printf("|%-25s|%-25s|\n", token.lexeme, "SPECIAL_CHARACTER");
                    break;
                case 3:
                    printf("|%-25s|%-25s|\n", token.lexeme, "CONSTANT");
                    break;
                case 4:
                    printf("|%-25s|%-25s|\n", token.lexeme, "IDENTIFIER");
                    break;
                case 5:
                    printf("|%-25s|%-25s|\n", token.lexeme, "LITERAL");
                    break;
                default:
                    printf("|%-25s|%-25s|\n", token.lexeme, "UNKNOWN");
            }
 
        
    }
    printf("|%-25s|%-25s|\n", "-------------------------","-------------------------");

    //getNextToken();

    closeFile(file);
    return 0;
}
