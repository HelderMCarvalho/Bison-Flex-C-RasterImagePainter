%{
  #include <stdio.h>
  #include "commands.h"
  #include "grammar.tab.h"

  int yylex();
  int yyerror(char*);

  extern SymbolTable *vars;
%}

%union {
  int iVal; /* Int */
  char *sVal; /* String */
  Comando *comando;
  Value *value;
  Argumento *argumento;
  Cor *cor;
}

%token <sVal> NEW COLOR REPEAT IN DO END VAR FILENAME SAVE LOAD POLYLINE POINT RECT RECTFILL CIRC LINE RAND
%token <iVal> INT

%type <comando> listaComandos comando
%type <value> value
%type <argumento> polypoints dims ponto pontoOrDim
%type <cor> cor

%%

program: listaComandos { executarComandos($1); };
listaComandos: comando { $$ = $1; }
	     | comando listaComandos { $1->proximo = $2; $$ = $1; };

comando: VAR '=' value ';' { vars = updateVar(vars, $1, $3);}
       | VAR '=' RAND INT ';' { vars = updateVar(vars, $1, new_int_value(randomInt($4)));}

       | NEW dims ';' { $$ = insertNew(NULL, $2, getDefCor());}
       | NEW dims cor ';'{ $$ = insertNew( NULL, $2, $3);}

       | SAVE '"' FILENAME '"' ';' { $$ = insertSave(NULL,$3);}

       | LOAD '"' FILENAME '"' ';' { $$ = insertLoad(NULL, $3);}

       | COLOR cor ';' { $$ = insertColor(NULL, $2);}

       | REPEAT VAR IN '[' value '.''.' value ']' DO listaComandos END REPEAT';'
                { $$ = insertRepeat(NULL, $2, $5, $8, $11); }

       | POLYLINE polypoints';' { $$ = insertPolyline(NULL, $2, getDefCor()); }
       | POLYLINE polypoints cor';' { $$ = insertPolyline(NULL, $2, $3); }

       | LINE ponto ponto ';' { $$ = insertLine(NULL, $2, $3, getDefCor()); }
       | LINE ponto ponto cor ';' { $$ = insertLine(NULL, $2, $3, $4); }

       | POINT ponto ';' { $$ = insertPoint(NULL, $2, getDefCor()); }
       | POINT ponto cor ';' { $$ = insertPoint(NULL, $2, $3); }

       | CIRC ponto value ';' { $$ = insertCirc(NULL, $2, $3, getDefCor()); }
       | CIRC ponto value cor ';' { $$ = insertCirc(NULL, $2, $3, $4); }

       | RECT ponto pontoOrDim ';' { $$ = insertRect(NULL, $2, $3, getDefCor()); }
       | RECT ponto pontoOrDim cor ';' { $$ = insertRect(NULL, $2, $3, $4); }

       | RECTFILL ponto pontoOrDim ';' { $$ = insertRectFill(NULL, $2, $3, getDefCor()); }
       | RECTFILL ponto pontoOrDim cor ';' { $$ = insertRectFill(NULL, $2, $3, $4); }
       ;

polypoints : ponto { $$ = $1; }
           | ponto polypoints { $1->proximo = $2; $$ = $1; }
           ;
cor: value ':' value ':' value { $$ = newCor($1, $3, $5); };
pontoOrDim : dims { $$ = $1; }
           | ponto { $$ = $1; }
           ;
dims: value 'x' value { $$ = newArg(NULL, $1, $3, 0); };
ponto: value ',' value { $$ = newArg(NULL, $1, $3, 1); };
value: INT { $$ = new_int_value($1); }
     | VAR { $$ = new_var_value($1); }
     ;

%%

int yyerror(char* msg){
  printf("ERROR: %s\n", msg);
  return 0;
}
