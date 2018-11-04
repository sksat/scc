#include <stdio.h>
#include <stdlib.h>

#define bool _Bool
#define true 1
#define false 0

// token type
enum {
	tNumber,
	tOperator,
	tEOF,
};

// operator type
enum {
	oAdd,
	oSub,
};

// node type
enum {
	nNumber,
	nOperator,
};

typedef struct {
	char *str;	// token string
	int type;	// token type
	int val;	// number
} token_t;

typedef struct node_t {
	int type;	// node type
	struct node_t *lhs; // left
	struct node_t *rhs; // right
	int val;	// number
} node_t;

int ppos = 0; // parse pos

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
		switch(*src){
			case '+':
				tokens[i].val = oAdd;
				goto op;
			case '-':
				tokens[i].val = oSub;
				goto op;
			default:
				fprintf(stderr, "tokenize error: %s\n", src);
				exit(1);
		}
op:
		tokens[i].str = src;
		tokens[i].type = tOperator;
		src++;
		i++;
	}
	tokens[i].type = tEOF;

	return tokens;
}

node_t* new_expr(int type, node_t *lhs, node_t *rhs){
	node_t *expr = malloc(sizeof(node_t));
	expr->type = type;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

node_t* get_number(token_t *token){
	if(token[ppos].type != tNumber){
		fprintf(stderr, "number expected, but got %s", token[ppos].str);
	}
	node_t *num = malloc(sizeof(node_t));
	num->type = nNumber;
	num->val  = token[ppos].val;
	ppos++;
	return num;
}

node_t* parse_expr(token_t *token){
	node_t *lhs = get_number(token);
	if(token[ppos].type == tEOF) return lhs;
	if(token[ppos].type != tOperator){
		fprintf(stderr, "unknown token: %s\n", token[ppos].str);
		exit(1);
	}

	if(token[ppos].val == oAdd || token[ppos].val == oSub){
		ppos++;
		return new_expr(nOperator+token[ppos-1].val, lhs, parse_expr(token));
	}else{
		fprintf(stderr, "unknown operator: %s\n", token[ppos].str);
		exit(1);
	}
}

const char* get_op_str(int type){
	switch(type){
		case oAdd:
			return "+";
		case oSub:
			return "-";
		default:
			return "unknwon";
	}
}

void print_node(int n, node_t *node){
	for(int i=0;i<n;i++) fprintf(stderr, "  ");
	switch(node->type){
		case nNumber:
			fprintf(stderr, "number(%d)\n", node->val);
			return;
		case nOperator:
			fprintf(stderr, "operator(%s)\n", get_op_str(node->val));
			break;
		default:
			fprintf(stderr, "unknown\n");
			return;
	}
	print_node(n+1, node->lhs);
	print_node(n+1, node->rhs);
}

void gen_x86(node_t *node){
	// とりあえずすべてをeaxに詰める
	if(node->type == nNumber){
		printf("\tmov eax, %d\n", node->val);
		return;
	}
	if(nOperator <= node->type){
		int op = node->type - nOperator;
		gen_x86(node->lhs);
		printf("\tpush eax\n");
		gen_x86(node->rhs);
		printf("\tpush eax\n");

		printf("\tpop ecx\n");
		printf("\tpop eax\n");
		switch(op){
			case oAdd:
				printf("\tadd eax, ecx\n");
				break;
			case oSub:
				printf("\tsub eax, ecx\n");
				break;
			default:
				fprintf(stderr, "unknown operator\n");
				exit(1);
		}
	}
}

int main(int argc, char **argv){

	// "1+2"のような簡単な式を処理する

	if(argc != 2){
		fprintf(stderr, "usage> scc <expr>\n");
		return 1;
	}

	token_t *tokens = tokenize(argv[1]);
	node_t *expr = parse_expr(tokens);
	print_node(0, expr);

	// start asm
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen_x86(expr);

	printf("\tret\n");

	return 0;
}
