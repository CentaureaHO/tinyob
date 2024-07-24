%{
#include <stdio.h>
#include <string>
int yyerror(const char *, ...);
extern int yylex();
extern int yyparse(std::string sql);
%}

/* declare tokens */
%token NUMBER
%token ADD SUB MUL DIV ABS
%token OP CP
%token EOL
%token EXIT

/* declare operator precedence and associativity */
%left ADD SUB
%left MUL DIV
%right ABS

%%
calclist:
    /* nothing */
    | calclist exp EOL { printf("= %d\n> ", $2); }
    | calclist EOL { printf("> "); } /* blank line or a comment */
    | calclist EXIT { printf("Exiting...\n"); exit(0); }
;

exp:
    factor
    | exp ADD exp { $$ = $1 + $3; }
    | exp SUB factor { $$ = $1 - $3; }
    | ABS exp { $$ = $2 >= 0 ? $2 : -$2; }
;

factor:
    term
    | factor MUL term { $$ = $1 * $3; }
    | factor DIV term { $$ = $1 / $3; }
;

term:
    NUMBER
    | OP exp CP { $$ = $2; }
;

%%