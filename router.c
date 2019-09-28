#include "router.h"

Router router[N_ROT];

pthread_t receiver_thread, sender_thread;
int router_socket, id_router;

void die(char *s){ //função que retorna os erros que aconteçam na execução e a encerra
	perror(s);
	exit(1);
}

void create_router(){
	FILE *config_file = fopen("roteadores.config", "r");

	if(!config_file)
		die("Não foi possivel abrir o arquvio de configuração dos roteadores!\n");

	for (int i = 0; fscanf(config_file, "%d %d %s", &router[i].id, &router[i].port, &router[i].ip) != EOF; i++);
	fclose(config_file);

	printf("\t┏━━━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━┓\n");
    printf("\t┃  Router ID  ┃   Port    ┃     IP      ┃\n");
	printf("\t┣━━━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━┫\n");
	printf("\t┃     %2d      ┃  %6d   ┃  %s  ┃\n", router[id_router-1].id,  router[id_router-1].port,  router[id_router-1].ip);
	printf("\t┗━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━┛\n");



}

void menu(){ //apenas uma função de menu
		system("clear");
		printf("\t\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t\t┃            Router %2d            ┃\n", id_router);
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
	
	while(1){

	}
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

	create_router();

	pthread_create(&receiver_thread, NULL, receiver, NULL);
	pthread_create(&sender_thread, NULL, sender, NULL);

	//pthread_join(sender_thread, NULL);


	return 0;
}