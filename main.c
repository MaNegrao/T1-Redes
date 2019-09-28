#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

typedef struct setar_configuracoes{
	int id;
}setar_configuracoes;

//struct dos vertices para o grafo da rede
typedef struct nodo{
	int id;
	int custo;
	struct nodo *next;
}vertex;

//função auxiliar para as configurações iniciais
setar_configuracoes inicializa_protocolo(int ID){
	setar_configuracoes set_config;
	set_config.id = ID;
	return set_config;
}

int main(void){
	int ID;
	setar_configuracoes set_config;

	printf("Digite o identificador do roteador instanciado: \n");
	scanf("%d", &ID);	
	set_config = inicializa_protocolo(ID);

	return 0;
}