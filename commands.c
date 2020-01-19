#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "commands.h"

#define max(x, y)    (x > y) ? x : y
#define min(x, y)    (x < y) ? x : y

SymbolTable *vars = NULL;
Cor *corDef = NULL;
Imagem *img = NULL;

/**
 * Devolve o índice do array de uma dada coordenada
 * @param c -> Coluna a encontrar
 * @param l -> Linha a encontrar
 * @return -> Índice representante da coordenada no array
 */
int getPonto(int c, int l) {
    return (c - 1) * img->l + (l - 1);
}

/**
 * Executa os comandos da lista de comandos para execução
 * Verifica o tipo de cada comando e executa-o da forma correta
 * @param lst -> Lista de comandos para execução
 */
void executarComandos(Comando *lst) {
    if (!lst) { return; }
    switch (lst->tipo) {
        case new: {
            createNewImgFile(lst->arg, lst->cor);
            break;
        }
        case save: {
            saveImgFile(lst->filename);
            break;
        }
        case load: {
            loadImgFile(lst->filename);
            break;
        }
        case polyline: {
            drawPolyline(lst->arg, lst->cor);
            break;
        }
        case repeat: {
            int i = 0;
            int min = getVal(vars, lst->arg->proximo->iVal1);
            int max = getVal(vars, lst->arg->proximo->iVal2);
            for (i = min; i < max; i++) {
                vars = updateVar(vars, lst->arg->iVal1->var, new_int_value(i));
                executarComandos(lst->subComandos);
            }
            break;
        }
        case color: {
            setDefCor(lst->cor);
            break;
        }
        case line: {
            drawLine(lst->arg, lst->cor);
            break;
        }
        case circ: {
            drawCirc(lst->arg, lst->cor);
            break;
        }
        case rect: {
            drawRect(lst->arg, lst->cor);
            break;
        }
        case rectfill: {
            drawRectFill(lst->arg, lst->cor);
            break;
        }
        case point: {
            drawPoint(getVal(vars, lst->arg->iVal2), getVal(vars, lst->arg->iVal1), lst->cor);
            break;
        }
    }
    executarComandos(lst->proximo);
}

/**
 * Define a cor por defeito
 * @param cor -> Cor a ser definida como padrão
 */
void setDefCor(Cor *cor) {
    if (corDef == NULL) {
        corDef = (Cor *) malloc(sizeof(Cor));
    }
    corDef = cor;
}

/**
 * Devolve a cor padrão do programa
 * @return -> Nova cor
 */
Cor *getDefCor() {
    if (corDef == NULL) {
        corDef = (Cor *) malloc(sizeof(Cor));
        corDef->r = corDef->g = corDef->b = new_int_value(255);
    }
    return corDef;
}

/**
 * Cria uma nova cor
 * @param corR -> Valor Red (Vermelho)
 * @param corG -> Valor Green (Verde)
 * @param corB -> Valor Blue (Azul)
 * @return -> Nova cor
 */
Cor *newCor(Value *corR, Value *corG, Value *corB) {
    Cor *node = malloc(sizeof(Cor));
    node->r = corR;
    node->g = corG;
    node->b = corB;
    return node;
}

/**
 * Atualiza ou cria uma nova variável
 * @param vars -> Lista de variáveis já existentes
 * @param var -> Nome da variável a encontrar ou criar
 * @param val -> Inteiro a guardar
 * @return -> Lista de Variáveis já existentes com a nova adicionada ou atualizada
 */
SymbolTable *updateVar(SymbolTable *vars, char *var, Value *val) {
    if (vars == NULL) {
        SymbolTable *node = (SymbolTable *) malloc(sizeof(SymbolTable));
        node->proximo = NULL;
        node->var = var; // keep a fresh copy of the variable name
        node->val = getVal(vars, val);
        return node;
    } else {
        if (strcmp(vars->var, var) == 0) {
            vars->val = getVal(vars, val);
            return vars;
        } else {
            vars->proximo = updateVar(vars->proximo, var, val);
            return vars;
        }
    }
}

/**
 * Procura uma variável e devolve o seu Inteiro
 * @param vars -> Lista de variáveis já existentes
 * @param var -> Nome da variável a encontrar
 * @return -> Inteiro da variável
 */
int findVar(SymbolTable *vars, char *var) {
    if (vars == NULL) {
        fprintf(stderr, "A variavel %s não está definida!\n", var);
        exit(1);
    }
    if (strcmp(vars->var, var) == 0) {
        return vars->val;
    } else {
        return findVar(vars->proximo, var);
    }
}

/**
 * Devolve o Inteiro de um Valor fixo ou de uma Variável
 * Verifica de se o Valor recebido como argumento é uma Variável ou não, se for procura o seu valor senão devolve o
 * Inteiro do Valor
 * @param vars -> Lista de variáveis já existentes
 * @param value -> Valor
 * @return -> Inteiro do Valor ou da Variável
 */
int getVal(SymbolTable *vars, Value *value) {
    if (value->var == NULL) { return value->val; }
    return findVar(vars, value->var);
}

/**
 * Cria um valor com um inteiro (que não pode ser modificado)
 * @param v -> Inteiro
 * @return -> Novo valor
 */
Value *new_int_value(int v) {
    Value *val = (Value *) malloc(sizeof(Value));
    val->val = v;
    val->var = NULL;
    return val;
}

/**
 * Cria um valor com uma variável (que pode posteriormente ser modificada)
 * @param v -> Nome da variável
 * @return -> Novo valor
 */
Value *new_var_value(char *v) {
    Value *val = (Value *) malloc(sizeof(Value));
    val->var = v;
    return val;
}

//--------------- COMANDOS ---------------

/**
 * Insere uma ocorrência do comando New na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param dim -> Dimensões da imagem
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertNew(Comando *lst, Argumento *dim, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = new;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = dim;
    node->cor = cor;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Save na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param filename -> Nome do ficheiro a guardar
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertSave(Comando *lst, char *filename) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = save;
    node->filename = filename;
    node->subComandos = NULL;
    node->arg = NULL;
    node->cor = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Load na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param filename -> Nome do ficheiro a carregar
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertLoad(Comando *lst, char *filename) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = load;
    node->filename = filename;
    node->subComandos = NULL;
    node->arg = NULL;
    node->cor = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Color na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertColor(Comando *lst, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = color;
    node->cor = cor;
    node->proximo = lst;
    node->subComandos = NULL;
    setDefCor(cor);
    return node;
}

/**
 * Insere uma ocorrência do comando Repeat na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param i -> Variável a ser incrementada em cada iteração
 * @param inicial -> Valor inicial da variável a ser incrementada
 * @param final -> Valor final da variável a ser incrementada
 * @param subComandos -> Lista de Comandos a serem executados em cada iteração
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertRepeat(Comando *lst, char *i, Value *inicial, Value *final, Comando *subComandos) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = repeat;
    node->subComandos = subComandos;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg->iVal1 = new_var_value(i);
    node->arg->proximo = (Argumento *) malloc(sizeof(Argumento));
    node->arg->proximo->iVal1 = inicial;
    node->arg->proximo->iVal2 = final;
    node->proximo = lst;
    node->filename = NULL;
    node->cor = NULL;
    return node;
}

/**
 * Insere uma ocorrência do comando Polyline na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param arg1 -> Argumento a ser utilizado na execução do comando (lista de pontos)
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertPolyline(Comando *lst, Argumento *arg1, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = polyline;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = arg1;
    node->cor = cor;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Linha na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param arg1 -> Argumento a ser utilizado na execução do comando (ponto inicial da linha)
 * @param arg2 -> Argumento a ser utilizado na execução do comando (ponto final da linha)
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertLine(Comando *lst, Argumento *arg1, Argumento *arg2, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = line;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = arg1;
    node->arg->proximo = arg2;
    node->cor = cor;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Retângulo na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param arg1 -> Argumento a ser utilizado na execução do comando (ponto inicial do retângulo)
 * @param arg2 -> Argumento a ser utilizado na execução do comando (ponto final do retângulo ou o seu tamanho)
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertRect(Comando *lst, Argumento *arg1, Argumento *arg2, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = rect;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = arg1;
    node->arg->proximo = arg2;
    node->cor = cor;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Retângulo Preenchido na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param arg1 -> Argumento a ser utilizado na execução do comando (ponto inicial do retângulo)
 * @param arg2 -> Argumento a ser utilizado na execução do comando (ponto final do retângulo ou o seu tamanho)
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertRectFill(Comando *lst, Argumento *arg1, Argumento *arg2, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = rectfill;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = arg1;
    node->arg->proximo = arg2;
    node->cor = cor;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Circulo na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param arg1 -> Argumento a ser utilizado na execução do comando
 * @param raio -> Raio do circulo
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertCirc(Comando *lst, Argumento *arg1, Value *raio, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = circ;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = arg1;
    node->arg->proximo = (Argumento *) malloc(sizeof(Argumento));
    node->arg->proximo->iVal1 = raio;
    node->cor = cor;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

/**
 * Insere uma ocorrência do comando Point na lista de comandos a executar
 * @param lst -> Lista de comandos já existentes para execução
 * @param arg1 -> Argumento a ser utilizado na execução do comando
 * @param cor -> Cor a ser utilizada na execução do comando
 * @return -> Lista de Comandos já existentes com o novo adicionado
 */
Comando *insertPoint(Comando *lst, Argumento *arg1, Cor *cor) {
    Comando *node = (Comando *) malloc(sizeof(Comando));
    node->tipo = point;
    node->arg = (Argumento *) malloc(sizeof(Argumento));
    node->arg = arg1;
    node->cor = cor;
    node->arg->proximo = NULL;
    node->filename = NULL;
    node->subComandos = NULL;
    node->proximo = lst;
    return node;
}

//--------------- GESTÃO DE IMAGENS ---------------
/**
 * Cria uma nova imagem
 * Aloca a memória necessária para a armazenar em memória tendo em conta as suas dimensões
 * @param dims -> Dimensões da imagem a criar
 * @param cor -> Cor de fundo da imagem
 */
void createNewImgFile(Argumento *dims, Cor *cor) {
    int i;
    int c = getVal(vars, dims->iVal1);
    int l = getVal(vars, dims->iVal2);

    img = malloc(sizeof(Imagem));
    img->pixels = malloc(c * l * sizeof(Cor));

    img->c = c;
    img->l = l;

    for (i = 0; i < img->l * img->c; i++) {
        img->pixels[i] = *cor;
    }
}

/**
 * Guarda a imagem em memoria num ficheiro ".pnm"
 * @param filename -> Nome do ficheiro a guardar
 */
void saveImgFile(char *filename) {
    FILE *ficheiro = fopen(filename, "wb");

    fprintf(ficheiro, "P3\n");
    fprintf(ficheiro, "%d %d\n", img->c, img->l);
    fprintf(ficheiro, "255\n");

    for (int i = 0; i < (img->c * img->l); i++) {
        fprintf(ficheiro, "%d %d %d \t", getVal(vars, img->pixels[i].r), getVal(vars, img->pixels[i].g),
                getVal(vars, img->pixels[i].b));
    }

    fclose(ficheiro);
}

/**
 * Carrega para memoria uma imagem ".pnm" já existente
 * @param filename -> Nome do ficheiro a carregar
 */
void loadImgFile(char *filename) {
    FILE *ficheiro = fopen(filename, "r+");
    char tipo[3];
    int colunas = 0, linhas = 0, profundidade = 0;

    fscanf(ficheiro, "%s %d %d %d", tipo, &linhas, &colunas, &profundidade);

    free(img);

    img = malloc(sizeof(Imagem));
    img->pixels = malloc(linhas * colunas * sizeof(Cor));
    img->c = linhas;
    img->l = colunas;

    if (ficheiro) {
        for (int i = 0; i < (img->c * img->l); i++) {
            int r = getVal(vars, img->pixels[i].r);
            int g = getVal(vars, img->pixels[i].g);
            int b = getVal(vars, img->pixels[i].b);

            fscanf(ficheiro, "%d %d %d ", &r, &g, &b);
        }
    } else {
        printf("Não existe nenhum ficheiro com o nome \"%s\"\n", filename);
    }
    fclose(ficheiro);
}

//--------------- DESENHO ---------------

/**
 * Cria um Novo Argumento
 * @param argList -> Lista de Argumentos já existentes
 * @param x -> Valor x da coordenada do ponto
 * @param y -> Valor y da coordenada do ponto
 * @param tipo -> Tipo do Argumento
 * @return -> Lista de Argumentos já existentes com o novo adicionado
 */
Argumento *newArg(Argumento *argList, Value *x, Value *y, int tipo) {
    Argumento *arg = malloc(sizeof(Argumento));
    arg->iVal1 = x;
    arg->iVal2 = y;
    arg->tipo = tipo;
    arg->proximo = argList;
    return arg;
}

/**
 * Desenha um ponto numa coordenada
 * @param y -> Valor y da coordenada do ponto
 * @param x -> Valor x da coordenada do ponto
 * @param cor -> Cor do ponto
 */
void drawPoint(int y, int x, Cor *cor) {
    img->pixels[getPonto(y, x)] = *cor;
}

/**
 * Desenha uma linha
 * Verifica que tipo de linha é (Vertical, Horizontal, Obliqua) e desenha-a de acordo com o seu tipo
 * @param arg -> Argumentos da linha
 * @param cor -> Cor da linha
 */
void drawLine(Argumento *arg, Cor *cor) {
    int x1 = getVal(vars, arg->iVal1);
    int y1 = getVal(vars, arg->iVal2);
    int x2 = getVal(vars, arg->proximo->iVal1);
    int y2 = getVal(vars, arg->proximo->iVal2);

    int xAux = (x2 - x1);
    (xAux < 0) ? xAux = xAux * (-1) : xAux;
    int yAux = (y2 - y1);
    (yAux < 0) ? yAux = yAux * (-1) : yAux;

    if (x1 == x2) {
        int yMin = min(y1, y2);
        int yMax = max(y1, y2);
        for (int i = yMin; i <= yMax; i++) {
            img->pixels[getPonto(i, x1)] = *cor;
        }
    } else if (y1 == y2) {
        int xMin = min(x1, x2);
        int xMax = max(x1, x2);
        for (int i = xMin; i <= xMax; i++) {
            img->pixels[getPonto(y1, i)] = *cor;
        }
    } else if (xAux >= yAux) {
        int xMin = min(x1, x2);
        int xMax = max(x1, x2);
        float m = (float) (y2 - y1) / (float) (x2 - x1);
        float b = (float) y1 - m * (float) x1;
        for (int x = xMin; x <= xMax; x++) {
            int y = m * x + b;
            drawPoint(y, x, cor);
        }
    } else if (yAux > xAux) {
        int yMin = min(y1, y2);
        int yMax = max(y1, y2);
        float m = (float) (y2 - y1) / (float) (x2 - x1);
        float b = (float) y1 - m * (float) x1;
        for (int y = yMin; y <= yMax; y++) {
            int x = (y - b) / m;
            if (x == 0) { x = 1; }
            drawPoint(y, x, cor);
        }
    }
}

/**
 * Desenha um circulo
 * Faz pontos afastados por um raio em volta de uma coordenada específica
 * @param arg -> Argumentos do circulo
 * @param cor -> Cor do circulo
 */
void drawCirc(Argumento *arg, Cor *cor) {
    int x = getVal(vars, arg->iVal1);
    int y = getVal(vars, arg->iVal2);
    int r = getVal(vars, arg->proximo->iVal1);

    float theta;

    for (theta = 0; theta <= 2 * M_PI; theta += 0.01f) {
        int xx = cosf(theta) * r + x;
        int yy = sinf(theta) * r + y;
        drawPoint(yy, xx, cor);
    }
}

/**
 * Desenha um retângulo
 * Verifica o tipo de Argumento que recebeu, se for dimensão calcula o ponto oposto,
 * se já receber o ponto oposto não faz nada
 * Faz uma linha para cada um dos lados
 * @param arg -> Argumentos do retângulo
 * @param cor -> Cor do retângulo
 */
void drawRect(Argumento *arg, Cor *cor) {
    int x1 = getVal(vars, arg->iVal1);
    int y1 = getVal(vars, arg->iVal2);
    int x2, y2;

    if (arg->proximo->tipo) {
        x2 = getVal(vars, arg->proximo->iVal1);
        y2 = getVal(vars, arg->proximo->iVal2);
    } else {
        x2 = getVal(vars, arg->proximo->iVal1) + x1;
        y2 = getVal(vars, arg->proximo->iVal2) + y1;
    }

    for (int l = 1; l <= 4; l++) {
        Argumento *argTemp = malloc(sizeof(Argumento));
        argTemp->iVal1 = (l == 1 || l == 4) ? new_int_value(x1) : new_int_value(x2);
        argTemp->iVal2 = (l == 1 || l == 2) ? new_int_value(y1) : new_int_value(y2);
        argTemp->proximo = malloc(sizeof(Argumento));
        argTemp->proximo->iVal1 = (l == 3 || l == 4) ? new_int_value(x1) : new_int_value(x2);
        argTemp->proximo->iVal2 = (l == 1 || l == 4) ? new_int_value(y1) : new_int_value(y2);
        drawLine(argTemp, cor);
        free(argTemp->proximo);
        free(argTemp);
    }
}

/**
 * Desenha um retângulo preenchido
 * Verifica o tipo de Argumento que recebeu, se for dimensão calcula o ponto oposto,
 * se já receber o ponto oposto não faz nada
 * Faz uma linha para cada um dos y's
 * @param arg -> Argumentos do retângulo
 * @param cor -> Cor do retângulo
 */
void drawRectFill(Argumento *arg, Cor *cor) {
    int x1 = getVal(vars, arg->iVal1);
    int y1 = getVal(vars, arg->iVal2);
    int x2, y2;

    if (arg->proximo->tipo) {
        x2 = getVal(vars, arg->proximo->iVal1);
        y2 = getVal(vars, arg->proximo->iVal2);
    } else {
        x2 = getVal(vars, arg->proximo->iVal1) + x1;
        y2 = getVal(vars, arg->proximo->iVal2) + y1;
    }

    int yMin = min(y1, y2);
    int yMax = max(y1, y2);

    for (int y = yMin; y <= yMax; y++) {
        Argumento *argTemp = malloc(sizeof(Argumento));
        argTemp->iVal1 = new_int_value(x1);
        argTemp->iVal2 = new_int_value(y);
        argTemp->proximo = malloc(sizeof(Argumento));
        argTemp->proximo->iVal1 = new_int_value(x2);
        argTemp->proximo->iVal2 = new_int_value(y);
        drawLine(argTemp, cor);
        free(argTemp->proximo);
        free(argTemp);
    }
}

/**
 * Desenha uma Polyline
 * Guarda o primeiro ponto, guarda o próximo ponto, desenha a linha, passa esse próximo ponto para o lugar do primeiro
 * ponto, guarda o próximo ponto de novo e repete isto até não existir próximo ponto.
 * @param arg -> Argumentos da Polyline
 * @param cor -> Cor da Polyline
 */
void drawPolyline(Argumento *arg, Cor *cor) {
    int x1 = getVal(vars, arg->iVal1);
    int y1 = getVal(vars, arg->iVal2);
    int x2, y2, polypoints = 0;

    Argumento *argAux = arg->proximo;

    while (argAux != NULL) {
        x2 = getVal(vars, argAux->iVal1);
        y2 = getVal(vars, argAux->iVal2);
        ++polypoints;

        Argumento *argTemp = malloc(sizeof(Argumento));
        argTemp->iVal1 = new_int_value(x1);
        argTemp->iVal2 = new_int_value(y1);
        argTemp->proximo = malloc(sizeof(Argumento));
        argTemp->proximo->iVal1 = new_int_value(x2);
        argTemp->proximo->iVal2 = new_int_value(y2);
        drawLine(argTemp, cor);
        free(argTemp->proximo);
        free(argTemp);

        x1 = getVal(vars, argAux->iVal1);
        y1 = getVal(vars, argAux->iVal2);

        argAux = argAux->proximo;
    }
    free(argAux);
}

/**
 * Devolve um número aleatório entre 0 e um dado número
 * @param upper -> Número máximo de geração (não gera acima deste)
 * @return -> Número aleatório gerado
 */
int randomInt(int upper) {
    srand(time(0));
    return rand() % (upper + 1);
}
