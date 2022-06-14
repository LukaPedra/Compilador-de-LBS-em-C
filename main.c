#include <stdio.h>
#include "gera_codigo.h"

int main(void){
	FILE *fp;
	fp = fopen("LBS constant.txt","r");
	unsigned char teste[2048];
	funcp *entry = NULL;
	gera_codigo(fp, teste, entry);
}