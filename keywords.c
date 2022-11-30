#include <string.h>
#include <keywords.h>

char            *keyword[] = {
    "begin",
    "program",
    "procedure",
    "function",
    "if",
    "then",
    "else",
    "while",
    "do",
    "repeat",
    "until",
    "var",
    "integer",
    "real",
    "double",
    "boolean",
    "not",
    "or",
    "div",
    "mod",
    "and",
    "end"
};

int
iskeyword(char const *name)
{
    int             i;
    for (i = BEGIN; i < END + 1; i++) {
        if (strcmp(keyword[i - BEGIN], name) == 0)
            return i;
    }
    return 0;
}
