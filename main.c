#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

	// "1+2"のような簡単な式を処理する

	if(argc != 2){
		fprintf(stderr, "usage> scc <expr>\n");
		return 1;
	}

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	char *p;
	// 最初の数字
	printf("\tmov eax, %ld\n", strtol(argv[1], &p, 10));

	while(*p != '\0'){
		if(*p == '+'){
			p++; // 読み飛ばす
			printf("\tadd eax, %ld\n", strtol(p, &p, 10));
			continue;
		}
		if(*p == '-'){
			p++;
			printf("\tsub eax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		// よくわからない文字
		fprintf(stderr, "unexpected char: %c", *p);
		return 1;
	}

	printf("\tret");

	return 0;
}
