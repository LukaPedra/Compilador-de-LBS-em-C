#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 50
#define MEMORY_FOR_CODE 2048  // 2048 = 32 bytes da função LBS * max 50 linhas = 1600, arrendondamos para 2048






/* ------------------------ PROTOTIPOS -----------------------
                                                           
    Funções protipos que serão utilizadas durante o código   
                                                           
-------------------------------------------------------------*/

typedef int(*funcp)(int x);
void gera_codigo(FILE *f, unsigned char code[], funcp *entry);




/* ---------------------- VARIAVEIS GLOBAIS ------------------ */

unsigned char func_cont = 0;

unsigned char curr_line = 0;

unsigned char aux_code = NULL;



int main(void){
	FILE *fp;
	fp = fopen("LBS constant.txt","r");
	char *line_buf = NULL; //String com o conteúdo de uma linha
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
	puts("");
}

/* -------------------- Função principal GERA_CODIGO ---------------------- */

void gera_codigo(FILE *f, unsigned char code[], funcp *entry){

	int c;				// Caracter do arquivo
	int line_count = 1;	// Variavél que armazna linha atual
	int ret;			// Variavél que armazena valor de retorno
	char *command;
	char *line = NULL;
	size_t linesize = 0;

	if (code == NULL){
		puts("Falha ao alocar memória");
		return -1;
	}

	func_cont = 0;


	aux_code = (unsigned char*)malloc(MEMORY_FOR_CODE);

	if (aux_code == NULL){
		puts("Falha ao alocar memória");
		return -1;
	}


	while((c = fgetc(f)) != EOF){ // LENDO O ARQUIVO ATÉ O FIM

		switch(c){ 	// SWITCH CASE PARA ACHAR A FUNÇÃO CORRESPONDENTE

			case 'f': { // FUNÇÃO

				break;
			}

			case 'e': { // END

				break;
			}

			case 'r': {	// RETORNO INCONDICIONAL

				break;
			}

			case 'z': {	// RETORNO CONDICIONAL

				break;
			}

			case 'v': {	// ATRIBUIÇÃO DE VARIAVEL

				if (c0 = 'c'){ // CALL

				}

				else {			// OPERAÇÃO ARITMETICA

				}

				break;
			}
			default: error("Comando desconhecido",line_count);
		}
		line_count++;

	}
	return;
}
