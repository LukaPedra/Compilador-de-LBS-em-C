#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 50

/* ------------------------ PROTOTIPOS -----------------------
                                                           
    Funções protipos que serão utilizadas durante o código   
                                                           
-------------------------------------------------------------*/

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, unsigned char code[], funcp *entry);

static void error (const char *msg, int line);

int lbs_to_asm_func(int funcLabel);

int lbs_to_asm_end();

int lbs_to_asm_ret(char var, int idx);

int lbs_to_asm_zret(char var0, char var1, int idx0, int idx1, int funcLabel);

int lbs_to_asm_call(char var0, int idx0, int fx, char var1, int idx1);

int lbs_to_asm_opr(char var0, int idx0, char var1, int idx1, char op, char var2, int idx2);

/* ---------------------- VARIAVEIS GLOBAIS ------------------ */

unsigned char func_count = 0;

/* ---------------------- COMMANDOS ASSEMBLY EM HEXA ------------------ */
static unsigned char code_func[11] = {0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20, 0x89, 0x7d, 0xe4};
static unsigned char code_end[2] = {0xc9, 0xc3};
static unsigned char code_ret_var[];


/* -------------------- Função principal GERA_CODIGO ---------------------- */

void gera_codigo(FILE *f, unsigned char code[], funcp *entry){

	int c;				// CARACTER DO ARQUIVO
	int line_count = 1;	// VARIAVEL QUE ARMAZENA LINHA ATUAL

	if (code == NULL){
		error("Vetor código nulo",0);
	}

	func_count = 0;

	while((c = fgetc(f)) != EOF){

		switch(c){

			case 'f': { 

				/* --------- LEITURA CASO F -------------

				caso f 	=== function

				----------------------------------------*/

				char c0;

				if (fscanf(f, "unction%c", &c0) != 1){
					error("Comando inválido", line_count);
				}

				//printf("function\n");
				lbs_to_asm_func(func_count);
				func_count++;
				break;
			}

			case 'e': {

				/* --------- LEITURA CASO E -------------

				caso e 	=== end

				----------------------------------------*/

				char c0;

				if (fscanf(f, "nd%c", &c0) != 1){
					error("Comando inválido", line_count);
				}

				//printf("end\n");
				lbs_to_asm_end();
				break;
			}

			case 'r': {

				/* --------- LEITURA CASO R -------------

				caso r 	=== ret varpc

				----------------------------------------*/

				int idx0;
				char var0;

				if (fscanf(f, "et %c%d", &var0, &idx0) != 2){
					error("Comando inválido", line_count);
				}

				//printf("ret %c%d\n",var0 ,idx0);
				lbs_to_asm_ret(var0,idx0);
				break;
			}

			case 'z': {

				/* --------- LEITURA CASO Z -------------

				caso z 	=== zret varpc varpc

				----------------------------------------*/

				int idx0, idx1;
				char var0, var1;

				if (fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4){
					error("Comando inválido", line_count);
				}

				//printf("zret %c%d %c%d\n",var0 ,idx0, var1, idx1);
				lbs_to_asm_zret(var0, var1, idx0, idx1, func_count);
				func_count++;
				break;
			}

			case 'v': {

				/* --------- LEITURA CASO V -------------

				caso v 	=== var num

				BIFURCAÇÕES

				caso c === call

				caso op === operações

				----------------------------------------*/

				char var0 = c, c0;
				int idx0;

				if (fscanf(f, "%d = %c", &idx0, &c0) != 2){
					error("Comando inválido", line_count);
				}

				if ((c0 == 'c')){

					/* --------- LEITURA CASO C -------------

					caso c === call num varpc

					----------------------------------------*/

					int fx, idx1;
					char var1;

					if (fscanf(f, "all %d %c%d",&fx, &var1, &idx1) != 3){
						error("Comando inválido", line_count);
					}

					//printf("%c%d = call %d %c%d\n",var0 ,idx0, fx, var1, idx1);
					lbs_to_asm_call(var0, idx0, fx, var1, idx1);

				}

				else {

					/* --------- LEITURA CASO OP -------------

					caso op === varpc op varpc

					----------------------------------------*/

					int idx1, idx2;
					char var1 = c0, var2, op;

					if (fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4){
						error("Comando inválido", line_count);
					}

					//printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
					lbs_to_asm_opr(var0, idx0, var1, idx1, op, var2, idx2);

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

	/* ------- CASO END ------

	leave			   === LEAVE 
	ret				   === RETORNO

	--------------------------*/

	printf("leave\n");
	printf("ret\n");
	puts("");
	return 0;
}

int lbs_to_asm_func(int funcLabel){ // COMO A LINGUAGEM RECEBE NO MÁXIMO 5 VARIAVEIS, PRECISAMOS DE 20 BYTES, PARA COMPLETAR O MULTIPLO DE 16, ALOCAMOS 32 NA PILHA

	/* ------- CASO FUNCTION ------

	pushq %rbp			  ===  INICIALIZANDO A PILHA
	movq %rsp, %rbp		  ===  INICIALIZANDO A PILHA
	subq $32, %rsp		  ===  ABRINDO OS 32 ESPAÇOS PARA ARMAZENAR OS 20 BYTES POSSÍVEIS
	movl %edi, -28(%rbp)  ===  SALVANDO O VALOR DE EDI NA POSIÇÃO 28 PARA PODER ACESSAR ELE DEPOIS

	TRADUÇÃO PARA HEX:
	0:   55                      push   %rbp
    1:   48 89 e5                mov    %rsp,%rbp
    4:   48 83 ec 20             sub    $0x20,%rsp
    8:   89 7d e4                mov    %edi,-0x1c(%rbp)
	--------------------------*/

	printf("%d:\n",funcLabel);
	printf("pushq %%rbp\n");
	printf("movq %%rsp, %%rbp\n");
	printf("subq $32, %%rsp\n");
	printf("movl %%edi, -28(%%rbp)\n");
	puts("");
	return 0;

}

int lbs_to_asm_ret(char var0, int idx0){

	/* ------- CASO RETORNO INCONDICIONAL ------

	POSSÍVEIS CASOS

	var0 :== constante($) - variavel(V) - parametro(P)

	CASO $ 					=== CONSTANTE

	movl $constante, %eax 	=== SALVANDO A CONSTANTE NO VALOR DE RETORNO EAX

	CASO V 					=== VARIAVEL

	ACESSAR MEMORIA NA PILHA PARA VARIAVEL USAMOS -4 * (IDX0 + 1), POIS IDX0 ARMAZENA O NUMERO DA VARIAVEL, SEJA v0 OU v1 ...
	movl -x(%rbp), %eax		=== COPIAMOS O VALOR DOS BYTES DA PILHA PARA EAX

	CASO P					=== PARAMETRO

	ACESSAR A MEMORIA PARA RECUPERAR EDI QUE FOI SALVO NA -28 DA PILHA
	movl -28(%rbp), %eax	=== COPIAMOS O VALOR DE EDI PARA EAX

	------------------------------------------*/

	switch(var0){

		case '$': {
			printf("movl $%d, %%eax\n",idx0);
			break;
		}

		case 'v': {
			int access_pilha = (-4*(idx0+1));
			printf("movl %d(%%rbp), %%eax\n",access_pilha);
			puts("");
			break;
		}

		case 'p': {
			printf("movl -28(%%rbp), %%eax\n");
			puts("");
			break;
		}
		default: return -1;  //error


	}
	return 0;
}

int lbs_to_asm_opr(char var0, int idx0, char var1, int idx1, char op, char var2, int idx2){

	/* ------- CASO OPERAÇÃO -----------------

	POSSIVEIS CASOS

	var1 :== constante($) - variavel(V) - parametro(P)
	op :== '+' - '-' - '*'
	var2 :== constante($) - variavel(V) - parametro(P)

	CASO VAR1 $

	movl $constante, %r10d				=== SALVANDO A CONSTANTE EM R10D

	CASO VAR1 V

	movl -x(%rbp) , %r10d				=== SALVANDO -x(%rbp) EM R10D (MESMO CONCEITO PARA ACESSAR A PILHA)

	CASO VAR1 P

	movl <p0>, %r10d					=== SALVANDO PARAMETRO EM R10D

	APLICAREMOS O MESMO CONCEITO PORÉM AGORA SOMANDO COM OS OPERADORES COM O VAR2

	CASO OP +

	addl var2, %r10d					=== SOMANDO O VAR2 EM R10D

	CASO OP -

	subl var2, %r10d					=== SUBTRAINDO O VAR2 EM R10D

	CASO OP *

	imull var2, %r10d					=== MULTIPLICANDO O VAR2 EM R10D

	------------------------------------------*/

	switch(var1){
		
		case '$': {
			printf("movl $%d, %%r10d\n", idx1);

			break;
		}

		case 'v': {
			int access_pilha = -4*(idx1+1);
			printf("movl %d(%%rbp), %%r10d\n", access_pilha);

			break;
		}

		case 'p': {
			printf("movl %%edi, %%r10d\n");

			break;
		}
		default: return -1; //error
	}

	switch(op){

		case '+': {

			switch(var2){
				
				case '$': {
					printf("addl $%d, %%r10d\n", idx2);

					break;
				}

				case 'v': {
					int access_pilha = -4*(idx2+1);
					printf("addl %d(%%rbp), %%r10d\n", access_pilha);

					break;
				}

				case 'p': {
					printf("addl %%edi, %%r10d\n");

					break;
				}
				default: return -1; //error
			}

			break;
		}

		case '-': {

			switch(var2){
				
				case '$': {
					printf("subl $%d, %%r10d\n", idx2);

					break;
				}

				case 'v': {
					int access_pilha = -4*(idx2+1);
					printf("subl %d(%%rbp), %%r10d\n", access_pilha);

					break;
				}

				case 'p': {
					printf("subl %%edi, %%r10d\n");

					break;
				}
				default: return -1; //error
			}

			break;
		}

		case '*': {

			switch(var2){
				
				case '$': {
					printf("imull $%d, %%r10d\n", idx2);

					break;
				}

				case 'v': {
					int access_pilha = -4*(idx2+1);
					printf("imull %d(%%rbp), %%r10d\n", access_pilha);

					break;
				}

				case 'p': {
					printf("imull %%edi, %%r10d\n");

					break;
				}
				default: return -1; //error
			}

			break;
		}
		default: return -1; //error
	}

	int access_pilha = -4*(idx0+1);
	printf("movl %%r10d, %d(%%rbp)\n", access_pilha);
	puts("");

	return 0;
}

int lbs_to_asm_call(char var0, int idx0, int fx, char var1, int idx1){

	/* ----------------- CASO CALL ----------------

	POSSIVEIS CASOS

	var = call num varpc

	var1 :== constante($) - variavel(V) - parametro(P)

	CASO VAR1 $

	movl $constante, %edi			=== SALVANDO A CONSTANTE EM EDI
	
	CASO VAR1 V

	movl -x(%rbp) , %edi			=== SALVANDO A VARIAVEL DA PILHA EM EDI (MESMA LÓGICA PARA ACESSAR A PILHA)

	CASO VAR1 P

	movl <p0>, %edi					=== SALVANDO O PARAMETRO EM EDI


	CALL

	call <fx>
	movl %eax, -4(%rbp)				=== SALVAR O RETORNO DA FUNÇÃO NO V0 (MESMA LÓGICA PARA ACESSAR A PILHA)

	---------------------------------------------*/

	switch(var1){

		case '$': {
			printf("movl $%d, %%edi\n",idx1);
			puts("");

			break;
		}
		
		case 'v': {
			int access_pilha = -4*(idx1+1);
			printf("movl %d(%%rbp), %%edi\n", access_pilha);
			puts("");

			break;
		}

		case 'p': {
			//printf("movl <p%d>, %%edi\n",idx1);
			puts("");
			
			break;
		}
		default: return -1; //error
	}

	int access_pilha = -4*(idx0+1);
	printf("call %d\n",fx);
	printf("movl %%eax, %d(%%rbp)\n",access_pilha);
	puts("");

	return 0;
}

int lbs_to_asm_zret(char var0, char var1, int idx0, int idx1, int funcLabel){

	/* ----------------- CASO ZRET (RETORNO CONDICIONAL) ----------------

	zret varpc varpc

	var0 :== constante($) - variavel(V) - parametro(P)

	CASO VAR0 $

	movl $constante, %r10d				=== SALVANDO A CONSTANTE EM R10D PARA COMPARAR DEPOIS

	CASO VAR0 V

	movl -x(%rbp), %r10d				=== SALVANDO A VARIAVEL EM R10D PARA COMPARAR DEPOIS

	CASO VAR0 P

	movl <p0>, %r10d					=== SALVANDO O PARAMETRO EM R10D PARA COMPARAR DEPOIS


	CONTINUAÇÃO ZRET


	cmpl $0, %r10d
	movl <retorno>, %eax
	jne <fx>
	leave
	ret

	---------------------------------------------------------------------*/

	switch(var0){

		case '$': {
			printf("movl $%d, %%r10d\n", idx0);
			puts("");

			break;
		}

		case 'v': {
			int access_pilha = -4*(idx0+1);
			printf("movl %d(%%rbp), %%r10d\n", access_pilha);
			puts("");

			break;
		}

		case 'p': {
			printf("movl %%edi, %%r10d\n");
			puts("");

			break;
		}
		default: return -1; //error
	}

	printf("cmpl $0, %%r10d\n");
	printf("movl %c%d, %%eax\n",var1,idx1);
	printf("jne %d\n",func_count);
	lbs_to_asm_end();
	printf("%d:\n",funcLabel);


	return 0;
}