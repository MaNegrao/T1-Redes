#include "router.h"

Router router[N_ROT];
Table router_table[N_ROT];

pthread_t receiver_thread, sender_thread;
int router_socket, id_router;
struct sockaddr_in si_me, si_other;

void die(char *s){ //função que retorna os erros que aconteçam na execução e encerra
	perror(s);
	exit(1);
}

void inicializa_djikstra(mat_djikstra info[]) {
    for (int i = 0; i < N_ROT; i++) {
        info[i].visit = 0;
        info[i].cost = 10000;
        info[i].prev = -1;
    }
}

int menor(mat_djikstra info[], int *para) {
    int menorIndice, i, menorcost = 1000;

    for(i = 0; i < N_ROT; i++){ 
        if(info[i].visit == 0){ 
            break;
        }
    }    

    if(i == N_ROT){
        *para = 1;
        return 0;
    }

    for(i = 0; i < N_ROT; i++){
        if((info[i].cost < menorcost) && (info[i].visit == 0)){
            menorcost = info[i].cost;
            menorIndice = i;
        }
    }

    return menorIndice;
}

void dijkstra(int grafo[N_ROT][N_ROT], mat_djikstra info[], int vertice){
    // marcar vértice como visitado
    info[vertice].visit = 1;

    //percorrer para registrar cost e vértice anterior
    for(int i = 0; i < N_ROT; i++){
        if((grafo[vertice][i] > 0) && (info[i].visit == 0) && (info[vertice].cost + grafo[vertice][i] < info[i].cost)){
			info[i].cost = grafo[vertice][i] + info[vertice].cost;
			router_table[i].cost[vertice] = info[i].cost;
            info[i].prev = vertice;
        }
    }

    // repetir o processo para o menor filho não visitado
    int para = 0, prox = menor(info, &para);
    if(!para) dijkstra(grafo, info, prox);
}

/*
void pathcost(mat_djikstra info[], int tab[N_ROT][N_ROT]){
	for(int i = 0; i < N_ROT; i++){
		for(int j = 0; j < N_ROT; j++){
			router_table[i].cost[j] = info[i].cost;
		}
	}
}*/

void read_links(int tab[N_ROT][N_ROT]){ //função que lê os enlaces
  int x, y, cost;
  FILE *file = fopen("enlaces.config", "r");

  if (file){
    for (int i = 0; fscanf(file, "%d %d %d", &x, &y, &cost) != EOF; i++){
	  tab[x-1][y-1] = cost;
      tab[y-1][x-1] = cost;
    }
    fclose(file);
  }
} 

void create_router(){ //função que cria os sockets para os roteadores
	FILE *config_file = fopen("roteadores.config", "r");

	if(!config_file)
		die("Não foi possivel abrir o arquvio de configuração dos roteadores!\n");

	for (int i = 0; fscanf(config_file, "%d %d %s", &router[i].id, &router[i].port, router[i].ip) != EOF; i++);
	fclose(config_file);

	printf("\t┏━━━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("\t┃ ID Roteador ┃   Porta   ┃           Endereço de IP           ┃\n");
	printf("\t┣━━━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	printf("\t┃     %02d      ┃  %6d   ┃  %32s  ┃\n", router[id_router-1].id,  router[id_router-1].port,  router[id_router-1].ip);
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

void menu(){ //função menu
		system("clear");
		printf("\t\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t\t┃           Roteador %02d           ┃\n", id_router);
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
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_addr.s_addr =  htonl(INADDR_ANY);

	sleep(2);
	while(1){
		menu();
		scanf("%d", &op);
		switch(op){
			case 0: //sair
				exit(0);
				break;
			case 1: //enviar mensagem
				printf("joia\n");
				sleep(1);
				break;
			case 2: //ver mensagens anteriores
				break;
			default:
				printf("Opção inválida!\n");
				break;
		}
	}
}

void *receiver(void *data){ //função da thread receiver
	
	while(1){
		break;
	}
}


int main(int argc, char *argv[]){
	int links_table[N_ROT][N_ROT];
	mat_djikstra info[N_ROT];

	pthread_create(&receiver_thread, NULL, receiver, NULL);
	pthread_create(&sender_thread, NULL, sender, NULL);

	//faz uma comparação com o que veio de parametro no comando executável
	if(argc < 2)
		die("Digite o ID do roteador!\n");
	else if(argc > 2)		
		die("Digite apenas um ID para o roteador!\n");

	id_router = strtol(argv[1], NULL, 10); //função de casting do argv id para int 
	if(id_router >= N_ROT){
		die("ID do roteador inválido!\n");
	}

	memset(links_table, -1, sizeof(int) * N_ROT * N_ROT); //limpa a tabela router

	read_links(links_table); //função que lê do arquivo enlaces.config

	inicializa_djikstra(info); // inicializa matriz djkistra
	info[0].cost = 0;
    dijkstra(links_table, info, 0); // algoritimo djikstra recursivo

	for(int i = 0; i < N_ROT; i++){
		for(int j = 0; j < N_ROT; j++){
			printf("%d\n", router_table[i].cost[j]);
		}
	}

	exit(0);

	create_router(); //função que lê e cria os roteadores do arquivo roteadores.config

	pthread_join(sender_thread, NULL);
	pthread_join(receiver_thread, NULL);


	return 0;
}