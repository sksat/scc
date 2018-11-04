#include <stdio.h>
#include <stdlib.h>
#include "token.h"

#define bool _Bool
#define true 1
#define false0

const char* get_token_type(int type){
	switch(type){
		case tNumber:
			return "number";
		case tTermStart:
			return "term start";
		case tTermEnd:
			return "term end";
		case tOperator:
			return "operator";
		case tEOF:
			return "EOF";
		default:
			return "unknown";
	}
}

void print_token(vector_t *tokens){
	token_t *t, *t2;

	fprintf(stderr, "token(%ld)\n", tokens->size);

	for(size_t i=0;i<tokens->size;i++){
		t = vector_get(tokens, i);
		if(i+1 == tokens->size)
			fprintf(stderr, "[%s] %s\n", t->str, get_token_type(t->type));
		else{
			t2 = vector_get(tokens, i+1);
			int len = t2->str - t->str;
			fprintf(stderr, "[%.*s] %s\n", len, t->str, get_token_type(t->type));
		}
	}

	fprintf(stderr, "\n");
}

token_t* new_token(char *str, int type, int val){
	token_t *token = malloc(sizeof(token_t));
	token->str = str;
	token->type= type;
	token->val = val;
	return token;
}

bool is_space(char c){
	return (c==' ' || c=='\t' || c=='\n');
}

vector_t* tokenize(char *src){
	token_t *t;
	vector_t *tokens = vector_new(0);

	while(*src != '\0'){
		// space
		if(is_space(*src)){
			src++;
			continue;
		}

		// number
		if('0' <= *src && *src <= '9'){
			t = new_token(src, tNumber, strtol(src, &src, 10));
			vector_push_back(tokens, t);
			continue;
		}

		// ( )
		if(*src == '(' || *src == ')'){
			t = new_token(src, (*src=='(' ? tTermStart : tTermEnd), 0);
			vector_push_back(tokens, t);
			src++;
			continue;
		}

		// operator
		int op;
		switch(*src){
			case '+':
				op = oAdd;
				break;
			case '-':
				op = oSub;
				break;
			case '*':
				op = oMul;
				break;
			case '/':
				op = oDiv;
				break;
			default:
				fprintf(stderr, "tokenize error: %s\n", src);
				exit(1);
		}
		t = new_token(src, tOperator, op);
		vector_push_back(tokens, t);
		src++;
	}

	t = new_token(NULL, tEOF, 0);
	vector_push_back(tokens, t);

	return tokens;
}
