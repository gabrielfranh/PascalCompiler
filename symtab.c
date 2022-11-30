/**@<symtab.c>::**/

#include <stdio.h>
#include <symtab.h>
#include <string.h>
#include <lexer.h>

extern int symtab_next;
extern int symtab_lookup(char const *symbol);
extern int symtab_append(char const *symbol);
extern void listsymtab();

#define MAXSYMTABSIZE 1024
SYMTAB symtab[MAXSYMTABSIZE];
int symtab_next = 0;

int symtab_lookup(char const *symbol)
{
	int i;
	for(i = symtab_next-1; i > -1; i--)
	{
		if(strcmp(symtab[i].name, symbol) == 0)
			return i;
	}
	
	return i;
}

int symtab_append(char const *symbol)
{
	if (symtab_lookup(symbol) > -1)
	{
		fprintf(stderr, "%s already defined at line %d \n", symbol, linecount);
		return -1;
	}
	else
	{
		strcpy(symtab[symtab_next].name, symbol);
		return symtab_next++;
	}
}

void list_symtab()
{
	int j;
	for(j = 0; j < symtab_next; j++)
	{
		printf("Tipo: %d ", symtab[j].type);
	    	printf("Nome: %s \n", symtab[j].name);
	}
}
