/*
*    C Calculator With Lexer
*    
*    Example inputs/outputs:
*
*    Input: (12 + 5) * 6 + 12 / 3  - 100
*    Output: 6
*
*    Input: (12 + 5) * 39 * ( 12 - 4
*    Output: 663 * ( 8 
*    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    EMPTY,
    PAR_OPEN,
    PAR_CLOSE,
    NUM,
    PLUS,
    MINUS,
    DIVIDE,
    MULTIPLY
} TokenType;

typedef struct {
    TokenType type;
    int value;
    char identifier;
    char *name;
} Token;

void printTokenList(Token** tokens, int token_count);
void printTokenListWithIdentifiers(Token** tokens, int token_count);
void shiftTokensToLeft(Token** tokens, int* token_count, int start_pos, int steps);

Token* createToken(const TokenType type, const int value, const char* name, const char identifier) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->identifier = identifier;
    token->name = malloc(strlen(name) + 1);
    strcpy(token->name, name);
    return token;
}

Token* copyToken(const Token* token) {
    if (token == NULL) {
        return NULL;
    }
    Token* copy = createToken(token->type, token->value, token->name, token->identifier);
    return copy;
}

void printTokenList(Token** tokens, const int token_count) {
    if (tokens == NULL)
        return;
    for (int i = 0; i < token_count; i++) {
        const Token *t = tokens[i];
        if (t == NULL) {
            printf("#NULL!#");
        } else {
            printf("%s", t->name);
            if (t->type == NUM) {
                printf("(%d)", t->value);
            }
        }
        printf(" ");
    }
    printf("\n");
}

void printTokenListWithIdentifiers(Token** tokens, const int token_count) {
    if (tokens == NULL)
        return;
    for (int i = 0; i < token_count; i++) {
        const Token *t = tokens[i];
        if (t == NULL) {
            printf("#NULL!#");
        } else {
            if (t->identifier) {
                printf("%c", t->identifier);
            }
            if (t->type == NUM) {
                printf("%d", t->value);
            }
        }
        printf(" ");
    }
    printf("\n");
}

void shiftTokensToLeft(Token** tokens, int* token_count, const int start_pos, const int steps) {
    if (tokens == NULL || token_count == NULL)
        return;
    for (int i = 0; i < steps; i++) {
        free(tokens[start_pos - steps + i]->name);
        free(tokens[start_pos - steps + i]);
    }
    for (int i = start_pos - steps; i < *token_count - steps; i++) {
        tokens[i] = tokens[i + steps];
    }
    *token_count -= steps;
}

int iterateCalculation(Token** tokens, int* token_count) {
    if (*token_count < 3 || tokens == NULL) {
        return 0;
    }
    int changed = 0;

    // IS THERE NUM DIVIDE NUM
    for (int i = 2; i < *token_count; i++) {
        if (i < 2) {
            continue;
        }
        if (tokens[i-2]->type == NUM && tokens[i-1]->type == DIVIDE && tokens[i]->type == NUM) {
            tokens[i-2]->value = tokens[i-2]->value / tokens[i]->value;
            shiftTokensToLeft(tokens, token_count, i + 1, 2);
            i -= 2;
            changed = 1;
        }
    }

    // IS THERE NUM MULTIPLY NUM
    for (int i = 2; i < *token_count; i++) {
        if (i < 2) {
            continue;
        }
        if (tokens[i-2]->type == NUM && tokens[i-1]->type == MULTIPLY && tokens[i]->type == NUM) {
            tokens[i-2]->value = tokens[i-2]->value * tokens[i]->value;
            shiftTokensToLeft(tokens, token_count, i + 1, 2);
            i -= 2;
            changed = 1;
        }
    }

    // IS THERE ADDITION OR SUBTRACTION
    for (int i = 2; i < *token_count; i++) {
        if (i < 2) {
            continue;
        }
        if (tokens[i-2]->type == NUM && (tokens[i-1]->type == PLUS || tokens[i-1]->type == MINUS) && tokens[i]->type == NUM
            && (i-3 < 0 || (tokens[i-3]->type != MULTIPLY && tokens[i-3]->type != DIVIDE))
            && (i + 1 >= *token_count || (tokens[i+1]->type != MULTIPLY && tokens[i+1]->type != DIVIDE))) {
            if (tokens[i-1]->type == PLUS) {
                tokens[i-2]->value = tokens[i-2]->value + tokens[i]->value;
            } else {
                tokens[i-2]->value = tokens[i-2]->value - tokens[i]->value;
            }

            shiftTokensToLeft(tokens, token_count, i + 1, 2);
            i -= 2;
            changed = 1;
        }
    }

    // IS THERE PARENTHESIS
    for (int i = 2; i < *token_count; i++) {
        if (i < 2) {
            continue;
        }
        if (tokens[i-2]->type == PAR_OPEN && tokens[i-1]->type == NUM && tokens[i]->type == PAR_CLOSE) {
            free(tokens[i-2]->name);
            free(tokens[i-2]);
            tokens[i-2] = copyToken(tokens[i - 1]);
            shiftTokensToLeft(tokens, token_count, i + 1, 2);
            i -= 2;
            changed = 1;
        }
    }

    return changed;
}

int tokenize(Token **tokens, const char* buffer, unsigned long buffer_size) {
    int token_count = 0;
    for (int i = 0; i < buffer_size; i++) {
        if (buffer[i] == ' ') {
            continue;
        }
        if (buffer[i] == '(') {
            tokens[token_count] = createToken(PAR_OPEN, 0, "PAR_OPEN", '(');
            token_count++;
        } else if (buffer[i] == ')') {
            tokens[token_count] = createToken(PAR_CLOSE, 0, "PAR_CLOSE", ')');
            token_count++;
        } else if (buffer[i] == '/') {
            tokens[token_count] = createToken(DIVIDE, 0, "DIVIDE", '/');
            token_count++;
        } else if (buffer[i] == '-') {
            tokens[token_count] = createToken(MINUS, 0, "MINUS", '-');
            token_count++;
        } else if (buffer[i] == '+') {
            tokens[token_count] = createToken(PLUS, 0, "PLUS", '+');
            token_count++;
        } else if (buffer[i] == '*') {
            tokens[token_count] = createToken(MULTIPLY, 0, "MULTIPLY", '*');
            token_count++;
        } else if ('0' <= buffer[i] && buffer[i] <= '9') {
            Token *token = createToken(NUM, 0, "NUM", 0);
            tokens[token_count] = token;
            token_count++;
            while (buffer[i] >= '0' && buffer[i] <= '9') {
                token->value = token->value * 10 + buffer[i] - '0';
                i++;
            }
            i --;
        } else {
            if (buffer[i] != 10) {
                printf("Character at position %d: %c (ASCII: %d)\n", i, buffer[i], buffer[i]);
            }
        }
    }
    return token_count;
}

int main(void) {
    const int maxBufferSize = 256;
    char buffer[maxBufferSize];
    printf("Enter an expression: ");
    fgets(buffer, maxBufferSize, stdin);

    const unsigned long length = strlen(buffer);

    // TOKENIZE
    const int max_tokens = 255;
    Token **tokens = malloc(max_tokens * sizeof(Token *));
    int token_count = tokenize(tokens, buffer, length);

    for (int i = token_count; i < max_tokens; i++) {
        free(tokens[i]);
    }

    for (int i = 0; i < 100; i ++) {
        int changed = iterateCalculation(tokens, &token_count);
        if (changed == 0) {
            break;
        }
        if (i == 99) {
            printf("Maximum depth reached!\nExiting...\n");
        }
    }

    printf("The result: ");
    printTokenListWithIdentifiers(tokens, token_count);

    for (int i = 0; i < token_count; i++) {
        free(tokens[i]->name);
        free(tokens[i]);
    }
    free(tokens);

    return 0;
}
