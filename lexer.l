%{

  #include "commands.h"
  #include "grammar.tab.h"
  #include <strings.h>
  #include <string.h>

%}

numbers     ([0-9]+)
filename    ([a-zA-Z0-9_])+\.pnm
variable    ([a-wy-z]([a-z][0-9]?)*)

%%

{numbers}   { yylval.iVal = atof(yytext);   return INT;      }
{filename}  { yylval.sVal = strdup(yytext); return FILENAME; }
{variable}  { yylval.sVal = strdup(yytext); return VAR; }



NEW         { return NEW; }
LOAD        { return LOAD; }
SAVE        { return SAVE; }
COLOR       { return COLOR; }
POLYLINE    { return POLYLINE; }
POINT       { return POINT; }
LINE        { return LINE; }
RECT        { return RECT; }
RECTFILL    { return RECTFILL; }
CIRC        { return CIRC;}

RAND        { return RAND;}

FOR         { return REPEAT; }
IN          { return IN; }
DO          { return DO; }
END         { return END; }



[\n ]+      { }
.           { return yytext[0]; }


%%

int yywrap(){ return 1; }
