#include "commands.h"
#include "grammar.tab.h"

int main() {
  yyparse();
  return 0;
}
