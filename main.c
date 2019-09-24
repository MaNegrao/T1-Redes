#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

typedef struct setar_configuracoes{
	int s;
}setar_configuracoes;

setar_configuracoes inicializa_protocolo(int ID){
	setar_configuracoes set_config;
	set_config.s = ID;
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