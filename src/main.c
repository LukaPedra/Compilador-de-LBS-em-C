#include <stdio.h>
#include "gera_codigo.h"

int main(int argc, char *argv[]){
	FILE *fp;
	unsigned char code[300];
	funcp funcLBS;
	int res;

	fp = fopen("LBS_return.txt","r");

	funcLBS = gera_codigo(fp,code,funcLBS);
	printf("AAAA\n");
	if (funcLBS == NULL){
		printf("Erro na geração\n");
	}
	fclose(fp);
	res = (*funcLBS)(3);
	printf("%d", res);
	return 0;
}