__Assume-se para este tutorial que o computador configurado possui ambiente Linux e GCC instalado.__

Chat (Na visão de usuário):
========

```
* Servidor (Responsável por atender a requisições de clientes).
* Cilente (Permite conexões entre o servidor).
```

Compilando o código fonte do Servidor/Cliente (GNU GCC):
-----------
```
gcc server.c -lpthread -o servidor
gcc cliente.c -o cliente
```

Executando o servidor
-----------
```
- ./servidor 5533
```

Executando o cliente
-----------
```
- ./cliente IPv4 do servidor 5533
```

Chat (Na visão do desenvolvedor):
========

Visão geral do repositório:
-----------
1. server.c: Funções responsáveis pela criação das seções, conexão do servidor a rede.
2. client.c: Funções que permitem que um cliente se conecte com o servidor (Envie/Receba) mensagens.

Novas funcionalidades (Futuro):
-----------
- Permitir que o cliente se conecte em duas seções diferentes (Uma só para envio e outra só para recebimento).
- Uso de Pthreads para realizar as duas conexões em um único código C.
- Uso de múltiplas interfaces para permitir a escrita/recebimento de dados simultâneos.
