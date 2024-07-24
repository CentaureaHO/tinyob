%{
#include "parser.hpp"
#include <iostream>
extern int yyerror(const char *, ...);
extern "C" int my_yywrap();  // Renaming the yywrap function to avoid conflict
%}

%%
"exit"  { return EXIT; }
"+"     { return ADD; }
"-"     { return SUB; }
"*"     { return MUL; }
"/"     { return DIV; }
"|"     { return ABS; }
"("     { return OP; }
")"     { return CP; }
[-+]?[0-9]+ { yylval = atoi(yytext); return NUMBER; }

\n      { return EOL; }
"//".*  
[ \t]   { /* ignore white space */ }
.       { yyerror("Mystery character %c\n", *yytext); }
%%

int yywrap() { return 1; }  // Definition of the renamed yywrap function

int yyerror(const char *s, ...)
{
    (void)s; // Explicitly ignore the unused parameter
    std::cout << s << "\n";
    return 0;
}
