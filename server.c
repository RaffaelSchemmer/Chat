/* Universidade Regional Integrada do Alto uruguai e das Missões */
/* Servidor do chat implementado em linguagem C utilizando paradigma de programacão procedural */
/* Disciplina : Linguagem de Programacao IV - 5 Semestre */
/* Professor : Mr.Luiz Carlos Gubert */
/* Aluno : Raffael Bottoli Schemmer */
/* Data Termino : 30/04/2009 */
#include<stdio.h>
#include<string.h> /* Bibliotecas padrao */
#include<stdlib.h>

#include<pthread.h>
#include<semaphore.h> /* Bibliotecas para threads */
#include<sys/types.h>

#include<sys/socket.h>
#include<netinet/in.h> /* Bibliotecas para sockets */
#include<arpa/inet.h>
/* Estrutura responsavel por armazenar os dados do cliente */
typedef struct dados_cliente
{
	int socket_cliente; /* Guarda o socket do cliente */
  	char nome_cliente[30]; /* Guarda o nome do cliente */   
	char mensagem[256]; /* Guarda a mensagem informada pelo cliente */
	struct dados_cliente *proximo; /* Guarda o endereco do proximo cliente da lista */
	struct dados_cliente *anterior; /* Guarda o endeerco do cliente anterior da lista */
}cliente;
sem_t var_sem; /* Variavel semaforo responsavel por controlar o uso as variaveis compartilhadas (ini/fim) */
cliente *ini,*fim; /* Variaveis reponsaveis por manter os enderecos do inicio e do fim da lista de clientes cadastrados */
/* Funcao responsavel por tratar os erros do servidor */
void tratamento_de_erros(int numero_erro)
{
	switch(numero_erro)
	{
		case(0): /* Tratamento de erro caso falte memoria */
		{
			printf("Erro(1) : Nao foi possivel alocar memoria !!\n");
			break;
		}
		case(1): /* Tratamento de erros recv */
		{
			printf("Erro(2) :  Nao foi possivel ler mensagem do cliente !!\n");
			break;
		}
		case(2): /* Tratamento de erros send */
		{
			printf("Erro(3) :Nao foi possivel escrever mensagem ao cliente !!\n");
			break;
		}
		case(3): /* Tratamento de erros de falta de parametros */
		{
			printf("Erro(4) : Falta um parametro !! Ex : (./servidor localhost 5555)\n");
			break;
		}
		case(4): /* Tratamento de erros da socket */
		{
			printf("Erro(5) : Nao foi possivel abrir o socket !!\n");
			break;
		}
		case(5): /* Tratamento de erros do bind */
		{
			printf("Erro(6) : Nao foi possivel ligar endereco ao socket !!\n");
			break;
		}
		case(6): /* Tratametno de erros da accept */
		{
			printf("Erro(7) : Erro na conexao com o cliente !!\n");
			break;
		}
		case(7): /* Tratamento de erros do recv da leitura do nome do cliente */
		{
		        printf("Erro(8) : Nao foi possivel ler nick do cliente !!\n");
		        break;
		}
	}
}
/* Funcao responsavel por alocar memoria a um novo cliente */
cliente* aloca_memoria()
{
	cliente *temp_cliente; /* Cria um ponteiro temporario */
	if((temp_cliente =(cliente*) malloc (sizeof(cliente))) == NULL) /* Aloca memoria para um tipo cliente e retorna o endereco  ao ponteiro temp */
	{
		tratamento_de_erros(0); /* Caso o retorno for NULL imprime que faltou memoria para aloca o cliente */
		exit(0); /* Finaliza o servidor */
	}
	return(temp_cliente); /* Retorna o novo endereco onde foi alocado memoria */
}

/* Funcao reponsavel por desalocar um endereco de memoria */
void desaloca_memoria(cliente *temp_cliente) /* Recebe como parametro o endereco a ser desalocado */
{
	free(temp_cliente); /* Chama free que desaloca o endereco */
}
/*  Funcao responsavel por validar o nome do usuario */
int valida_nick(char temp_nome_cliente[30]) /* Recebe o nome informado como parametro */
{ 
	cliente *temp_cliente;
	temp_cliente = ini;
	if(ini == NULL) /* Caso nao existir clientes criados retorna 1 dizendo que o nome foi aceito */
	{
		return(1);
	}
	if(temp_cliente == fim) /* Caso existir somente um cliente cadastrado verifica se o nome eh igual e retorna zero */
	{
		if(strcmp(temp_cliente -> nome_cliente,temp_nome_cliente) == 0)
		{
			return(0);
		}
	}
	while(temp_cliente != fim) /* Enquando o ponteiro dos clientes for diferente do fim(ultimo cliente) executa */
	{
		if(temp_cliente == ini) /* Caso o temp cliente for igual ao inicio deve comparar se os nomes foram iguais e se sim deve retornar zero */
		{
			if(strcmp(temp_cliente -> nome_cliente,temp_nome_cliente) == 0)
			{
				return(0);
			}			
		}
		temp_cliente = temp_cliente -> proximo; /* Avanca o ponteiro temporario fazendo ele receber o endereco do proximo cliente */
		if(strcmp(temp_cliente -> nome_cliente,temp_nome_cliente) == 0) /* Caso os nomes forem iguais retorna zero */
		{
			return(0);
		}
	}
	return(1); /* Se a funcao chegar ate aqui retorna 1 dizendo que o nome informado nao eh igual a nenhum cadastrado na lista de clientes */
}
/* Funcao responsavel por inciar a lista de clientes */
cliente* incia_lista_clientes(int temp_socket_cliente,char temp_nome_cliente[30]) /* Recebe como parametro o socket e o nome do cliente */
{
	cliente *temp_cliente;
	temp_cliente = aloca_memoria(); /* Chama aloca memoria que retorna o endereco da area alocada */
	temp_cliente -> proximo = NULL; /* Inicia com NULL o proximo */
	temp_cliente -> anterior = NULL; /* Inicia com NULL o anterior */
	temp_cliente -> socket_cliente = temp_socket_cliente; /* Faz o socket_cliente da estrutura receber o socket do cliente passado por parametro */
	strcpy(temp_cliente -> nome_cliente,temp_nome_cliente); /* Copia o nome passado por parametro para a estrutura nome_cliente*/
	return(temp_cliente); /* Retorna o ponteiro com o novo cliente */
}
/* Funcao responsavel por inserir o novo cliente na lista de clientes */
cliente*  insere_cliente_lista(cliente *temp_fim,int temp_socket_cliente,char temp_nome_cliente[30]) /* Recebe como parametro o endereco do fim do socket do cliente e do nome do cliente */
{	
	temp_fim -> proximo=aloca_memoria();
	temp_fim -> proximo -> proximo=NULL;  /* Chama aloca memoria que retorna o endereco da area alocada */
	temp_fim -> proximo -> anterior = temp_fim;  /* Inicia com NULL o proximo */
	temp_fim -> proximo -> socket_cliente = temp_socket_cliente; /* Faz o socket_cliente da estrutura receber o socket do cliente passado por parametro */
	strcpy(temp_fim -> proximo -> nome_cliente,temp_nome_cliente); /* Copia o nome passado por parametro para a estrutura nome_cliente*/
	temp_fim = temp_fim -> proximo; /* faz o fim receber o endereco do proximo do temp do cliente */
	return(temp_fim); /* Retorna o endereco do novo fim */
}
/* Funcao responsavel por criar um novo cliente */
cliente* cria_cliente(int temp_socket_cliente,char temp_nome_cliente[30]) /* Recebe como parametro o socket e o nome do cliente */
{
	sem_wait(&var_sem); /* Bloqueia todas as variaveis compartilhadas usadas na funcao */
	cliente *temp;	
	if(ini == NULL) /* Caso nao tiver clientes cadastrados chama incia_lista_clientes pasasdo o socket do cliente e o nome do cliente para criar o primeiro novo cliente */
	{
	        ini = incia_lista_clientes(temp_socket_cliente,temp_nome_cliente);
		fim = ini; /* faz o inicio apontar para o fim */
	}
	else /* Caso ja existir outros clientes cadastrados , chama insere_cliente passando o fim o socket do cliente e o nome do cliente para criar um novo cliente*/
	{
		fim = insere_cliente_lista(fim,temp_socket_cliente,temp_nome_cliente); /* Atualiza o fim que recebe o retorno de insere_cliente_lista */
	}
	sem_post(&var_sem); /* Desbloqueia as variaveis compartilhadas usandas na funcao */
	return(fim); /* Retorna o fim */
}
/* Funcao responsavel por excluir cliente */
void exclui_cliente(cliente *temp_cliente) /* Recebe como parametro o endereco do cliente a ser removido */
{
	sem_wait(&var_sem); /* Bloqueia todas as variaveis compartilhadas usadas pela funcao */
	cliente *temp_cliente_1;
	cliente *temp_cliente_2;
	temp_cliente_1 = temp_cliente -> anterior; /* Variavel temp_cliente_1 recebe o anterior do cliente */
	temp_cliente_2 = temp_cliente -> proximo; /* Variavel temp_cliente_2 recebe o proximo do cliente */
	if(temp_cliente_1 == NULL && temp_cliente_2 == NULL) /* Caso o cliente anterior e o proximo nao existirem (cliente eh unico na lista) */
	{
		desaloca_memoria(temp_cliente); /* Desaloca o cliente */
		temp_cliente = NULL; /* O ponteiro do cliente recebe NULL */
 		ini = NULL; /* O inicio recebe NULL */
		fim = NULL; /* O fim recebe NULL*/
	}	
	else if(temp_cliente_1 != NULL && temp_cliente_2 == NULL) /* Caso o cliente anterior existir e o proximo nao existir (cliente tem um outro cliente cadastrado a esquerda)*/
	{
		temp_cliente_1 -> proximo = NULL; /*O proximo do cliente recebe NULL */
		fim = temp_cliente_1; /* O fim recebe o temp_cliente_1 */
		desaloca_memoria(temp_cliente); /* O cliente eh desalocado */
		temp_cliente = NULL; /* o temp_cliente recebe NULL */
	}
	else if(temp_cliente_1 != NULL && temp_cliente_2 != NULL) /* Caso o cliente anterior existir e o proximo tambem (Existem clientes cadastrandos tanto anets como depois na lista)*/
	{
		temp_cliente_1 -> proximo = temp_cliente -> proximo; /* O proximo do temp_cliente_1 vai receber o proximo do temp_cliente */
		desaloca_memoria(temp_cliente); /* Temp_cliente eh desalocado */
		temp_cliente = NULL; /* Temp_cliente recebe NULL */
	}
	else if(temp_cliente_1 == NULL && temp_cliente_2 != NULL) /* Caso o cliente anterior nao existir e o cliente proximo existir quer dizer que o cliente eh o primeiro da lista*/
	{
		temp_cliente_2 -> anterior = NULL; /* Cliente proximo posicao anterior vai receber NULL */
		ini = temp_cliente -> proximo; /* O inicio vai receber o temp_cliente proximo */
		desaloca_memoria(temp_cliente); /* Temp_cliente vai ser desalocado */
		temp_cliente = NULL; /* Temp_cliente vai receber NULL */
	}
	sem_post(&var_sem); /* Desbloqueia as variaveis compartilhadas bloqueadas por sem_wait */
}
/* Funcao responsavel por fazer o broadcast entre os clientes */
int replica_mensagem(cliente *client)
{
	cliente *temp_cliente;
	temp_cliente = ini; /* A variavel temporaria recebe o inicio */
	while(temp_cliente != fim) /* Enquando a temporaria for diferente do fim ele deve executar enviando em broadcast o nome e a mensagem aos clientes */
	{
		if(temp_cliente == ini) /* Caso a temp for igual ao inicio */
		{
			if((strcmp(temp_cliente -> nome_cliente,client -> nome_cliente) != 0) && temp_cliente -> socket_cliente != 0) /* Verifica se os nomes sao diferentes e se o socket_cliente eh diferente de zero(cliente esta vivo) */
			{
				send(temp_cliente -> socket_cliente,client ->mensagem,sizeof(client->mensagem),0); /* Envia a mensagem a ser replicada ao clientes */
				send(temp_cliente -> socket_cliente,client ->nome_cliente,sizeof(client->nome_cliente),0); /* Envia o nome a ser replicado aos clientes */
			}		
		}
		temp_cliente = temp_cliente -> proximo; /* temp_cliente recebe o temp_cliente proximo fazendo com que seja possivel avancar na lista */
		if((strcmp(temp_cliente -> nome_cliente,client -> nome_cliente) != 0) && temp_cliente->socket_cliente != 0) /* Verifica se os nomes sao diferentes e se o socket_cliente eh diferente de zero(cliente esta vivo) */
		{
			send(temp_cliente -> socket_cliente,client -> mensagem,sizeof(client->mensagem),0); /* Envia a mensagem a ser replicada ao clientes */
			send(temp_cliente -> socket_cliente,client -> nome_cliente,sizeof(client->nome_cliente),0); /* Envia o nome a ser replicado aos clientes */
		}
	}
}
/* Funcao que implementa a thread chat que escuta a mensagem do cliente e replica a todos os outros */
void chat(int temp_socket_cliente) /* Recebe como parametro o socket do cliente */
{
	int retorno;
	char temp_nome_cliente[30];
	cliente *cliente;
	do /* Responsavel por validar o nome do cliente */
	{ 
		retorno = recv(temp_socket_cliente,temp_nome_cliente,256,0); /* Recebe o nome informado pelo usuario */
		if(retorno < 0){tratamento_de_erros(1);} /* Caso o retorno seja < 0 informa que nao foi possivel receber a informacao do cliente */
		if(retorno == 0) /* Caso o retorno seja == 0 o cliente se desconectou, assim a thread eh finalizada */
		{
		        tratamento_de_erros(7);
		        pthread_exit(NULL);
		}
		if(valida_nick(temp_nome_cliente) == 0) /* Chama a valida nick mandando o nome informado como parametro */
		{
			retorno = send(temp_socket_cliente,"1\n",3,0); /* Caso o retorno seja zero ele  manda 0 ao cliente dizendo que o nome informado jah existe cadastrado */
			if(retorno < 0){tratamento_de_erros(2);} /* Caso o retorno for < 0 indica que nao foi possivel enviar o dado ao cliente */
			continue; /* Chama continue para receber novamente o nick do usuario */
		}
		else
		{
			retorno = send(temp_socket_cliente,"0\n",3,0); /* Caso for diferente de zero envia 0 dizendo que o nick foi validado pelo servidor*/
			if(retorno < 0){tratamento_de_erros(2);} /* Caso o retorno for < 0 indica que nao foi possivel enviar o dado ao cliente */
			break; /* Para de executar o do-while infinito para comecar a trocar mensagens com o cliente */
		}
	}while(1);
	cliente = cria_cliente(temp_socket_cliente,temp_nome_cliente); /* Cria um novo cliente passando o socket e o nome do cliente*/
	printf("Cliente (%s) conectou\n",cliente -> nome_cliente); /* Informa que o cliente atualmente esta validado no servidor */
	do /* Responsavel por receber a mensagem do usuario e replicar para os outros clientes cadastrados */
	{
		retorno = recv(cliente -> socket_cliente,cliente -> mensagem,256,0); /* Recebe a mensagem informada pelo usaurio */
		if(retorno == 0) /* Se o retorno for zero indica que o cliente desconectou */
		{
			printf("Cliente (%s) desconectou\n",cliente -> nome_cliente); /* Informa que cliente desconectou */
			exclui_cliente(cliente); /* Exclui cliente da lista*/
			pthread_exit(NULL); /* Desliga a thread */
		}
		else if(retorno < 0) /* Caso o retorno for menor que zero informa que nao foi possivel receber a mensagem do cliente */
		{
			tratamento_de_erros(1);
		}
		replica_mensagem(cliente); /* Replica aos outos clientes (Broadcast) a mensagem informada pelo cliente */	
	}while(1);
}
int main (int argc, char *argv[])
{
        sem_init(&var_sem,0,1);
        int socket_servidor,socket_cliente,tam_end_int_cli,numero_da_porta,cont_cli;
        struct sockaddr_in end_int_cli,end_int_serv;
        pthread_t thread;
        if(argc < 2)
        {
                tratamento_de_erros(3); /* Tratamento de erro caso falte parametros */
                exit(1);
        }
        if((socket_servidor = socket(AF_INET, SOCK_STREAM, 0)) < 0) /* Responsavel por criar o socket */
        {
                tratamento_de_erros(4); /* Tratamento de erros caso o socket nao seja criado */
                exit(1); /* Para o servidor por nao ter socket */
        }
        numero_da_porta = atoi(argv[1]); /* Pega o numero da porta informado por parametro */
        bzero((char *)&end_int_serv,sizeof(end_int_serv));
        end_int_serv.sin_family = AF_INET; /* Estabelece os campos do endereco de internet do servidor */
        end_int_serv.sin_addr.s_addr = htonl(INADDR_ANY);
        end_int_serv.sin_port = htons(numero_da_porta);
        if(bind(socket_servidor,(struct sockaddr *)&end_int_serv,sizeof(end_int_serv)) < 0) /* Responsavel por ligar endereco ao socket */
        {
                tratamento_de_erros(5); /* Tratamento de erros caso nao seja posivel ligar endereco ao socket */
                exit(1); /* Para o servidor por nao conseguir ligar endereco ao socket */
        }
        tam_end_int_cli = sizeof(end_int_cli); /* Captura o tamanho do endereco do cliente */
        printf("Servidor Inicializado !!\n"); /* Informa que o servidor esta conectado */
        cont_cli = 1; /* Contador de clientes para listen */    
        while (1) /* While responsavel por implementar o accept de novos clientes ao servidor */
        {
                listen(socket_servidor,cont_cli); /* Define o numero de chamadas que o sistema pode ouvir */
                socket_cliente = accept(socket_servidor,(struct sockaddr *)&end_int_cli,&tam_end_int_cli); /* Aceita novas conexoes de clientes ao servidor*/
                if(socket_cliente < 0) /* Caso o socket_cliente for < 0 quer dizer que nao foi possivel conectar o cliente*/
                {
                        tratamento_de_erros(6);
                } 
                else /* Caso contrario ele inicia uma thread cliente passado como parametro o socket do cliente */
                {
                        pthread_create(&thread,NULL,(void *)&chat,(void *)socket_cliente); 
                        cont_cli++; /* Incrementa o contador de clientes para que listen possa entao escutar agora mais um cliente */
                }
        }
        close(socket_servidor); /* Fecha o socket do servidor */
}
