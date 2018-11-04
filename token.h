#ifndef TOKEN_H_
#define TOKEN_H_

#include "vector.h"

// token type
enum {
	tNumber,
	tTermStart, tTermEnd,
	tOperator,
	tExprEnd,
	tEOF,
};

// operator type
enum {
	oAdd = 0,
	oSub,
	oMul,
	oDiv,
};

typedef struct {
	char *str;  // token string
	int type;   // token type
	int val;    // number
} token_t;

const char* get_token_type(int type);
void print_token(vector_t *tokens);

token_t* new_token(char *str, int type, int val);
vector_t* tokenize(char *src);

#endif
