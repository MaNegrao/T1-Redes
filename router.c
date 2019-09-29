#include "router.h"

Router router[N_ROT];
Table router_table[N_ROT];

pthread_t receiver_thread, sender_thread;
int router_socket, id_router;
struct sockaddr_in si_me, si_other;

int qtd_message = 0;

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

void dijkstra(int graph[N_ROT][N_ROT], mat_djikstra info[], int vertex){
    // marcar vértice como visitado
    info[vertex].visit = 1;

    //percorrer para registrar custo e vértice anterior
    for(int i = 0; i < N_ROT; i++){
        if((graph[vertex][i] > 0) && (info[i].visit == 0) && (info[vertex].cost + graph[vertex][i] < info[i].cost)){
			info[i].cost = graph[vertex][i] + info[vertex].cost;
            info[i].prev = vertex;
        }
    }

    // repetir o processo para o menor filho não visitado
    int para = 0, prox = menor(info, &para);
    
	if(!para)
		dijkstra(graph, info, prox);
	
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

void create_message(){
	int destination;

	do{ //verificação do roteador destino
	printf("Digite o roteador destino:\n");
	scanf("%d", &destination);
	if(destination < 0 || destination >= N_ROT)
		printf("O numero do roteador informado não existe. Por favor digite novamente!\n");
	}while(destination < 0 || destination >= N_ROT);

	printf("Digite a mensagem a ser enviada para o roteador %d:\n", destination);
	getchar(); //limpar o buffer de algum lixo de memória
	fgets(router[id_router].msg_out[qtd_message].content, MSG_SIZE, stdin);

	router[id_router].msg_out[qtd_message].num_pack = qtd_message;
	router[id_router].msg_out[qtd_message].origin = id_router;
	router[id_router].msg_out[qtd_message].dest = destination;
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
	int op;
	
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
				create_message();
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

void print(mat_djikstra info[], int indice){
    if(info[indice].prev > 0){
        print(info, info[indice].prev);
    }
    printf(" %d ", indice+1);
}

int main(int argc, char *argv[]){
	int links_table[N_ROT][N_ROT];
	mat_djikstra info[N_ROT];

	pthread_create(&receiver_thread, NULL, receiver, NULL); //terceiro parametro é a função que a thread ira rodar
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
	info[id_router-1].cost = 0;
    dijkstra(links_table, info, id_router-1); // algoritimo djikstra recursivo

	printf("\t┏━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━┓\n");
    printf("\t┃ Vértice ┃ Anterior  ┃   Custo   ┃  Menor Caminho  ┃\n");
    printf("\t┣━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫\n");
    for(int i = 0; i < N_ROT; i++){
        printf("\t┃    %d    ┃     %d     ┃ %5d     ┃", i +1, info[i].prev+1, info[i].cost);
        print(info, i);
        printf("\n");
    }
    printf("\t┗━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━┛\n");

	exit(0);

	create_router(); //função que lê e cria os roteadores do arquivo roteadores.config

	pthread_join(sender_thread, NULL);
	pthread_join(receiver_thread, NULL);


	return 0;
}