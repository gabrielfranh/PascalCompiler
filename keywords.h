#pragma once

int iskeyword(char const *);

enum {
	BEGIN = 4096,
	PROGRAM,
	PROCEDURE,
	FUNCTION,
	IF,
	THEN,
	ELSE,
	WHILE,
	DO,
	REPEAT,
	UNTIL,
	VAR,
	INTEGER,
	REAL,
	DOUBLE,
	BOOLEAN,
	NOT,
	OR,
	DIV,
	MOD,
	AND,
	END
};
