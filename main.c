#include <stdio.h>
#include <stdlib.h>

#define bool _Bool
#define true 1
#define false 0

enum {
	tNumber,
	tOperator,
	tEOF,
};

typedef struct {
	char *str;	// token string
	int type;	// token type
	int val;	// number
} token_t;

bool is_space(char c){
	return (c==' ' || c=='\t' || c=='\n');
}

token_t* tokenize(char *src){
	static token_t tokens[100];

	int i = 0;
	while(*src != '\0'){
		// space
		if(is_space(*src)){
			src++;
			continue;
		}

		// number
		if('0' <= *src && *src <= '9'){
			tokens[i].str = src;
			tokens[i].type= tNumber;
			tokens[i].val = strtol(src, &src, 10);
			i++;
			continue;
		}

		// operator
		if(*src == '+' || *src == '-'){
			tokens[i].str = src;
			tokens[i].type= tOperator;
			src++;
			i++;
			continue;
		}

		fprintf(stderr, "tokenize error: %s\n", src);
		exit(1);
	}
	tokens[i].type = tEOF;

	return tokens;
}

int main(int argc, char **argv){

	// "1+2"のような簡単な式を処理する

	if(argc != 2){
		fprintf(stderr, "usage> scc <expr>\n");
		return 1;
	}

	token_t *tokens = tokenize(argv[1]);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 最初の数字

	printf("\tmov eax, %d\n", tokens[0].val);

	int i = 1;
	while(tokens[i].type != tEOF){
		if(tokens[i].type == tOperator){
			printf("\t");
			switch(*tokens[i].str){
				case '+': printf("add "); break;
				case '-': printf("sub "); break;
				default:
					fprintf(stderr, "unknown operator: %c", *tokens[i].str);
					break;
			}
			i++;
			printf("eax, %d\n", tokens[i].val);
			i++;
		}
	}

	printf("\tret");

	return 0;
}
