#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 50

typedef int(*funcp)(int x);
void gera_codigo(FILE *f, unsigned char code[], funcp *entry){
	unsigned char c;
	char *comand;
	char *line = NULL;
	size_t linesize = 0;
	int line_Count = 0;

}

int main(void){
	FILE *fp;
	fp = fopen("LBS constant.txt","r");
	char *line_buf = NULL; //String com o conteúdo de uma linha
	size_t line_buf_size = 0;
	int line_Count = 0;
	ssize_t line_size;
	
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
	puts("");
}