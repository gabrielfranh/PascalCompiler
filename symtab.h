/**@<symtab.h>::**/

#include<lexer.h>

extern int lexlevel;

 typedef struct _symtab_ {
	char name[MAXIDLEN+1];
	int type;
	int lexlevel;
	int objtype; /** 0, 1, 2 **/
} SYMTAB;

extern int symtab_next;
extern int symtab_lookup(char const *symbol);
extern int symtab_append(char const *symbol);
extern void list_symtab();
extern SYMTAB symtab[];
