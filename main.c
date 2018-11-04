#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

#include "vector.h"
#include "token.h"

// node type
enum {
	nNumber,
	nOperator,
};

typedef struct node_t {
	int type;	// node type
	struct node_t *lhs; // left
	struct node_t *rhs; // right
	int val;	// number
} node_t;

int ppos = 0; // parse pos

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

node_t* parse_expr(vector_t *token);

node_t* parse_term(vector_t *token){
	token_t *t = vector_get(token, ppos);
	if(t->type == tNumber){
		ppos++;
		return new_number(t->val);
	}
	if(t->type == tTermStart){
		ppos++;
		node_t *term = parse_expr(token);
		t = vector_get(token, ppos);
		if(t->type != tTermEnd){
			fprintf(stderr, "\')\' not found.\n");
			exit(1);
		}
		ppos++;
		return term;
	}
	fprintf(stderr, "unknown token(%d): %s\n", t->type, t->str);
	exit(1);
}

node_t* parse_mul_div(vector_t *token){
	node_t *lhs = parse_term(token);
	token_t *t = vector_get(token, ppos);
	if(t->type == tEOF || t->type == tTermEnd || t->type == tExprEnd) return lhs;
	if(t->type != tOperator){
		fprintf(stderr, "unknown token: %s\n", t->str);
		exit(1);
	}

	if(t->val == oMul || t->val == oDiv){
		ppos++;
		return new_expr(nOperator+t->val, lhs, parse_mul_div(token));
	}

	return lhs;
}

node_t* parse_expr(vector_t *token){
	node_t *lhs = parse_mul_div(token);
	token_t *t = vector_get(token, ppos);
	switch(t->type){
		case tEOF:
		case tTermEnd:
			return lhs;
		case tExprEnd:
			ppos++;
			return lhs;
		default:
			break;
	}

	if(t->type != tOperator){
		fprintf(stderr, "unknown token: %s\n", t->str);
		exit(1);
	}

	if(t->val == oAdd || t->val == oSub){
		ppos++;
		return new_expr(nOperator+t->val, lhs, parse_expr(token));
	}else{
		fprintf(stderr, "unknown operator: %s\n", t->str);
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

long get_file_size(const char *fname){
	struct stat st;
	if(stat(fname, &st) != 0){
		fprintf(stderr, "stat error\n");
		exit(1);
	}
	return st.st_size;
}

char* load_file(const char *fname){
	FILE *fp = fopen(fname, "r");
	if(fp == NULL){
		fprintf(stderr, "cannot open file: \"%s\"\n", fname);
		exit(1);
	}

	long fsize = get_file_size(fname);
	char *src = malloc(fsize);

	fread(src, 1, fsize, fp);

	fclose(fp);
	return src;
}

int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "usage> scc <src>\n");
		return 1;
	}

	char* src = load_file(argv[1]);
	vector_t *tokens = tokenize(src);

	print_token(tokens);

	node_t *expr;
	for(int i=1;;i++){
		fprintf(stderr, "expr %d\n", i);
		expr = parse_expr(tokens);
		if(ppos == tokens->size-1) break;
	}
	print_node(0, expr);

	// start asm
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen_x86(expr);

	printf("\tret\n");

	return 0;
}
