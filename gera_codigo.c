#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 50
#define MEMORY_FOR_CODE 2048  // 2048 - 32 bytes da função LBS * max 50 linhas = 1600, arrendondamos para 2048



/* ------------------------ PROTOTIPOS -----------------------
                                                           
    Funções protipos que serão utilizadas durante o código   
                                                           
-------------------------------------------------------------*/

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, unsigned char code[], funcp *entry);

static void error (const char *msg, int line);

int lbs_to_asm_func();

int lbs_to_asm_end();

int lbs_to_asm_ret();

int lbs_to_asm_zret();

int lbs_to_asm_call();

int lbs_to_asm_opr();

/* ---------------------- VARIAVEIS GLOBAIS ------------------ */

unsigned char func_cont = 0; //Contar o número de funções

unsigned char curr_line = 0; //Qual é a linha atual

unsigned char *aux_code = NULL;

/* -------------------- Função principal GERA_CODIGO ---------------------- */

void gera_codigo(FILE *f, unsigned char code[], funcp *entry){

	int c;				// Caracter do arquivo
	int line_count = 1;	// Variavél que armazna linha atual
	int ret;			// Variavél que armazena valor de retorno

	if (code == NULL){
		error("Vetor código nulo",0);
	}

	func_cont = 0;


	aux_code = (unsigned char*)malloc(MEMORY_FOR_CODE);

	if (aux_code == NULL){
		error("Falha ao alocar memória",0);
	}


	while((c = fgetc(f)) != EOF){ // LENDO O ARQUIVO ATÉ O FIM

		switch(c){ 	// SWITCH CASE PARA ACHAR A FUNÇÃO CORRESPONDENTE

			case 'f': { // FUNÇÃO
				char c0;
				if (fscanf(f, "unction%c", &c0) != 1){
					error("Comando inválido", line_count);
				}
				printf("function\n");
				func_cont++;
				break;
			}

			case 'e': { // END
				char c0;
				if (fscanf(f, "nd%c", &c0) != 1){
					error("Comando inválido", line_count);
				}
				//printf("end\n");
				lbs_to_asm_end();
				break;
			}

			case 'r': {	// RETORNO INCONDICIONAL
				int idx0;
				char var0;
				if (fscanf(f, "et %c%d", &var0, &idx0) != 2){
					error("Comando inválido", line_count);
				}
				printf("ret %c%d\n",var0 ,idx0);
				break;
			}

			case 'z': {	// RETORNO CONDICIONAL
				int idx0, idx1;
				char var0, var1;
				if (fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4){
					error("Comando inválido", line_count);
				}
				printf("zret %c%d %c%d\n",var0 ,idx0, var1, idx1);
				break;
			}

			case 'v': {	// ATRIBUIÇÃO DE VARIAVEL
				char var0 = c, c0;
				int idx0;
				if (fscanf(f, "%d = %c", &idx0, &c0) != 2){
					error("Comando inválido", line_count);
				}
				if ((c0 == 'c')){ // CALL
					int fx, idx1;
					char var1;
					if (fscanf(f, "all %d %c%d",&fx, &var1, &idx1) != 3){
						error("Comando inválido", line_count);
					}
					printf("%c%d = call %d %c%d\n",var0 ,idx0, fx, var1, idx1);

				}

				else {			// OPERAÇÃO ARITMETICA
					int idx1, idx2;
					char var1 = c0, var2, op;
					if (fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4){
						error("Comando inválido", line_count);
					}
					printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);

				}

				break;
			}
			default: error("Comando desconhecido",line_count);
		}
		line_count++;
		fscanf(f, " ");

	}
	return;
}

static void error (const char *msg, int line) {
    fprintf(stderr, "Erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}


/* ------------------ TRADUÇÃO LBS TO ASM --------------------- */

int lbs_to_asm_end(){
	printf("leave\n");
	printf("ret\n");
	return 0;
}