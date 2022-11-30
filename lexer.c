/**<lexer.c>::**/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <keywords.h>
#include <lexer.h>

char            lexeme[MAXIDLEN + 1];

extern int      linecount;

int             linecount = 1;

/** { .* }**/

void
skipspaces(FILE * tape)
{
    int             head;

_skipspaces:    
    while (isspace(head = getc(tape))) {
        if (head == '\n')
            linecount++;
    }
    
    if(head == '{') {
    	while ((head = getc(tape)) != '}' && head != EOF);
    	goto _skipspaces;
    }
    ungetc(head, tape);
}

int
isID(FILE * tape)
{
    /***********
	 * ID = [A-Za-z][A-Za-z0-9]*
	 */
    int             i = 0;
    if (isalpha(lexeme[i] = getc(tape))) {
        i++;
        while (isalnum(lexeme[i] = getc(tape))) {
            if (i < MAXIDLEN)
                i++;
        }
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        int             token = iskeyword(lexeme);
        if (token)
            return token;
        return ID;
    }
    ungetc(lexeme[i], tape);
    lexeme[i] = 0;

    return 0;
}

int
isINT(FILE * tape)
{
    int             i = 0;
    if (isdigit(lexeme[i] = getc(tape))) {
        if (lexeme[i] == '0') {
            /** OCT = 0[0-7]+ */
            i++;
            if ((lexeme[i] = getc(tape)) >= '0' && lexeme[i] < '8') {
                i++;
                while ((lexeme[i] = getc(tape)) >= '0' && lexeme[i] < '8')
                    i++;
                ungetc(lexeme[i], tape);
                lexeme[i] = 0;
                return OCT;
            }
            ungetc(lexeme[i], tape);

            /** HEX = 0[xX][0-9a-fA-F]+ */
            lexeme[i] = getc(tape);
            if (toupper(lexeme[i]) == 'X') {
                i++;
                if (isxdigit(lexeme[i] = getc(tape))) {
                    i++;
                    while (isxdigit(lexeme[i] = getc(tape)))
                        i++;
                    ungetc(lexeme[i], tape);
                    lexeme[i] = 0;
                    return HEX;
                }
                ungetc(lexeme[i], tape);
                i--;
            }
            ungetc(lexeme[i], tape);

            return DEC;
        }
        i++;
        while (isdigit(lexeme[i] = getc(tape)))
            i++;
        ungetc(lexeme[i], tape);
        lexeme[i] = 0;
        return DEC;
    }
    ungetc(lexeme[i], tape);
    lexeme[i] = 0;

    return 0;
}

/** FLT = DEC '.' [0-9]* EE? | '.' [0-9]+ EE? | DEC EE = (DEC'.'[0-9]* | '.'[0-9]+) EE? | DEC EE
 *  NUM = (DEC | DEC'.'[0-9]* | '.'[0-9]+) EE?
 *  EE = [eE] ['+'|'-']? [0-9]+
 **/
int
isEE(FILE * tape)
{
    int             i = strlen(lexeme);
    int             ee = getc(tape);

    /**  EE = [eE] ['+'|'-']? [0-9]+ **/
    if (toupper(ee) == 'E') {
        lexeme[i] = ee;
        int             plusminus = getc(tape);
        if (plusminus != '+' && plusminus != '-') {
            ungetc(plusminus, tape);
            plusminus = 0;
        } else {
            i++;
            lexeme[i] = plusminus;
        }
        i++;
        if (isdigit(lexeme[i] = getc(tape))) {
            i++;
            while (isdigit(lexeme[i] = getc(tape)))
                i++;
            ungetc(lexeme[i], tape);
            lexeme[i] = 0;
            return 1;
        }
        ungetc(lexeme[i], tape);
        i--;
        if (plusminus) {
            i--;
            ungetc(plusminus, tape);
        }
    }
    ungetc(ee, tape);
    lexeme[i] = 0;
    return 0;
}

int
isNUM(FILE * tape)
{
    int             token = isINT(tape),
        i;
    switch (token) {
    case OCT:
    case HEX:
        return token;
    }
    if (token == DEC) {
        i = strlen(lexeme);
        if ((lexeme[i] = getc(tape)) == '.') {
            token = FLT;
            i++;
            while (isdigit(lexeme[i] = getc(tape)))
                i++;
            ungetc(lexeme[i], tape);
        } else {
            ungetc(lexeme[i], tape);
        }
        lexeme[i] = 0;
    } else {
        i = 0;
        if ((lexeme[i] = getc(tape)) == '.') {
            i++;
            if (isdigit(lexeme[i] = getc(tape))) {
                token = FLT;
                i++;
                while (isdigit(lexeme[i] = getc(tape)))
                    i++;
                ungetc(lexeme[i], tape);
            } else {
                ungetc(lexeme[i], tape);
                i--;
                ungetc('.', tape);
                i--;
            }
        } else {
            ungetc(lexeme[i], tape);
        }
        lexeme[i] = 0;
    }
    if (token && isEE(tape)) {
        token = FLT;
    }
    return token;
}

int
isRELOP(FILE * tape)
{
    lexeme[0] = getc(tape);
    lexeme[2] = 0;
    switch (lexeme[0]) {
    case '<':
        lexeme[1] = getc(tape);
        if (lexeme[1] == '>') {
            return NEQ;
        }
        if (lexeme[1] == '=') {
            return LEQ;
        }
        ungetc(lexeme[1], tape);
        lexeme[1] = 0;
        return '<';
    case '>':
        lexeme[1] = getc(tape);
        if (lexeme[1] == '=') {
            return GEQ;
        }
        ungetc(lexeme[1], tape);
        lexeme[1] = 0;
        return '>';
    }
    ungetc(lexeme[0], tape);
    lexeme[0] = 0;
    return 0;
}

int isASGN(FILE *tape)
{
	lexeme[0] = getc(tape); lexeme[2] = 0;
	if (lexeme[0] == ':') {
		lexeme[1] = getc(tape);
		if (lexeme[1] == '=') {
			return ASGN;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

int
gettoken(FILE * tape)
{
    int             token;

    skipspaces(tape);

    if ((token = isID(tape)))
        return token;

    if ((token = isNUM(tape)))
        return token;

    if ((token = isRELOP(tape)))
        return token;

    if ((token = isASGN(tape)))
        return token;

    return token = getc(tape);
}
