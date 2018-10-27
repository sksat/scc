#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

	if(argc != 2){
		fprintf(stderr, "usage> scc <return value>\n");
	}

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("\tmov eax, %d\n", atoi(argv[1]));
	printf("\tret");

	return 0;
}
