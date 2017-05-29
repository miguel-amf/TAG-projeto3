/*
MARCO 2017
UNIVERSIDADE DE BRASILIA
DEPARTAMENTO DE CIENCIA DA COMPUTACAO

TEORIA E APLICACAO DE GRAFOS
DOCENTE RESPONSAVEL:
PROFESSOR DR. DIBIO LEANDRO BORGES

PROJETO 2 - ORGANIZACAO TOPOLOGICA DO FLUXO DE BACHARELADO EM CIENCIA DA COMPUTACAO
			COM IDENTIFICACAO DE CAMINHO CRITICO

DISCENTES:
GUILHERME GOMES PRANDI 15/0128274
MIGUEL A. MONTAGNER FILHO 13/0127302

COMPILADO UTILIZANDO gcc -ansi -Wall

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Adj
{
    int id;
    struct Adj *prox;
} Adj;

typedef struct Vertice
{
    int id;
    int situacao;
    int hab;
    int array[5];
    struct Vertice *prox;
    struct Adj *adj;

} Vertice;

/*lista de caminhos, caminhos eh uma sequencia de Vertices*/
typedef struct Caminhos
{
	struct Vertice *vertices;
	float dificuldadeCaminho;
	struct Caminhos *prox;

} Caminhos; 




/*define tamanho max de cada linha do arquivo disciplinas.txt*/
#define TAM_LINHA 100
/*Nome do arquivo a ser lido*/
#define NOME_ARQUIVO "entradaProj3TAG.txt"
/*define quantos professores existem*/
#define NUM_PROFESSORES 100
/*define quantas escolas existem*/
#define NUM_ESCOLAS 50

/*CABECALHO DE FUNCOES*/
int povoaLista (Vertice **, Vertice **);

/*Funcoes de push e pop de adjacentes de vertice*/
void pushAdj(Adj **, int);
void popAdj(Adj **, int);
void liberaListaAdj(Adj **);

/*Funcoes de push e pop de vertices*/
void pushVertice(Vertice **, int);
void pushVerticeNoFinal(Vertice **, int);
void popVertice(Vertice **, int);
void liberaListaVertice(Vertice **);

/*funcoes de lista*/
void imprimeLista(Vertice *);
Vertice *copiaLista(Vertice *);
void atualizaIncidentes(Vertice *);
Vertice *listaAdjacenciaParaIncidencia(Vertice *);

/*funcoes especificas ao trabalho*/
void theBigWedding(Vertice *, Vertice*);
void tornaDisponivelProfessor(Vertice *, int);
void tornaPositivoArray(Vertice *);

/*Funcoes auxiliares*/
char *idParaNome(int);

/*
ESTRUTURA DE DADOS UTILIZADA:
LISTA DE DISCIPLINAS COM LISTA DE ADJACENCIA
EXEMPLO:
LISTA DE VERTICES     LISTA ADJACENCIA
    113784				->112345->115432->119876->NULL
    ||
    \/               
    112364            	->116475->118253->NULL
    ||
    \/
    NULL
*/

void imprimeLista(Vertice *lista){

    Vertice *cursor = lista;
    int i = 0;

    printf("id\tqtdHab\tEscola\tLista de preferencias\n");
    while(cursor!=NULL) {
    	printf("%d\t%d\t%d\t", cursor->id, cursor->hab, cursor->situacao);
    	for(i=0;i<5;i++) {
    		printf("%d ", cursor->array[i]);
    	}
    	printf("\n");
    	cursor = cursor->prox;
    }

}


/*Recebe uma lista de adj por referencia, cria um novo Adj, coloca no fim da lista e atualiza o inicio fornecido se necessario*/
void pushAdj(Adj **inicio, int id) {

    /*aloca espaco para novo adj*/
    Adj *adjNovo = malloc(sizeof(Adj));
    Adj *cursor = *inicio;

    /*coloca valor do id*/
    adjNovo->id = id;

 	/*coloca NULL como proximo*/
 	adjNovo->prox = NULL;

 	/*caso a lista esteja vazia, apenas insere*/
 	if (*inicio == NULL) {
 		*inicio = adjNovo;
 		return;
 	}
 	/*percorre ate o final da lista*/
 	while(cursor->prox != NULL) cursor = cursor->prox;
 	/*insere no final*/
 	cursor->prox = adjNovo; 
    

}

void popAdj(Adj **inicio, int id) {

	/*checa para ver se a lista eh nula*/
    if(*inicio == NULL) {
        return;
    }

    Adj *cursor = *inicio;
    Adj *cursorProx = cursor->prox;
    
    /*verifica se o inicio aponta para o elemento a ser removido*/
    if (cursor->id == id) {
    	*inicio = (*inicio)->prox;
    	free(cursor);
    	return;

    }
    
    while(cursorProx != NULL) {
    	if(cursorProx->id == id) {
    		cursor->prox = cursorProx->prox;
    		free(cursorProx);
    		return;
    	}
    	cursor = cursorProx;
    	cursorProx = cursorProx->prox;
    }
    /*como o while acima chama return qdo encontra, 
    esta linha so sera executada se sair do loop 
    por "== null", logo pode-se assumir que nao encontrou*/
    printf("\nATENCAO:POPADJ CHAMADO MAS ELEM NAO ENCONTRADO\n");
}

void liberaListaAdj(Adj **inicio) {
	Adj *cursorProx = NULL;
	/*percorre a lista enquanto desaloca*/
	while(*inicio != NULL) {
		cursorProx = (*inicio)->prox;
		free(*inicio);
		*inicio = cursorProx;
	}
}
/*Aloca espaco e coloca um vertice com o novo id no final da lista*/
void pushVerticeNoFinal(Vertice **inicio, int id) {
	Vertice *cursor = *inicio;

    /*aloca espaco para novo vertice*/
    Vertice *verticeNovo = malloc(sizeof(Vertice));

    /*coloca valor do id e inicia os outros campos*/
    verticeNovo->id = id;
    verticeNovo->adj = NULL;

    if (*inicio == NULL) {
    	*inicio = verticeNovo;
    	verticeNovo->prox = NULL;
    	return;
    }
    /*faz novo vertice apontar para o inicio da lista de vertice*/
    while (cursor->prox != NULL) cursor = cursor->prox;
    verticeNovo->prox = NULL;
    cursor->prox = verticeNovo;    

}

void pushVertice(Vertice **inicio, int id) {

    /*aloca espaco para novo vertice*/
    Vertice *verticeNovo = malloc(sizeof(Vertice));

    /*coloca valor do id e inicia os outros campos*/
    verticeNovo->id = id;
    verticeNovo->adj = NULL;
    /*faz novo vertice apontar para o inicio da lista de vertice*/
    verticeNovo->prox = *inicio;
    /*atualiza o valor do inicio da lista*/
	*inicio = verticeNovo;
} 

void popVertice(Vertice **inicio, int id) {

	/*checa para ver se a lista eh nula*/
    if(*inicio == NULL) {
        return;
    }

    Vertice *cursor = *inicio;
    Vertice *cursorProx = cursor->prox;
    
    /*verifica se o inicio aponta para o elemento a ser removido*/
    if (cursor->id == id) {
    	*inicio = (*inicio)->prox;
    	free(cursor);
    	return;

    }
    
    while(cursorProx != NULL) {
    	if(cursorProx->id == id) {
    		cursor->prox = cursorProx->prox;
    		free(cursorProx);
    		return;
    	}
    	cursor = cursorProx;
    	cursorProx = cursorProx->prox;
    }
    /*como o while acima chama return qdo encontra, 
    esta linha so sera executada se sair do loop 
    por "== null", logo pode-se assumir que nao encontrou*/
    printf("\nATENCAO:POPVERTICE CHAMADO MAS ELEM NAO ENCONTRADO\n");
   

}

void liberaListaVertice(Vertice **inicio) {

	if(*inicio == NULL) {
		return;
	}

    Vertice *cursorProx = (*inicio)->prox;

    while(*inicio != NULL) {
    	cursorProx = (*inicio)->prox;
    	/*libera sua lista de adjacencia antes*/
		liberaListaAdj(&(*inicio)->adj);
        free(*inicio);
        *inicio = cursorProx;
    }

}

/*Percorre o arquivo alunos.txt e povoa a lista de adjacencia*/
int povoaLista(Vertice **profs, Vertice **escolas) {

    
    /*declara e inicializa variaveis a serem utilizadas*/
    FILE *arquivo = NULL;
    char linhaAtual[TAM_LINHA];
    int i = 0, j = 0;
    int habilitacao;
    int array[5];



    /*Abre arquivo com nomes e matriculas, em modo leitura*/
    arquivo = fopen(NOME_ARQUIVO, "r");
    if (arquivo == NULL) {
        printf("\nERRO: Arquivo %s nao encontrado\n", NOME_ARQUIVO);
        return -1;
    }

    /*
    percorre o arquivo disciplinas.txt
    vai de linha em linha, colocando-as em uma string, supondo formato 
    			mat_vertice#creditos#dificuldade#mat_adj1#mat_adj2#mat_adj3#...#mat_adjN#\n
    percorre ate chegar a EOF
    a cada iteracao, aloca espaco para novo vertice e taribui a ele os valores lidos, e depois adiciona suas adjacencias
    */
    for(i = 0;i<NUM_PROFESSORES; i++) {
    
    	fgets(linhaAtual, TAM_LINHA, arquivo);

    	/*aloca e coloca no inicio da lista*/
		pushVertice(profs, i+1);

        /*le os valores habilitacao seguido das 5 preferencias*/
		sscanf(linhaAtual, "%d %d %d %d %d %d", &habilitacao, array, array + 1, array + 2, array + 3, array + 4);
		(*profs)->hab = habilitacao;

		/*inicializa a situacao para o valor 0*/
		(*profs)->situacao = 0;

		/*preenche as preferencias*/
		for (j = 0; j < 5; ++j)
		{
			(*profs)->array[j] = array[j];
		}
    }

    for(i = 0;i<NUM_ESCOLAS; i++) {
    
    	fgets(linhaAtual, TAM_LINHA, arquivo);

		pushVertice(escolas, i+1);

        /*le a matricula da disciplina*/
		sscanf(linhaAtual, "%d", &habilitacao);
		(*escolas)->hab = habilitacao;
		for (j = 0; j < 5; ++j)
		{
			(*escolas)->array[j] = 0;
		}

    }

return i;
}

void theBigWedding(Vertice *professores, Vertice *escolas) {
	/*
	para simplificar e utilizar o mesmo struct tanto para profs qto para escolas, o significado
	de cada variavel muda:
	em professores:
	hab : nivel de habilitacao do professor
	array: 	indica a preferencia por id de escola, sendo array[0] a escola de maior preferencia
			se id estiver negativo, quer dizer que aquele id já foi testado e o professor rejeitado
	situacao: flag, se 0 entao esta sem emprego, se != 0 entao esta alocado na escola com o id armazenado

	em escolas:
	hab: nivel de habilitacao que a escola quer
	array: 	posicao 0-1: id dos professores aceitos
			posicao 2-3: nivel dos professores nas posicoes 0-1, respectivamente
	situacao: nao ha uso
	*/

	/*
	Funcionamento do algoritmo:
	primeiro ele preenche apenas uma das duas vagas de cada escola
	cada professor faz sua "serenata", de acordo com sua lista de preferencia,
	e pra escola aceitar ele deve ter o exato HAB que ela precisa.
	repete-se até as primeiras 50 vagas estarem preenchidas ou todos os professores
	sem emprego terem tentado nas 5 de sua preferencia

	agora repete-se o mesmo processo, mas para a segunda vaga de cada escola.

	espera-se que agora exista tanto nas primeiras vagas quanto nas segundas espaços em branco, pois
	as escolas estavam com o critério muito exigente, pois queriam apenas quem tivesse HAB igual

	agora deve-se relaxar a regra, aceitando pessoas com grau próximo do requerido.

	novamente, os professores que estão sem emprego farao seus pedidos, e cada escola pegara
	o pedinte de maior qualificacao para a primeira vaga (se a vaga ainda estiver aberta)

	repete-se o mesmo processo, mas para a segunda vaga de cada escola.

	espera-se que todas as vagas estarão preenchidas. caso contrário, significa que existe vaga que
	caso algum professor a ocupasse geraria uma instabilidade no relacionamento, pois se ocupada ou seria por
	um professor que nao quer dar aula la ou prefere uma escola que tambem o prefere.

	este algoritmo poderia forcar os sem empregos a ocuparem vagas que eles nao querem, para tal fazendo com que as escolas
	por sua vez procurassem por professores e o professor escolheria a escola menos pior para ele, contudo este
	algoritmo nao realiza isso por demanda da documentacao do trabalho.
	*/

	int i = 0;
	Vertice *cursorEscola = escolas;
	Vertice *cursorProf = professores;

	/*primeira parte: colocar nas vagas apenas os relacionamentos otimos*/
	for (i = 0; i<5; i++) /*itera de 0 a 4, percorrendo gradualmente o array de preferencia dos professores*/
	{
		cursorProf = professores; /*volta ao inicio da lista para o proximo round de preferencias*/
		while (cursorProf != NULL) 
		{
			if (cursorProf->situacao == 0)/*verifica se ele ja nao esta empregado*/
			{
				cursorEscola = escolas;
				while(cursorEscola->id != cursorProf->array[i]) /*localiza na lista a escola que o professor quer se candidatar*/
				{
					cursorEscola = cursorEscola->prox;
				}
				/*verifica se a vaga esta disponivel e se as habilidades se casam*/
				if ((cursorEscola->array[0] == 0) && (cursorEscola->hab == cursorProf->hab)) 
				{
					/*armazena as informacoes de que houve um casamento*/
					cursorEscola->array[0] = cursorProf->id;
					cursorEscola->array[2] = cursorProf->hab;
					cursorProf->situacao = cursorEscola->id;
				}
			}
			cursorProf = cursorProf->prox;
		}
	}
	/*repetindo o processo para a segunda vaga*/
	for (i=0; i<5; i++) /*itera de 0 a 4, percorrendo gradualmente o array de preferencia dos professores*/
	{
		cursorProf = professores; /*volta ao inicio da lista para o proximo round de preferencias*/
		while (cursorProf != NULL) 
		{
			if (cursorProf->situacao == 0)/*verifica se ele ja nao esta empregado*/
			{
				cursorEscola = escolas;
				while(cursorEscola->id != cursorProf->array[i]) /*localiza na lista a escola que o professor quer se candidatar*/
				{
					cursorEscola = cursorEscola->prox;
				}
				/*verifica se a vaga esta disponivel e se as habilidades se casam*/
				if ((cursorEscola->array[1] == 0) && (cursorEscola->hab == cursorProf->hab)) 
				{
					/*armazena as informacoes de que houve um casamento*/
					cursorEscola->array[1] = cursorProf->id;
					cursorEscola->array[3] = cursorProf->hab;
					cursorProf->situacao = cursorEscola->id;
				}
			}
			cursorProf = cursorProf->prox;
		}
	}

	/*agora serao colocados os professores que nao se adequam perfeitamente na vaga*/
	/*para distinguir essas vagas nao otimas, qdo ocupadas os ids serao negativos*/
	/*
	as escolas com vagas irao inicialmente aceitar qualquer professor que faca um pedido
	depois, qualquer novo pedido para ela sera avaliado, com a ideia central de aproximar
	ao maximo o professor com a vaga, ou seja, nao ter alguem qualificado demais nem de menos
	fluxograma:
	o professor que tenho eh qualificado o suficiente?
		sim: o pedinte eh qualificado o suficiente?
			sim: o pedinte eh menos qualificado que o atual?
				sim: aceita (pois nao preciso de tanta qualificacao)
				nao: rejeita (sao ambos qualificados demais, entao mantenho)
			nao: rejeita (ele nao conseguira suprir minha necessidade melhor que o atual)
		nao: ele eh mais qualificado que o atual?
			sim: aceita (para aproximar ao valor que quero)
			nao: rejeita
	*/
	/*houveMudanca serve para verificar se ainda existe trocas de vagas a cada iteracao*/
	int houveMudanca = 1;
	while (houveMudanca != 0)
	{
		houveMudanca = 0;
		cursorProf = professores; /*volta ao inicio da lista para o proximo round de preferencias*/
		while (cursorProf != NULL) 
		{
			while (cursorProf->situacao == 0)/*verifica se ele ja nao esta empregado/conseguiu emprego*/
			{
				/*para denotar que o professor ja pediu para determinada escola e foi rejeitado, toda vez que
				ele faz um pedido o id da escola se torna negativo */
				i = 0;
				/*percorre para encontrar ate onde ele ja pediu*/
				for(i=0;i<5;i++) {
					if (cursorProf->array[i] > 0) {
						break; /*encontrou um id positivo, no indice i, ou seja, ainda nao pediu para esse id*/
					}
				}
				/*se i = 5, significa que nao encontrou positivo, ou seja, professor ja pediu
				para todas as escolas que gostaria e ninguem o aceitou*/
				if(i == 5) {
					/*coloca o professor como indisponivel*/
					cursorProf->situacao = -1;
					/*pula para o proximo professor*/
					break;
				}
				/*localiza a escola*/
				cursorEscola = escolas;
				while(cursorEscola->id != cursorProf->array[i])
				{
					cursorEscola = cursorEscola->prox;
				}
				/*negativa o id, para denotar que o pedido vai ser feito*/
				cursorProf->array[i] = -(cursorProf->array[i]);
				/*realiza o pedido*/
				if(cursorEscola->array[0] <= 0) {
					/*quer dizer que a vaga esta disponivel ou existe professor nao otimo, ou seja,
					a escola esta com a vaga aberta para sugestoes*/
					if(cursorEscola->array[0] == 0) {
						/*nao ha ninguem na vaga, entao automaticamente aceita o professor*/
						cursorProf->situacao = cursorEscola->id;
						cursorEscola->array[0] = -(cursorProf->id); /*negativo para denotar vaga temporaria*/
						cursorEscola->array[2] = cursorProf->hab;
						houveMudanca = 1; /*sinaliza para o loop que ainda esta ocorrendo mudancas*/
					} else {
						/*ha alguem na vaga, logo o duelo docente ira comecar*/
						/*se a habilitacao do professor atual for maior que a que a escola pede*/
						if((cursorEscola->array[2] - cursorEscola->hab) > 0) {
							/*esse pedinte eh bom o suficiente mas pior que o atual?*/
							if (cursorProf->hab >= cursorEscola->hab && cursorProf->hab < cursorEscola->array[2]) {
								/*sim: aceita*/
								/*remove o professor atual da vaga*/
								tornaDisponivelProfessor(professores, abs(cursorEscola->array[0]));
								/*coloca o novo professor na vaga*/
								cursorProf->situacao = cursorEscola->id;
								cursorEscola->array[0] = -(cursorProf->id); /*negativo para denotar vaga temporaria*/
								cursorEscola->array[2] = cursorProf->hab;
								houveMudanca = 1; /*sinaliza para o loop que ainda esta ocorrendo mudancas*/
							} /*nao: rejeita*/
						} else {
							/*o professor atual eh inferior ao que a escola pede*/
							/*se o professor pedinte for melhor que o atual*/
							if(cursorProf->hab > cursorEscola->array[2]) {
								/*sim: aceita*/
								/*remove o professor atual da vaga*/
								tornaDisponivelProfessor(professores, abs(cursorEscola->array[0]));
								/*coloca o novo professor na vaga*/
								cursorProf->situacao = cursorEscola->id;
								cursorEscola->array[0] = -(cursorProf->id); /*negativo para denotar vaga temporaria*/
								cursorEscola->array[2] = cursorProf->hab;
								houveMudanca = 1; /*sinaliza para o loop que ainda esta ocorrendo mudancas*/
							}
						}
					}

				}
			}
			cursorProf = cursorProf->prox;
		}
	}

	/*realiza o processo novamente, para a segunda vaga. antes deve-se voltar ao normal aqueles rejeitados para a primeira
	vaga*/
	cursorProf = professores;
	while (cursorProf != NULL) {
		if (cursorProf->situacao == -1) {
			cursorProf->situacao = 0;
		}
		cursorProf = cursorProf->prox;
	}
	/*solidifica as vagas e reinicializa os pedidos dos professores*/
	tornaPositivoArray(professores);
	tornaPositivoArray(escolas);
	houveMudanca = 1;
	/*comeca novamente para a segunda vaga*/
	while (houveMudanca != 0)
	{
		houveMudanca = 0;
		cursorProf = professores; /*volta ao inicio da lista para o proximo round de preferencias*/
		while (cursorProf != NULL) 
		{
			while (cursorProf->situacao == 0)/*verifica se ele ja nao esta empregado/conseguiu emprego*/
			{
				/*para denotar que o professor ja pediu para determinada escola e foi rejeitado, toda vez que
				ele faz um pedido o id da escola se torna negativo */
				i = 0;
				/*percorre para encontrar ate onde ele ja pediu*/
				for(i=0;i<5;i++) {
					if (cursorProf->array[i] > 0) {
						break; /*encontrou um id positivo, no indice i, ou seja, ainda nao pediu para esse id*/
					}
				}
				/*se i = 5, significa que nao encontrou positivo, ou seja, professor ja pediu
				para todas as escolas que gostaria e ninguem o aceitou*/
				if(i == 5) {
					/*coloca o professor como indisponivel*/
					cursorProf->situacao = -1;
					/*pula para o proximo professor*/
					break;
				}
				/*localiza a escola*/
				cursorEscola = escolas;
				while(cursorEscola->id != cursorProf->array[i])
				{
					cursorEscola = cursorEscola->prox;
				}
				/*negativa o id, para denotar que o pedido vai ser feito*/
				cursorProf->array[i] = -(cursorProf->array[i]);
				/*realiza o pedido*/
				if(cursorEscola->array[1] <= 0) {
					/*quer dizer que a vaga esta disponivel ou existe professor nao otimo, ou seja,
					a escola esta com a vaga aberta para sugestoes*/
					if(cursorEscola->array[1] == 0) {
						/*nao ha ninguem na vaga, entao automaticamente aceita o professor*/
						cursorProf->situacao = cursorEscola->id;
						cursorEscola->array[1] = -(cursorProf->id); /*negativo para denotar vaga temporaria*/
						cursorEscola->array[3] = cursorProf->hab;
						houveMudanca = 1; /*sinaliza para o loop que ainda esta ocorrendo mudancas*/
					} else {
						/*ha alguem na vaga, logo o duelo docente ira comecar*/
						/*se a habilitacao do professor atual for maior que a que a escola pede*/
						if((cursorEscola->array[3] - cursorEscola->hab) > 0) {
							/*esse pedinte eh bom o suficiente mas pior que o atual?*/
							if (cursorProf->hab >= cursorEscola->hab && cursorProf->hab < cursorEscola->array[3]) {
								/*sim: aceita*/
								/*remove o professor atual da vaga*/
								tornaDisponivelProfessor(professores, abs(cursorEscola->array[1]));
								/*coloca o novo professor na vaga*/
								cursorProf->situacao = cursorEscola->id;
								cursorEscola->array[1] = -(cursorProf->id); /*negativo para denotar vaga temporaria*/
								cursorEscola->array[3] = cursorProf->hab;
								houveMudanca = 1; /*sinaliza para o loop que ainda esta ocorrendo mudancas*/
							} /*nao: rejeita*/
						} else {
							/*o professor atual eh inferior ao que a escola pede*/
							/*se o professor pedinte for melhor que o atual*/
							if(cursorProf->hab > cursorEscola->array[3]) {
								/*sim: aceita*/
								/*remove o professor atual da vaga*/
								tornaDisponivelProfessor(professores, abs(cursorEscola->array[1]));
								/*coloca o novo professor na vaga*/
								cursorProf->situacao = cursorEscola->id;
								cursorEscola->array[1] = -(cursorProf->id); /*negativo para denotar vaga temporaria*/
								cursorEscola->array[3] = cursorProf->hab;
								houveMudanca = 1; /*sinaliza para o loop que ainda esta ocorrendo mudancas*/
							}
						}
					}

				}
			}
			cursorProf = cursorProf->prox;
		}
	}

	/*finaliza o algoritmo, tornando os valores todos positivos*/
		cursorProf = professores;
	while (cursorProf != NULL) {
		if (cursorProf->situacao == -1) {
			cursorProf->situacao = 0;
		}
		cursorProf = cursorProf->prox;
	}
	tornaPositivoArray(professores);
	tornaPositivoArray(escolas);
}

void tornaDisponivelProfessor(Vertice *professores, int idProfessor) {
	while(professores->id != idProfessor) {
		professores = professores->prox;
	}
	professores->situacao = 0;
}

void tornaPositivoArray(Vertice *professores) {
	int i = 0;
	while (professores != NULL) {
		for(i=0; i < 5; i++) {
			professores->array[i] = abs(professores->array[i]);
		}
		professores = professores->prox;
	}
}

/*FUNCAO MAIN*/
int main () {


	Vertice *professores = NULL;
	Vertice *escolas = NULL;
	int i = 0;

    /*ETAPA DE COLETA DE DADOS*/

    povoaLista(&professores, &escolas);

    theBigWedding(professores, escolas);


    /*ETAPA DE APRESENTACAO DOS RESULTADOS*/

    Vertice *cursorEscolas = escolas;
    printf("\nResultados:\nEscolas, seu nivel pedido, o id dos professores e seus niveis\n");
    while(cursorEscolas != NULL) {
    	printf("#%2d(nv%d): #%2d(%2d) #%2d(%2d)\n", 	cursorEscolas->id, 
    																	cursorEscolas->hab, 
    																	cursorEscolas->array[0], 
    																	cursorEscolas->array[2], 
    																	cursorEscolas->array[1], 
    																	cursorEscolas->array[3]);



    	cursorEscolas = cursorEscolas->prox;
    }
    printf("\nAgora Mostraremos os professores sem emprego e escolas com vagas\nPressione ENTER para continuar");
    getchar();
    Vertice *cursorProf = professores;
    cursorEscolas = escolas;

    printf("Professores sem emprego:\n");
    while(cursorProf != NULL) {
    	if(cursorProf->situacao == 0) {
    		printf("professor #%d: nivel%d preferencia: ", cursorProf->id, cursorProf->hab);
    		for(i=0;i<5;i++) {
    			printf("%d ", cursorProf->array[i]);
    		}
    		printf("\n");
    	}
    	cursorProf = cursorProf->prox;
    }
    printf("Escolas com vagas:\n");
    while(cursorEscolas != NULL) {
    	if(cursorEscolas->array[0]==0 || cursorEscolas->array[1]==0) {
    		printf("Escola #%d (nivel %d)\n", cursorEscolas->id, cursorEscolas->hab);
    	}
    	cursorEscolas = cursorEscolas->prox;
    }

    printf("NOTA: possivelmente haverao escolas com vagas, mesmo com professores sem emprego. Contudo, os professores sem emprego nao querem trabalhar naquelas escolas, e professores que possivelmente trabalhariam la ja encontraram trabalho em outro local.\n");
    printf("poderia portanto alocar os sem empregos nestas vagas, contudo foi optado em nao realizar tal acao, devido a restricao de nao colocar ");
    printf("professores que não querem trabalhar em determinada escola nela.");

    /*ETAPA DE FINALIZACAO DO PROGRAMA*/


    printf("\nFIM DO PROGRAMA\n");
    return 0;

}