#include <stdio.h>
#include "gera_codigo.h"



int main(void){
	FILE *fp;
	fp = fopen("LBS constant.txt","r");
	/*char *line_buf = NULL; //String com o conteúdo de uma linha
	size_t line_buf_size = 0;
	int line_Count = 0;
	size_t line_size;
	
	if(!fp) {
		printf("Ocorreu um erro ao abrir o arquivo");
		exit(1);
	}
	line_size = getline(&line_buf, &line_buf_size, fp);
	while (line_size >= 0){
		line_Count++;
		printf("line[%06d]: chars=%06zd, buf size=%06zu, contents:\t %s", line_Count, line_size, line_buf_size, line_buf);

		line_size = getline(&line_buf, &line_buf_size, fp); //Próxima linha 
	}
	//free(line_buf);
	line_buf = NULL;
	fclose(fp);
	puts("");*/
	unsigned char teste[2048];
	funcp *entry = NULL;
	gera_codigo(fp, teste, entry);
}