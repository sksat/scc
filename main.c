#include <stdio.h>

int main(int argc, char **argv){

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("\tmov eax, 0\n");
	printf("\tret");

	return 0;
}
