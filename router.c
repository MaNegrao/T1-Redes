#include "router.h"

pthread_t receiver_thread, sender_thread;
int id;

//função que retorna os erros que aconteçam na execução e a encerra
void die(char *s){
	perror(s);
	exit(1);
}

//função auxiliar para as configurações iniciais
set_config inicializa_protocolo(int id){
	set_config config;
	config.id = id;
	return config;
}

//apenas uma função de menu
void menu(int id_router){
		system("clear");
		printf("\t\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t\t┃            Router 0%d            ┃\n", id_router);
		printf("\t\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t\t┃ ➊ ─ Enviar mensagem             ┃\n");
		printf("\t\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t\t┃ ➋ ─ Ver mensagens anteriores    ┃\n");
		printf("\t\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t\t┃ ⓿ ─ Sair                        ┃\n");
		printf("\t\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n\t\t  ");
}

void *receiver(void *data){

}

//função da thread sender
void *sender(void *data){
	char buf[BUFLEN];
	int id_destiny, op;

	while(1){
		menu(id);
		scanf("%d", &op);

		switch(op){
			case 1:
				printf("joia\n");
				break;
			default:
				printf("Opção inválida!\n");
				break;
		}
	}
}

int main(int argc, char *argv[]){
	set_config config;

	if(argc < 2)
		die("Insira o ID do roteador!\n");
	else if(argc > 2)		
		die("Insira apenas um ID para o roteador!\n");

	pthread_create(&receiver_thread, NULL, receiver, NULL);
	pthread_create(&sender_thread, NULL, sender, NULL);

	pthread_join(sender_thread, NULL);


	id = strtol(argv[1], NULL, 10); //função de casting do argv id para int 

	//config = inicializa_protocolo(id);

	return 0;
}