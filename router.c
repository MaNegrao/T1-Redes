#include "router.h"

pthread_t receiver_thread, sender_thread;
int id_router;

void die(char *s){ //função que retorna os erros que aconteçam na execução e a encerra
	perror(s);
	exit(1);
}

/*set_config inicializa_protocolo(){ //função auxiliar para as configurações iniciais
	set_config config;
	config.id = id_router;
	return config;
}*/

void menu(){ //apenas uma função de menu
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

void *sender(void *data){ //função da thread sender
	char buf[BUFLEN];
	int id_destiny, op;

	while(1){
		menu();
		scanf("%d", &op);
		switch(op){
			case 0:
				exit(0);
				break;
			case 1:
				printf("joia\n");
				sleep(1);
				break;
			case 2:
				break;
			default:
				printf("Opção inválida!\n");
				break;
		}
	}
}

void *receiver(void *data){

}

int main(int argc, char *argv[]){
	int router_table[N_ROT][N_ROT];

	if(argc < 2)
		die("Insira o ID do roteador!\n");
	else if(argc > 2)		
		die("Insira apenas um ID para o roteador!\n");
	id_router = strtol(argv[1], NULL, 10); //função de casting do argv id para int 

	if(id_router >= N_ROT){
		die("ID de roteador inválido!\n");
	}

	memset(router_table, -1, sizeof(int) * N_ROT * N_ROT);

	pthread_create(&receiver_thread, NULL, receiver, NULL);
	pthread_create(&sender_thread, NULL, sender, NULL);

	pthread_join(sender_thread, NULL);

	return 0;
}