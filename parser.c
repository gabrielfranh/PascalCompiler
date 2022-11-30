#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <keywords.h>
#include <parser.h>
#include <symtab.h>

int lookahead;

int lexlevel = 0;

/***********************
 *
 * program -> PROGRAM ID (ID, ID) ; declrative imperative .
 */
void
program(void)
{
/**/ lexlevel++; /**/
    match(PROGRAM);
    match(ID);
    match('(');
    match(ID);
    match(',');
    match(ID);
    match(')');
    match(';');
    declarative();
    imperative();
    match('.');
    /**/ lexlevel--; /**/
    list_symtab();
}

/*
 * declarative -> vardecl sbpdecl
 */
void
declarative(void)
{
    vardecl();
    sbpdecl();
}

/*
 * vardecl -> VAR varlist : vartype ; { varlist : vartype ; }
 */
 int symtab_avail, symtab_type;
void
vardecl(void)
{
	if(lookahead == VAR)
	{
		int i;
		match(VAR);
		/**/ symtab_avail = symtab_next; /**/
		varlist();
		match(':');
		vartype();
		/**/
		for(i = symtab_avail; i < symtab_next; i++)
		{	
			symtab[i].type = symtab_type;
		}
		/**/
		match(';');
		while (lookahead == ID) {
		/**/ symtab_avail = symtab_next; /**/
		    varlist();
		    match(':');
		    vartype();
		    /**/
		    for(i = symtab_avail; i < symtab_next; i++)
		    {	
				symtab[i].type = symtab_type;
		    }
		    /**/
		    match(';');
		}
	}
}

/*
 * varlist -> ID { , ID } 
 */
void
varlist(void)
{
    /**/ symtab_append(lexeme); /**/
    match(ID);
    while (lookahead == ',') {
        match(',');
        /**/ symtab_append(lexeme); /**/
        match(ID);
    }
}

/*
 * vartype -> INTEGER | REAL | DOUBLE | BOOLEAN
 */
void
vartype(void)
{
    switch (lookahead) {
    case INTEGER:
    	symtab_type = 1;
    	match(lookahead);
        break;
    case REAL:
    	symtab_type = 2;
    	match(lookahead);
        break;
    case DOUBLE:
    	symtab_type = 3;
        match(lookahead);
        break;
    default:
    	symtab_type = 4;
        match(BOOLEAN);
    }
}

/*
 * sbpdecl -> { proceddecl | funcdecl }
 */
void
sbpdecl(void)
{
    while ( lookahead == PROCEDURE || lookahead == FUNCTION) {
        if (lookahead == PROCEDURE) {
            proceddecl();
        } else {
            funcdecl();
        }
    }
}

/*
 * proceddecl -> PROCEDURE ID [ ( parmlist ) ] ; declarative imperative ;
 */
void
proceddecl(void)
{
	/**/ lexlevel++; /**/
    match(PROCEDURE);
    match(ID);
    if (lookahead == '(') {
        match('(');
        parmlist();
        match(')');
    }
    match(';');
    declarative();
    imperative();
    match(';');
    /**/ lexlevel--; /**/
}

/*
 * funcdecl -> FUNCTION ID [ ( parmlist ) ] : vartype ; declarative imperative ; 
 */
void
funcdecl(void)
{
	/**/ lexlevel++; /**/
    match(FUNCTION);
    match(ID);
    if (lookahead == '(') {
        match('(');
        parmlist();
        match(')');
    }
    match(':');
    vartype();
    match(';');
    declarative();
    imperative();
    match(';');
    /**/ lexlevel--; /**/
}

/*
 * parmlist -> [VAR] varlist : vartype { ; [VAR] varlist : vartype }
 */
void
parmlist(void)
{
    if (lookahead == VAR) {
        match(VAR);
    }
    varlist();
    match(':');
    vartype();
    while (lookahead == ';') {
        match(';');
        if (lookahead == VAR) {
            match(VAR);
        }
        varlist();
        match(':');
        vartype();
    }
}

/*
 * imperative -> BEGIN stmtlist END 
 */
void
imperative(void)
{
    match(BEGIN);
    stmtlist();
    match(END);
}

/*
 * stmtlist -> stmt { ; stmt } 
 */
void
stmtlist(void)
{
    stmt();
    while (lookahead == ';') {
        match(';');
        stmt();
    }
}

void            smpexpr(void);
/*
 * stmt -> <empty> | imperative | ifstmt | whilestmt | repstmt | smpexpr 
 */
void
stmt(void)
{
    switch (lookahead) {
    case BEGIN:
        imperative();
        break;
    case IF:
        ifstmt();
        break;
    case WHILE:
        whilestmt();
        break;
    case REPEAT:
        repstmt();
        break;
    case ID:
        smpexpr();
        break;
    default:
        ;
    }
}

/*
 * ifstmt -> IF expr THEN stmt [ ELSE stmt ] 
 */
int loopcount = 1;
void
ifstmt(void)
{
    /**/int lbl1, lbl2;/**/
    match(IF);
    expr();
    match(THEN);
    /*1*/printf("\tjz .L%d\n", lbl1 = lbl2 = loopcount++);/*1*/
    stmt();
    if (lookahead == ELSE) {
        match(ELSE);
	/*2*/printf("\tjmp .L%d\n", lbl2 = loopcount++); printf(".L%d:\n", lbl1);/*2*/
        stmt();
    }
    /*3*/printf(".L%d:\n", lbl2);/*3*/
}

/*
 * whilestmt -> WHILE expr DO stmt 
 */
void
whilestmt(void)
{
    /**/ int startWhile, endWhile; /**/
    match(WHILE);
    /**/printf(".L%d:\n", startWhile = loopcount++); /**/
    expr();
    /**/ printf("\tjz .L%d\n", endWhile = loopcount++); /**/
    match(DO);
    stmt();
    /**/ printf("\tjmp .L%d\n", startWhile = loopcount++); /**/
    /**/ printf(".L%d:\n", endWhile); /**/
}

/*
 * repstmt -> REPEAT stmtlist UNTIL expr 
 */
void
repstmt(void)
{
    /**/ int startRepeat; /**/
    match(REPEAT);
    /**/printf(".L%d:\n", startRepeat = loopcount++); /**/
    stmtlist();
    match(UNTIL);
    expr();
    /**/ printf("\tjz .L%d\n", startRepeat); /**/
}

/*
 * expr -> smprexpr [ relop smpexpr ] 
 */
void
expr(void)
{
    smpexpr();
    if (relop()) {
        match(lookahead);
        smpexpr();
    }
}

/*
 * smpexpr -> [ unary ] term { oplus term } 
 */
void
smpexpr(void)
{
    if (unary()) {
        match(lookahead);
    }
    term();
    while (oplus()) {
        match(lookahead);
        term();
    }
}

/*
 * term -> fact { otimes fact } 
 */
void
term(void)
{
    fact();
    while (otimes()) {
        match(lookahead);
        fact();
    }
}

/*
 * fact -> ( expr ) | ID [ := expr ] | DEC | FLT | OCT | HEX 
 */
void
fact(void)
{
    switch (lookahead) {
    case ID:
        match(ID);
	if (lookahead == '(') {
		match('('); expr(); while(lookahead == ',') {match(','); expr();} match(')');
	} else if (lookahead == ASGN) {
            match(ASGN);
            expr();
        }
        break;
    case DEC:
        match(DEC);
        break;
    case FLT:
        match(FLT);
        break;
    case OCT:
        match(OCT);
        break;
    case HEX:
        match(HEX);
        break;
    default:
        match('(');
        smpexpr();
        match(')');
    }
}

/*
 * unary -> '+' | '-' | NOT 
 */
int
unary(void)
{
    switch (lookahead) {
    case '+':
    case '-':
    case NOT:
        return lookahead;
    }
    return 0;
}

/*
 * oplus -> '+' | '-' | OR 
 */
int
oplus(void)
{
    switch (lookahead) {
    case '+':
    case '-':
    case OR:
        return lookahead;
    }
    return 0;
}

/*
 * otimes -> '*' | '/' | DIV | MOD | AND 
 */
int
otimes(void)
{
    switch (lookahead) {
    case '*':
    case '/':
    case DIV:
    case MOD:
    case AND:
        return lookahead;
    }
    return 0;
}

/*
 * relop -> '=' | '<' | '>' | NEQ | GEQ | LEQ 
 */
int
relop(void)
{
    switch (lookahead) {
    case '=':
    case '<':
    case '>':
    case NEQ:
    case GEQ:
    case LEQ:
        return lookahead;
    }
    return 0;
}


void match(int expected)
{
	if (expected == lookahead) {
		lookahead = gettoken(source);
	} else {
		fprintf(stderr, "token mismatch at line %d\n", linecount);
		exit(-2);
	}
}
