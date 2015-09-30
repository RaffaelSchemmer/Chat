/* Universidade Regional Integrada do Alto uruguai e das Missões */
/* Cliente do chat implementado em linguagem C utilizando paradigma de programacão procedural */
/* Disciplina : Linguagem de Programacao IV - 5 Semestre */
/* Professor : Mr.Luiz Carlos Gubert */
/* Aluno : Raffael Bottoli Schemmer */
/* Data Termino : 30/04/2009 */
#include<stdio.h>
#include<string.h> /* Bibliotecas padroes */
#include<stdlib.h>

#include<pthread.h>
#include<sys/types.h> /* Bibliotecas para implementar threads */

#include<sys/socket.h>
#include<netinet/in.h> /* Bibliotecas para implementar */
#include<netdb.h>
/* Funcao responsavel por tratar os erros */
void tratamento_de_erros(int numero_erro)
{
	switch(numero_erro)
	{
		case(0): /* Tratamento do retorno == 0 de recv */
		{
			printf("Erro(1) : O servidor encerrou a sua conexao !!\n");
			break;
		}
		case(1): /* Tratamento do retorno < 0 de recv */
		{
			printf("Erro(2) : Erro de leitura da mensagem do servidor !!\n");
			break;
		}	
		case(2): /* Tratamento de falta do nome do servidor e da porta */
		{
			printf("Erro(3) : Faltam dois parametros !! Ex : (./cliente localhost 5555)\n");
			break;
		}
		case(3): /* Tratametno da falta da porta do servidor */
		{
			printf("Erro(4) : Falta um parametro !! Ex : (./cliente localhost 5555)\n");
			break;
		}	
		case(4): /* Tratamento do retorno de gethostbyname */
		{
			printf("Erro(5) : Nao foi possivel resolver o servidor !!\n");
			break;
		}
		case(5): /* Tratamento do retorno de socket */
		{
			printf("Erro(6) : Nao foi possivel abrir o socket !!\n");
			
			break;
		}
		case(6): /* Tratamento do retorno de connect */
		{
			printf("Erro(7) : O cliente nao pode se conectar ao servidor !!\n");
			break;
		}
		case(7): /* Tratamento do retorno de send */
		{
		        printf("Erro(8) : Nao foi possivel escrever mensagem ao servidor !!\n");
		        break;
		}
	}
}
/* Funcao reponsavel por escutar as mensagens enviadas pelo servidor */
void escuta_servidor(int socket_cliente) /* Recebe como parametro o socket do cliente */
{
	int retorno,cont; 
	char buffer[290];
	char nome_cliente[30];
	while(1) /* Looping infinito que fica escutando o servidor ate a main nao terminar e caso nao acontecam erros de leitura */
	{
		retorno = recv(socket_cliente,buffer,sizeof(buffer),0); /* Fica escutando o socket esperando a mensagem do servidor */
		if(retorno == 0) /* Se o retorno de recv for zero informa que o servidor foi encerrado */
		{
			tratamento_de_erros(0);
			exit(0); /* Para de executar a funcao(thread) finalizando o cliente */
		}
		else if(retorno < 0) /* Se o retorno de recv for menor que zero informa que nao foi possivel ler do servidor */
		{
			tratamento_de_erros(1);
			exit(1); /* Para de executar a funcao(thread) finalizando o cliente */
		}
		retorno = recv(socket_cliente,nome_cliente,sizeof(nome_cliente),0); /* Fica escutando o socket do servidor esperando o nome do cliente */
		if(retorno == 0) /* Se o retorno de recv for zero informa que o servidor foi encerrado */
		{
			tratamento_de_erros(0);
			exit(0); /* Para de executar a funcao(thread) finalizando o cliente */
		}
		else if(retorno < 0)  /* Se o retorno de recv for menor que zero informa que nao foi possivel ler do servidor */
		{
			tratamento_de_erros(1);
			exit(1);  /* Para de executar a funcao(thread) finalizando o cliente */
		}
		cont = 0; /* Variavel cont temporaria recebe zero */
		while(buffer[cont]!='\n') /* Varre a mensagem contida em buffer ate que a posicao cont seja diferente de \n */
		{
			cont ++;
		}
		buffer[cont] = '\0'; /* A posicao cont de buffer recebe \0 tirando o \n da mensagem */
		printf("Cliente (%s) falou : %s\n",nome_cliente,buffer); /* Informa a mensagem e o nome do cliente que a enviou */
		buffer [0] = '\n'; /* Limpa o buffer colocando \n na primeira posicao */
		printf("> : ");
		fflush(stdout);
	} 
}
/* Funcao reponsavel por escrever as mensagens do usuario ao servidor */
void escreve_servidor(int socket_cliente)
{
        char buffer[256];
        int retorno;
        buffer[0] = '\n'; /* Limpa o buffer colocando \n na primeira posicao */
        printf("> : ");
        while(fgets(buffer,256,stdin) != NULL) /* Executa as leituras enquanto o retorno de fgets for diferente de NULL */
        {
        	if((strcmp(buffer,"esc\n")) == 0) /* Compara o que foi informado e se for igual a esc finaliza o cliente */
		{
			exit(1);
		}
		if(buffer[0] != '\n') /* Caso o conteudo do buffer(mensagem) indice zero for diferente de \n(sem mensagem) envia o buffer ao servidor */
		{
			retorno = send(socket_cliente,buffer,strlen(buffer),0);
			if(retorno < 0){tratamento_de_erros(7);} /* Se o retorno de send for menor que zero informa que nao foi possivel escrever */
		}
		printf("> : ");
		fflush(stdout);
	}
}
int main (int argc, char *argv[])
{
	int socket_cliente,numero_da_porta,var_thread1,cont,retorno;
	struct sockaddr_in end_int_serv;
	struct hostent *hs;
	char nome_cliente[30],equals[3];
	char nome_servidor[15];
	pthread_t thread_cliente;
	if(argc < 3) 
	{
		if(argc < 2) /* Trata se o usuario esqueceu de informar o nome e a porta do servidor */
		{
			tratamento_de_erros(2);
		}
		else /* Trata se o usuario esqueceu de informar a porta do servidor */
		{
			tratamento_de_erros(3);
		}
		exit(1); /* Finaliza o cliente caso nao tiver os parametros */
	}
	numero_da_porta = atoi(argv[2]); /* Captura de argv o numero da porta informado pelo usuario */
	strcpy(nome_servidor,argv[1]); /* Captura de argv o nome do servidor informado pelo usuario */
  	hs = gethostbyname(nome_servidor); /* Tenta localizar o host */
	if(hs == NULL) /* Se o retorno de gethostbyname for null o host nao pode ser encontrado */
	{
		tratamento_de_erros(4);
		exit(0); /* Finaliza o cliente se nao encontrou o servidor */
	}
	bzero((char*)&end_int_serv,sizeof(end_int_serv));
  	bcopy(hs->h_addr,(char*)&end_int_serv.sin_addr,hs->h_length);
  	end_int_serv.sin_family = AF_INET; /* Estabelece os campos do endereco de internet do servidor */
  	end_int_serv.sin_port = htons(numero_da_porta);
  	if((socket_cliente = socket(AF_INET, SOCK_STREAM,0)) < 0) /* Responsavel por criar o socket do cliente */
	{
		tratamento_de_erros(5);
		exit(1); /* Finaliza o cliente caso nao conseguir criar o socket */
   	}
  	if(connect(socket_cliente,(struct sockaddr *)&end_int_serv,sizeof(end_int_serv)) < 0) /* Conecta o cliente ao servidor */
   	{
		tratamento_de_erros(6);
		exit(1); /* Finaliza o cliente caso nao conseguir conectar ao servidor */
   	}
	do /* Estrutura do-while responsavel por enviar nome do cliente ao servidor */
	{
		equals[0] = '0'; /* Inicializa com zero a string equals */
		printf("Digite um nome de usuario : ");
		fgets(nome_cliente,30,stdin); /* Captura nome de usuario do cliente */
		cont = 0; /* Variavel cont temporaria recebe zero */
	        while(nome_cliente[cont]!='\n') /* Varre a mensagem contida em nome_cliente ate que a posicao cont seja diferente de \n */
	        {       
	        	cont ++;
	        }
	        nome_cliente[cont] = '\0'; /* A posicao cont de nome_cliente recebe \0 tirando o \n da mensagem */
		retorno = send(socket_cliente,nome_cliente,strlen(nome_cliente)+1,0); /* Envia nome de usuario ao servidor */
		if(retorno < 0){tratamento_de_erros(7);} /* Se o retorno de send for menor que zero informa que nao foi possivel escrever */
		retorno = recv(socket_cliente,equals,sizeof(equals),0); /* Recebe a confirmacao se o nome eh valido */
		if(retorno == 0) /* Se o retorno de recv for zero informa que o servidor foi encerrado */
		{
			tratamento_de_erros(0);
			exit(0); /* Para de executar finalizando o cliente */
		}
		else if(retorno < 0)  /* Se o retorno de recv for menor que zero informa que nao foi possivel ler do servidor */
		{
			tratamento_de_erros(1);
			exit(1);  /* Para de executar a funcao(thread) finalizando o cliente */
		}
	}while(equals[0] == '1'); /* Continua no do-while enquanto nome for igual a 1 */
	printf("Conectado ao servidor %s como %s !!\n",nome_servidor,nome_cliente);
	printf("Para sair digite (esc) !!\n");
	pthread_create(&thread_cliente,NULL,(void *)&escuta_servidor,(void *)socket_cliente); /* Cria thread responsavel por escutar mensagens do servidor */
	var_thread1 = pthread_create(&thread_cliente,NULL,(void *)&escreve_servidor,(void *)socket_cliente); /* Cria thread responsavel por enviar as mensagens do cliente ao servidor */
	pthread_join(thread_cliente,(void **)&var_thread1); /* Dispara thread escreve_servidor e so deve deixar a main proseguir quando sua execucao for concluida */
	close(socket_cliente); /* Fecha o socket do cliente */
}
