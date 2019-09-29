#include "router.h"

Router router[N_ROT];
Table tabela_enl[N_ROT];

pthread_t receiver_thread, sender_thread;
int router_socket, id_router;
struct sockaddr_in si_me, si_other;

void die(char *s){ //função que retorna os erros que aconteçam na execução e a encerra
	perror(s);
	exit(1);
}

void create_router(){
	FILE *config_file = fopen("roteadores.config", "r");

	if(!config_file)
		die("Não foi possivel abrir o arquvio de configuração dos roteadores!\n");

	for (int i = 0; fscanf(config_file, "%d %d %s", &router[i].id, &router[i].port, router[i].ip) != EOF; i++);
	fclose(config_file);

	printf("\t┏━━━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("\t┃ ID Roteador ┃   Porta   ┃           Endereço de IP           ┃\n");
	printf("\t┣━━━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	printf("\t┃     %2d      ┃  %6d   ┃  %32s  ┃\n", router[id_router-1].id,  router[id_router-1].port,  router[id_router-1].ip);
	printf("\t┗━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");

	if((router_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("Erro ao criar socket!\n");

	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(router[id_router-1].port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(router_socket, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
		die("Erro ao conectar o socket a porta!\n");
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

	sleep(3);
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

void carrega_enlaces(int tab[N_ROT][N_ROT]){
  int x, y, peso;
  FILE *file = fopen("enlaces.config", "r");

  if (file){
    for (int i = 0; fscanf(file, "%d %d %d", &x, &y, &peso) != EOF; i++){
      tab[x][y] = peso;
      tab[y][x] = peso;
    }
    fclose(file);
  }
}

int main(int argc, char *argv[]){
	int router_table[N_ROT][N_ROT];
	int tabela_enlaces[N_ROT][N_ROT];

	//faz uma comparação com o que veio de parametro no comando executável
	if(argc < 2)
		die("Digite o ID do roteador!\n");
	else if(argc > 2)		
		die("Digite apenas um ID para o roteador!\n");

	if(id_router >= N_ROT){
		die("ID do roteador inválido!\n");
	}

	id_router = strtol(argv[1], NULL, 10); //função de casting do argv id para int 

	memset(router_table, -1, sizeof(int) * N_ROT * N_ROT); //limpa a tabela router

	memset(tabela_enlaces, -1, sizeof(int) * N_ROT * N_ROT); //limpa a tabela dos enlaces

	carrega_enlaces(tabela_enlaces); //função que lê do arquivo enlaces.config

	create_router();

	sleep(2);

	pthread_create(&receiver_thread, NULL, receiver, NULL);
	pthread_create(&sender_thread, NULL, sender, NULL);

	pthread_join(sender_thread, NULL);


	return 0;
}