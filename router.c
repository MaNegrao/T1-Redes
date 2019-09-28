#include <router.h>

typedef struct setar_configuracoes{
	int id;
}setar_configuracoes;

//struct dos vertices para o grafo da rede
typedef struct nodo{
	int id;
	int custo;
	struct nodo *next;
}vertex;

//função que retorna os erros que aconteçam na execução e a encerra
void die(char *s){
	perror(s);
	exit(1);
}

//função auxiliar para as configurações iniciais
setar_configuracoes inicializa_protocolo(int id){
	setar_configuracoes set_config;
	set_config.id = id;
	return set_config;
}

int main(int argc, char *argv[]){
	int id;
	setar_configuracoes set_config;

	if(argc < 2)
		die("Insira o ID do roteador!\n");
	else if(argc > 2)
		die("Insira apenas um ID para o roteador!\n");

	id = toint(argv[1]);


	set_config = inicializa_protocolo(id);

	return 0;
}