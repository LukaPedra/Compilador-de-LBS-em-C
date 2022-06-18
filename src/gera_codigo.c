#include <stdio.h>
#include <stdlib.h>

#define LINHAS 50
#define f_offset(f_id) (func_pos[f_id] - current_byte - 5) //COPIEI MUDAR

/* ------------------------ PROTOTIPOS -----------------------
                                                           
    Funções protipos que serão utilizadas durante o código   
                                                           
-------------------------------------------------------------*/

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, unsigned char code[], funcp *entry);

static void error (const char *msg, int line);

void lbs_to_asm_func(int funcLabel);

void lbs_to_asm_end();

void lbs_to_asm_ret(char var, int idx);

void lbs_to_asm_zret(char var0, char var1, int idx0, int idx1);

void lbs_to_asm_call(char var0, int idx0, int fx, char var1, int idx1);

void lbs_to_asm_opr(char var0, int idx0, char var1, int idx1, char op, char var2, int idx2);

void num_lendian ( unsigned char *commands, size_t pos, size_t bytes, int number );

void add_commands(unsigned char *commands, size_t bytes);

/* ---------------------- VARIAVEIS GLOBAIS ------------------ */

int view_x86_sintax = 0; // variavel global utilizada para ativar o modo print, caso queira ver o código em assembly, isso foi utilizado para auxiliar na hora da construção do código
unsigned char func_count = 0;
unsigned int current_byte = 0;
unsigned char *p_code = NULL;
unsigned int func_pos[LINHAS] = {};

/* ---------------------- COMMANDOS ASSEMBLY EM HEXA ------------------ */

static unsigned char code_func[11] = {0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20, 0x89, 0x7d, 0xe4};
static unsigned char code_end[2] = {0xc9, 0xc3};
static unsigned char code_ret_cst[5] = {0xb8, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_ret_par[3] = {0x8b, 0x45, 0xe4};
static unsigned char code_ret_var[3] = {0x8b, 0x45, 0x00};
static unsigned char code_mov_cst_reg[6] = {0x41, 0xba, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_mov_var_reg[4] = {0x44, 0x8b, 0x55, 0x00};
static unsigned char code_mov_par_reg[4] = {0x44, 0x8b, 0x55, 0xe4};
static unsigned char code_opr_add_cst[7] = {0x41, 0x83, 0xc2, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_opr_add_var[4] = {0x44, 0x03, 0x55, 0x00};
static unsigned char code_opr_add_par[4] = {0x44, 0x03, 0x55, 0xe4};
static unsigned char code_opr_sub_cst[7] = {0x41, 0x83, 0xea, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_opr_sub_var[4] = {0x44, 0x2b, 0x55, 0x00};
static unsigned char code_opr_sub_par[4] = {0x44, 0x2b, 0x55, 0xe4};
static unsigned char code_opr_mult_cst[7] = {0x45, 0x6b, 0xd2, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_opr_mult_var[5] = {0x44, 0x0f, 0xaf, 0x55, 0x00};
static unsigned char code_opr_mult_par[5] = {0x44, 0x0f, 0xaf, 0x55, 0xe4};
static unsigned char code_mov_reg_var[4] = {0x44, 0x89, 0x55, 0x00};
static unsigned char code_mov_cst_par[5] = {0xbf, 0x00, 0x00, 0x00, 0x00};
static unsigned char code_mov_var_par[3] = {0x8b, 0x7d, 0x00};
static unsigned char code_mov_par_par[3] = {0x8b, 0x7d, 0xe4};
static unsigned char code_call[8] = {0xe8,0x00,0x00,0x00,0x00,0x89,0x45,0x00};
static unsigned char code_zret_cmpl[4] = {0x41, 0x83, 0xfa, 0x00};
static unsigned char code_zret_jne[2] = {0x75, 0x07};

/* -------------------- Função principal GERA_CODIGO ---------------------- */

void gera_codigo(FILE *f, unsigned char code[], funcp *entry){

	int c;				// CARACTER DO ARQUIVO
	int line_count = 1;	// VARIAVEL QUE ARMAZENA LINHA ATUAL
	current_byte = 0;	//QUAL É O BYTE ATUAL DO ARRAY DO CODE

	if (code == NULL){
		error("Vetor código nulo",0);
	}

	func_count = 0;
	code = NULL;
	p_code = (unsigned char *)malloc(1024); // 1600 pois cada o código máximo tem 50 linhas, e o máximo que podemos ter é 32 bits, então 32 x 50 = 1600.

	if(p_code == NULL) {
		error("Erro ao alocar memória",0);
	}

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
				lbs_to_asm_zret(var0, var1, idx0, idx1);
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

	code = (void*) p_code;

	*entry = (funcp) (p_code + func_pos[func_count - 1]);
}

static void error (const char *msg, int line) {
    fprintf(stderr, "Erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}


/* ------------------ TRADUÇÃO LBS TO ASM --------------------- */

void lbs_to_asm_end(){

	/* ------- CASO END ------

	leave			   === LEAVE	0xc9
	ret				   === RETORNO	0xc3
	TRADUÇÃO PARA HEX:
	0:   c9                      leaveq 
	1:   c3                      retq  

	--------------------------*/
	/*debug assembly code */
	if(view_x86_sintax == 1){
		printf("leave\n");
		printf("ret\n");
		puts("");
	}

	add_commands(code_end, sizeof(code_end));
}

void lbs_to_asm_func(int funcLabel){ // COMO A LINGUAGEM RECEBE NO MÁXIMO 5 VARIAVEIS, PRECISAMOS DE 20 BYTES, PARA COMPLETAR O MULTIPLO DE 16, ALOCAMOS 32 NA PILHA

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
	if(view_x86_sintax == 1){
		printf("%d:\n",funcLabel);
		printf("pushq %%rbp\n");
		printf("movq %%rsp, %%rbp\n");
		printf("subq $32, %%rsp\n");
		printf("movl %%edi, -28(%%rbp)\n");
		puts("");
	}
	func_pos[func_count] = current_byte;
	func_count++;
	add_commands(code_func, sizeof(code_func));
}

void lbs_to_asm_ret(char var0, int idx0){

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
			if(view_x86_sintax){
				printf("movl $%d, %%eax\n",idx0);
			}
			num_lendian(code_ret_cst, 1, 4, idx0);
			add_commands(code_ret_cst, sizeof(code_ret_cst));
			break;
		}

		case 'v': {
			int access_pilha = (-4*(idx0+1));
			if(view_x86_sintax){
				printf("movl %d(%%rbp), %%eax\n",access_pilha);
				puts("");
			}
			num_lendian(code_ret_var, 2, 1, access_pilha);
			add_commands(code_ret_var, sizeof(code_ret_var));
			break;
		}

		case 'p': {
			if(view_x86_sintax){
				printf("movl -28(%%rbp), %%eax\n");
				puts("");
			}
			add_commands(code_ret_par, sizeof(code_ret_par));
			break;
		}
		default: printf("Erro na leitura");  //error


	}
}

void lbs_to_asm_opr(char var0, int idx0, char var1, int idx1, char op, char var2, int idx2){

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
			if(view_x86_sintax){
				printf("movl $%d, %%r10d\n", idx1);
			}
			/*Assembly code*********|hex***************
			*	movl $(cst), %r10d	| 41 ba 00 00 00 00 
			*						|		^^ = cst(4 bytes)
			**********************************************/
			num_lendian(code_mov_cst_reg, 2, 4, idx1);
			add_commands(code_mov_cst_reg, sizeof(code_mov_cst_reg));
			break;
		}

		case 'v': {
			int access_pilha = -4*(idx1+1);
			if(view_x86_sintax){
				printf("movl %d(%%rbp), %%r10d\n", access_pilha);
			}
			/*Assembly code*********|hex***************
			*	movl _(%rbp), %r10d	| 44 8b 55 00 
			*		 ^access_pilha	|		   ^^ = access_pilha
			**********************************************/
			num_lendian(code_mov_var_reg, 3, 1, access_pilha);
			add_commands(code_mov_var_reg, sizeof(code_mov_var_reg));
			break;
		}

		case 'p': {
			if(view_x86_sintax){
				printf("movl -28(%%rbp), %%r10d\n");
			}
			/*Assembly code*********|hex***************
			*movl -28(%rbp), %r10d	| 44 8b 55 e4
			**********************************************/
			add_commands(code_mov_par_reg, sizeof(code_mov_par_reg));
			break;
		}
		default: printf("Erro na leitura");  //error
	}

	switch(op){

		case '+': {

			switch(var2){
				
				case '$': {
					if(view_x86_sintax){
						printf("addl $%d, %%r10d\n", idx2);
					}
					/*Assembly code*********|hex***************
					*	addl $cst, %r10d	| 41 83 c2 00 00 00 00
					*						|		   ^^ = cst(4 bytes)
					**********************************************/
					num_lendian(code_opr_add_cst, 3, 4, idx2);
					add_commands(code_opr_add_cst, sizeof(code_opr_add_cst));
					break;
				}

				case 'v': {
					int access_pilha = -4*(idx2+1);
					if(view_x86_sintax){
						printf("addl %d(%%rbp), %%r10d\n", access_pilha);
					}
					/*Assembly code*********|hex***************
					*	addl _(%rbp), %r10d	| 44 03 55 00
					*		 ^acess_pilha	|		   ^^ = acess_pilha
					**********************************************/
					num_lendian(code_opr_add_var, 3, 1, access_pilha);
					add_commands(code_opr_add_var, sizeof(code_opr_add_var));
					break;
				}

				case 'p': {
					if(view_x86_sintax){
						printf("addl -28(%%rbp), %%r10d\n");
					}
					/*Assembly code*********|hex***************
					* addl -28(%rbp), %r10d	| 44 03 55 e4 
					**********************************************/
					add_commands(code_opr_add_par, sizeof(code_opr_add_par));
					break;
				}
				default: printf("Erro na leitura");  //error
			}

			break;
		}

		case '-': {

			switch(var2){
				
				case '$': {
					if(view_x86_sintax){
						printf("subl $%d, %%r10d\n", idx2);
					}
					/*Assembly code*********|hex***************
					*	subl $(cst), %r10d	| 41 83 ea 00 00 00 00
					*						|		   ^^ = cst(4 bytes)
					**********************************************/
					num_lendian(code_opr_sub_cst, 3, 4, idx2);
					add_commands(code_opr_sub_cst, sizeof(code_opr_sub_cst));
					break;
				}

				case 'v': {
					int access_pilha = -4*(idx2+1);
					if(view_x86_sintax){
						printf("subl %d(%%rbp), %%r10d\n", access_pilha);
					}
					/*Assembly code*********|hex***************
					*	subl _(%rbp), %r10d	| 44 2b 55 00  
					*		 ^acess_pilha	|		   ^^ = access_pilha
					**********************************************/
					num_lendian(code_opr_sub_var, 3, 1, access_pilha);
					add_commands(code_opr_sub_var, sizeof(code_opr_sub_var));
					break;
				}

				case 'p': {
					if(view_x86_sintax){
						printf("subl -28(%%rbp), %%r10d\n");
					}
					/*Assembly code*********|hex***************
					*subl -28(%rbp), %r10d	| 44 2b 55 e4
					**********************************************/
					add_commands(code_opr_sub_par, sizeof(code_opr_sub_par));
					break;
				}
				default: printf("Erro na leitura");  //error
			}

			break;
		}

		case '*': {

			switch(var2){
				
				case '$': {
					if(view_x86_sintax){
						printf("imull $%d, %%r10d\n", idx2);
					}
					/*Assembly code*********|hex***************
					*	imull $(cst), %r10d	| 45 6b d2 00 00 00 00
					*						|		   ^^ = cst (4 bytes)
					**********************************************/
					num_lendian(code_opr_mult_cst, 3, 4, idx2);
					add_commands(code_opr_mult_cst, sizeof(code_opr_mult_cst));
					break;
				}

				case 'v': {
					int access_pilha = -4*(idx2+1);
					if(view_x86_sintax){
						printf("imull %d(%%rbp), %%r10d\n", access_pilha);
					}
					/*Assembly code*********|hex***************
					*	imull _(%rbp),%r10d	| 44 0f af 55 00 
					*		  ^=acess_pilha	|		   	  ^^ = access_pilha
					**********************************************/
					num_lendian(code_opr_mult_var, 4, 1, access_pilha);
					add_commands(code_opr_mult_var, sizeof(code_opr_mult_var));
					break;
				}

				case 'p': {
					if(view_x86_sintax){
						printf("imull -28(%%rbp), %%r10d\n");
					}
					/*Assembly code*********|hex***************
					*imull -28(%rbp),%r10d	| 44 0f af 55 e4
					**********************************************/
					add_commands(code_opr_mult_par, sizeof(code_opr_mult_par));
					break;
				}
				default: printf("Erro na leitura");  //error
			}

			break;
		}
		default: printf("Erro na leitura");  //error
	}
	int access_pilha = -4*(idx0+1);
	if(view_x86_sintax){
		printf("movl %%r10d, %d(%%rbp)\n", access_pilha);
		puts("");
	}
	/*Assembly code*************|hex***************
	*movl %r10d, _(%rbp)		| 44 89 55 00
	*			 ^access_pilha	| 		   ^^=access_pilha
	**********************************************/
	num_lendian(code_mov_reg_var, 3, 1, access_pilha);
	add_commands(code_mov_reg_var, sizeof(code_mov_reg_var));
}

void lbs_to_asm_call(char var0, int idx0, int fx, char var1, int idx1){

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
			if(view_x86_sintax){
				printf("movl $%d, %%edi\n",idx1);
				puts("");
			}
			/*Assembly code*************|hex***************
			*movl $(cst), %%edi		| bf 00 00 00 00  
			*						| 	 ^^=cst(4 bytes)
			**********************************************/
			num_lendian(code_mov_cst_par, 1, 4, idx1);
			add_commands(code_mov_cst_par, sizeof(code_mov_cst_par));
			break;
		}
		
		case 'v': {
			int access_pilha = -4*(idx1+1);
			if(view_x86_sintax){
				printf("movl %d(%%rbp), %%edi\n", access_pilha);
				puts("");
			}
			/*Assembly code*************|hex***************
			*movl _(%rbp), %edi			| 8b 7d 00  
			*	  ^access_pilha			| 	 	^^=access_pilha
			**********************************************/
			num_lendian(code_mov_var_par, 2, 1, access_pilha);
			add_commands(code_mov_var_par, sizeof(code_mov_var_par));
			break;
		}

		case 'p': {
			if(view_x86_sintax){
				printf("movl -28(%%rbp), %%edi\n");
				puts("");
			}
			/*Assembly code*************|hex***************
			*movl -28(%rbp), %edi		| 8b 7d e4 
			**********************************************/
			add_commands(code_mov_par_par, sizeof(code_mov_par_par));
			break;
		}
		default: printf("Erro na leitura");  //error
	}
	int access_pilha = -4*(idx0+1);
	if(view_x86_sintax){
		printf("call %d\n",fx);
		printf("movl %%eax, %d(%%rbp)\n",access_pilha);
		puts("");
	}
	num_lendian(code_call, 1, 4, f_offset(fx));
	num_lendian(code_call, 7, 1, access_pilha);
	add_commands(code_call, sizeof(code_call));
}

void lbs_to_asm_zret(char var0, char var1, int idx0, int idx1){

	/* ----------------- CASO ZRET (RETORNO CONDICIONAL) ----------------

	zret varpc varpc

	var0 :== constante($) - variavel(V) - parametro(P)

	CASO VAR0 $

	movl $constante, %r10d				=== SALVANDO A CONSTANTE EM R10D PARA COMPARAR DEPOIS

	CASO VAR0 V

	movl -x(%rbp), %r10d				=== SALVANDO A VARIAVEL EM R10D PARA COMPARAR DEPOIS

	CASO VAR0 P

	movl -28(%rbp), %r10d					=== SALVANDO O PARAMETRO EM R10D PARA COMPARAR DEPOIS


	CONTINUAÇÃO ZRET


	cmpl $0, %r10d
	movl <retorno>, %eax
	jne <fx>
	leave
	ret

	---------------------------------------------------------------------*/

	switch(var0){

		case '$': {
			if(view_x86_sintax){
				printf("movl $%d, %%r10d\n", idx0);
				puts("");
			}
			num_lendian(code_mov_cst_reg, 2, 4, idx0);
			add_commands(code_mov_cst_reg, sizeof(code_mov_cst_reg));
			break;
		}

		case 'v': {
			int access_pilha = -4*(idx0+1);
			if(view_x86_sintax){
				printf("movl %d(%%rbp), %%r10d\n", access_pilha);
				puts("");
			}
			num_lendian(code_mov_var_reg, 3, 1, access_pilha);
			add_commands(code_mov_var_reg, sizeof(code_mov_var_reg));
			break;
		}

		case 'p': {
			if(view_x86_sintax){
				printf("movl -28(%%rbp), %%r10d\n");
				puts("");
			}
			add_commands(code_mov_par_reg, sizeof(code_mov_par_reg));
			break;
		}
		default: printf("Erro na leitura");  //error
	}
	if(view_x86_sintax){
		printf("cmpl $0, %%r10d\n");
		printf("movl %c%d, %%eax\n",var1,idx1);
		printf("jne %d\n",func_count);
	}
	/*cmpl $0, %r10d*/
	add_commands(code_zret_cmpl, sizeof(code_zret_cmpl));

	/*jne*/
	add_commands(code_zret_jne, sizeof(code_zret_jne));

	/*movl*/
	lbs_to_asm_ret(var1, idx1);

	/*end*/
	lbs_to_asm_end();
	//printf("%d:\n",funcLabel);
}


//Trocar
void num_lendian ( unsigned char *commands, size_t pos, size_t bytes, int number ) {

	int i = 0;
	char byte_nulo;

	if( number < 0 ) {
		number = ~(-(number+1)); /* inv(-(x+1)) */
		byte_nulo = 0xff;
	} /* if */
	else {
		byte_nulo = 0x00;
	} /* else */

	while( bytes-- ) {
		char num_byte = number & 0xff;
		commands[pos+i] = number ? num_byte : byte_nulo;
		number = number >> 8;
		i++;
	} /* while */
	
} /* fim da função num_lendian */

void add_commands(unsigned char *commands, size_t bytes){
	for (int i = 0; i < bytes; i++) {
		p_code[current_byte] = commands[i];
		current_byte++;
	}
}