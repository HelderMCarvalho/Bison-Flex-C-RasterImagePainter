/**
 * Enumeração de tipos de comandos
 * Gestão
 * Desenho
 * Coclos
 */
typedef enum {
    new, load, save,
    color, polyline, point, line, rect, rectfill, circ,
    repeat
} tipoComando;

/**
 * Estrutura que representa um valor sendo ele variável ou não
 */
typedef struct _value {
    int val;
    char *var;
} Value;

/**
 * Estrutura que representa uma cor
 */
typedef struct _cor {
    Value *r, *g, *b;
} Cor;

/**
 * Estrutura que representa a imagem
 */
typedef struct _imagem {
    int c, l;
    Cor *pixels;
} Imagem;

/**
 * Estrutura que representa uma variável e sua lista
 */
typedef struct _symbolTable {
    char *var;
    int val;
    struct _symbolTable *proximo;
} SymbolTable;

/**
 * Estrutura que representa um argumento e a sua lista
 */
typedef struct _arg {
    int tipo; //0 = x | 1 = ,
    Value *iVal1, *iVal2; //Inteiro 1 e 2
    struct _arg *proximo;
} Argumento;

/**
 * Estrutura que representa um comando e a sua lista
 */
typedef struct _comando {
    tipoComando tipo;
    struct _comando *proximo, *subComandos;
    Argumento *arg;
    Cor *cor;
    char *filename;
} Comando;

//--------------- EXECUÇÃO ---------------
void executarComandos(Comando *lst);

//--------------- COR ---------------
Cor *newCor(Value *corR, Value *corG, Value *corB);
void setDefCor(Cor *cor);
Cor *getDefCor();

//--------------- VARIÁVEIS ---------------
int findVar(SymbolTable *vars, char *var);
int getVal(SymbolTable *vars, Value *value);
SymbolTable *updateVar(SymbolTable *vars, char *var, Value *val);
Value *new_int_value(int v);
Value *new_var_value(char *v);
int randomInt(int upper);

//--------------- COMANDOS ---------------
Comando *insertNew(Comando *lst, Argumento *dim, Cor *cor);
Comando *insertSave(Comando *lst, char *filename);
Comando *insertLoad(Comando *lst, char *filename);
Comando *insertColor(Comando *lst, Cor *cor);
Comando *insertRepeat(Comando *lst, char *i, Value *inicial, Value *final, Comando *subComandos);
Comando *insertPolyline(Comando *lst, Argumento *pontos, Cor *cor);
Comando *insertLine(Comando *lst, Argumento *arg1, Argumento *arg2, Cor *cor);
Comando *insertPoint(Comando *lst, Argumento *arg1, Cor *cor);
Comando *insertCirc(Comando *lst, Argumento *arg1, Value *raio, Cor *cor);
Comando *insertRect(Comando *lst, Argumento *arg1, Argumento *arg2, Cor *cor);
Comando *insertRectFill(Comando *lst, Argumento *arg1, Argumento *arg2, Cor *cor);

//--------------- FICHEIROS ---------------
void createNewImgFile(Argumento *dims, Cor *cor);
void saveImgFile(char *filename);
void loadImgFile(char *filename);

//--------------- ARGUMENTOS ---------------
Argumento *newArg(Argumento *argList, Value *x, Value *y, int tipo);

//--------------- DESENHO ---------------
void drawPoint(int col, int linhas, Cor *cor);
void drawLine(Argumento *arg, Cor *cor);
void drawCirc(Argumento *arg, Cor *cor);
void drawRect(Argumento *arg, Cor *cor);
void drawRectFill(Argumento *arg, Cor *cor);
void drawPolyline(Argumento *arg, Cor *cor);
