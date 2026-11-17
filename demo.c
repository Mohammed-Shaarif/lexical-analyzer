#include <stdio.h>

int main() {
    int num = 10;        // Invalid identifier

    float value = 12.3456; // Invalid floating constant

    char ch = 'A';         // Invalid character constant

    char c = 'x';            // Unterminated character literal

    string msg = "Hello";    // Unterminated string literal

    int a = 10 @ 20;        // Invalid operator/token

    int b = 0xab;           // Invalid hexadecimal constant

    int x = -100;

    return 0;
}
