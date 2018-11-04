#include <stdio.h>
#include <stdlib.h>

#define bool _Bool
#define true 1
#define false 0

// token type
enum {
	tNumber,
	tTermStart, tTermEnd,
	tOperator,
	tEOF,
};

// operator type
enum {
	oAdd = 0,
	oSub,
	oMul,
	oDiv,
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

		if(*src == '(' || *src == ')'){
			tokens[i].str = src;
			if(*src == '(')
				tokens[i].type = tTermStart;
			else
				tokens[i].type = tTermEnd;
			src++;
			i++;
			continue;
		}

		// operator
		switch(*src){
			case '+':
				tokens[i].val = oAdd;
				break;
			case '-':
				tokens[i].val = oSub;
				break;
			case '*':
				tokens[i].val = oMul;
				break;
			case '/':
				tokens[i].val = oDiv;
				break;
			default:
				fprintf(stderr, "tokenize error: %s\n", src);
				exit(1);
		}

		tokens[i].str = src;
		tokens[i].type = tOperator;
		src++;
		i++;
	}
	tokens[i].type = tEOF;

	return tokens;
}

node_t* new_number(int val){
	node_t *num = malloc(sizeof(node_t));
	num->type = nNumber;
	num->val = val;
	return num;
}

node_t* new_expr(int type, node_t *lhs, node_t *rhs){
	node_t *expr = malloc(sizeof(node_t));
	expr->type = type;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

node_t* parse_expr(token_t *token);

node_t* parse_term(token_t *token){
	if(token[ppos].type == tNumber)
		return new_number(token[ppos++].val);
	if(token[ppos].type == tTermStart){
		ppos++;
		node_t *term = parse_expr(token);
		if(token[ppos].type != tTermEnd){
			fprintf(stderr, "\')\' not found.\n");
			exit(1);
		}
		ppos++;
		return term;
	}
	fprintf(stderr, "unknown token: %s\n", token[ppos].str);
	exit(1);
}

node_t* parse_mul_div(token_t *token){
	node_t *lhs = parse_term(token);
	if(token[ppos].type == tEOF || token[ppos].type == tTermEnd) return lhs;
	if(token[ppos].type != tOperator){
		fprintf(stderr, "unknown token: %s\n", token[ppos].str);
		exit(1);
	}

	if(token[ppos].val == oMul || token[ppos].val == oDiv){
		ppos++;
		return new_expr(nOperator+token[ppos-1].val, lhs, parse_mul_div(token));
	}

	return lhs;
}

node_t* parse_expr(token_t *token){
	node_t *lhs = parse_mul_div(token);
	if(token[ppos].type == tEOF || token[ppos].type == tTermEnd) return lhs;
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
		case oMul:
			return "*";
		case oDiv:
			return "/";
		default:
			return "unknown";
	}
}

void print_node(int n, node_t *node){
	for(int i=0;i<n;i++) fprintf(stderr, "  ");
	if(node->type == nNumber){
		fprintf(stderr, "number(%d)\n", node->val);
		return;
	}else if(node->type >= nOperator)
		fprintf(stderr, "operator(%s)\n", get_op_str(node->type-nOperator));
	else
		fprintf(stderr, "unknown(%d)\n", node->val);
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
			case oMul:
				printf("\timul eax, ecx\n");
				break;
			case oDiv:
				printf("\tcdq\n"); // EAX->EDX:EAX
				printf("\tidiv eax, ecx\n");
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
