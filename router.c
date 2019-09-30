#include "router.h"

Router router[N_ROT];
Table router_table;

pthread_t receiver_thread, sender_thread;
int router_socket, id_router, qtd_message = 0, qtd_message_in = 0;
struct sockaddr_in si_me, si_other;

void die(char *s){ //função que retorna os erros que aconteçam na execução e encerra
	perror(s);
	exit(1);
}

void inicializa_dijkstra(mat_dijkstra dijkstra_info[]) {
    for (int i = 0; i < N_ROT; i++) {
        dijkstra_info[i].visit = 0;
        dijkstra_info[i].cost = 10000;
        dijkstra_info[i].prev = -1;
    }
}

int menor(mat_dijkstra dijkstra_info[], int *para) {
    int menorIndice, i, menorcost = 1000;

    for(i = 0; i < N_ROT; i++){ 
        if(dijkstra_info[i].visit == 0){ 
            break;
        }
    }    

    if(i == N_ROT){
        *para = 1;
        return 0;
    }

    for(i = 0; i < N_ROT; i++){
        if((dijkstra_info[i].cost < menorcost) && (dijkstra_info[i].visit == 0)){
            menorcost = dijkstra_info[i].cost;
            menorIndice = i;
        }
    }

    return menorIndice;
}

void dijkstra(int graph[N_ROT][N_ROT], mat_dijkstra dijkstra_info[], int vertex){
    // marcar vértice como visitado
    dijkstra_info[vertex].visit = 1;

    //percorrer para registrar custo e vértice anterior
    for(int i = 0; i < N_ROT; i++){
        if((graph[vertex][i] > 0) && (dijkstra_info[i].visit == 0) && (dijkstra_info[vertex].cost + graph[vertex][i] < dijkstra_info[i].cost)){
			dijkstra_info[i].cost = graph[vertex][i] + dijkstra_info[vertex].cost;
            dijkstra_info[i].prev = vertex;
        }
    }

    // repetir o processo para o menor filho não visitado
    int para = 0, prox = menor(dijkstra_info, &para);
    
	if(!para)
		dijkstra(graph, dijkstra_info, prox);
}	

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
	printf("\t┃     %02d      ┃  %6d   ┃  %32s  ┃\n", router[id_router].id,  router[id_router].port,  router[id_router].ip);
	printf("\t┗━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");

	if((router_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("Erro ao criar socket!\n");

	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(router[id_router].port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(router_socket, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
		die("Erro ao conectar o socket a porta!\n");
}

void show_messages(){
	for(int i = 0; i < qtd_message_in; i++){
		printf("Mensagem #%d recebida de %d\n", router[id_router].msg_in[i].num_pack, router[id_router].msg_in[i].origin+1);
		printf(" --- %s", router[id_router].msg_in[i].content);
	}
}

void send_message(int next_id, Package msg_out){//função que enviar mensagem
	printf("Enviando pacote para o roteador de ID %d\n", next_id+1);
	sleep(1);

	si_other.sin_port = htons(router[next_id].port); //enviando para o socket

	if(inet_aton(router[next_id].ip, &si_other.sin_addr) == 0)
		die("Erro ao tentar encontrar o IP destino\n");
	else{
		if(sendto(router_socket, &msg_out, sizeof(msg_out), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
			die("Erro ao enviar a mensagem!\n");
		else
			printf("O roteador %d está enviando a mensagem de numero %d para o roteador de ID %d\n", msg_out.origin+1, msg_out.num_pack+1, msg_out.dest+1);		
	}
}

void create_message(){//função cria mensagem
	int destination, next_id;
	Package msg_out; 

	do{ //verificação do roteador destino
		printf("Digite o roteador destino:\n");
		scanf("%d", &destination);
		destination = destination - 1;
		if(destination < 0 || destination >= N_ROT)
			printf("O numero do roteador informado não existe. Por favor digite novamente!\n");
	}while(destination < 0 || destination >= N_ROT);

	printf("Digite a mensagem a ser enviada para o roteador %d:\n", destination+1);
	__fpurge(stdin); //limpar o buffer de algum lixo de memória
	fgets(router[id_router].msg_out[qtd_message].content, MSG_SIZE, stdin);

	router[id_router].msg_out[qtd_message].num_pack = qtd_message;
	router[id_router].msg_out[qtd_message].origin = id_router;
	router[id_router].msg_out[qtd_message].dest = destination;

	next_id = router_table.path[destination]; //quem vai receber a mensagem

	msg_out = router[id_router].msg_out[qtd_message];
	qtd_message++; //atualiza a quantidade de mensagem que foram enviadas

	send_message(next_id, msg_out);
}

void menu(){ //função menu
		system("clear");
		printf("\t\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t\t┃           Roteador %02d           ┃\n", id_router+1);
		printf("\t\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t\t┃ ➊ ─ Enviar mensagem             ┃\n");
		printf("\t\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t\t┃ ➋ ─ Ver mensagens anteriores    ┃\n");
		printf("\t\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t\t┃ ⓿ ─ Sair                        ┃\n");
		printf("\t\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n\t\t  ");
}

void *sender(void *data){ //função da thread sender - transmissor
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
				show_messages();
				break;
			default:
				printf("Opção inválida!\n");
				break;
		}
	}
}

void *receiver(void *data){ //função da thread receiver
	int slen = sizeof(si_other);
	int next;

	while(1){
		Package message_in = router[id_router].msg_in[qtd_message_in];
		Package message_out = router[id_router].msg_out[qtd_message];

		if((recvfrom(router_socket, &message_in, sizeof(message_in), 0, (struct sockaddr *) &si_me, &slen)) == -1){
			printf("Erro ao receber mensagem!\n");
			qtd_message_in--;
		}
		else{
			if(message_in.dest == id_router){
				printf("Mensagem recebida do roteador %d!\n", message_in.origin+1);
				printf("Mensagem: %s\n", message_in.content);
				strcpy(router[id_router].msg_in[qtd_message_in].content, message_in.content);
				router[id_router].msg_in[qtd_message_in].num_pack = message_in.num_pack;
				router[id_router].msg_in[qtd_message_in].origin = message_in.origin;
				qtd_message_in++;
			}
			else{
				message_out = message_in;
				next = router_table.path[message_out.dest];
				printf("Retransmitindo de %d para %d\n", id_router+1, next+1);

				send_message(next, message_out);
			}
		}
	}
}

void define_path(mat_dijkstra dijkstra_info[], int indice, int position){
	if(dijkstra_info[indice].prev > -1){	
		if(dijkstra_info[indice].prev == id_router){
			router_table.path[position] = indice;	
			return;	
		}
        define_path(dijkstra_info, dijkstra_info[indice].prev, position);
    }
	if(dijkstra_info[indice].prev == -1){
		router_table.path[position] = position;
	}
}

void pathcost(int tab[N_ROT][N_ROT]){
	for(int i = 0; i < N_ROT; i++){
		router_table.cost[i] = tab[id_router][router_table.path[i]];
	}
}

void print_dijkstra_line(mat_dijkstra dijkstra_info[], int indice){
	if(dijkstra_info[indice].prev > -1){
        print_dijkstra_line(dijkstra_info, dijkstra_info[indice].prev);
    }
    printf(" %d ", indice+1);
}

void print_dijkstra(mat_dijkstra dijkstra_info[]){
	printf("\t┏━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━┓\n");
    printf("\t┃ Vértice ┃ Anterior  ┃   Custo   ┃  Menor Caminho  ┃\n");
    printf("\t┣━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┛\n");
    for(int i = 0; i < N_ROT; i++){
        printf("\t┃    %d    ┃     %d     ┃ %5d     ┃", i +1, dijkstra_info[i].prev+1, dijkstra_info[i].cost);
        print_dijkstra_line(dijkstra_info, i);
        printf("\n");
    }
    printf("\t┗━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━┛\n\n");
}

int main(int argc, char *argv[]){
	int links_table[N_ROT][N_ROT];
	mat_dijkstra dijkstra_info[N_ROT];

	//faz uma comparação com o que veio de parametro no comando executável
	if(argc < 2)
		die("Digite o ID do roteador!\n");
	else if(argc > 2)		
		die("Digite apenas um ID para o roteador!\n");

	id_router = strtol(argv[1], NULL, 10) - 1; //função de casting do argv id para int 
	
	if(id_router >= N_ROT)
		die("ID do roteador inválido!\n");

	memset(links_table, -1, sizeof(int) * N_ROT * N_ROT); //limpa a tabela router

	read_links(links_table); //função que lê do arquivo enlaces.config

	inicializa_dijkstra(dijkstra_info); // inicializa matriz djkistra
	dijkstra_info[id_router].cost = 0;
    dijkstra(links_table, dijkstra_info, id_router); // algoritimo dijkstra recursivo
	
	for(int i = 0; i < N_ROT; i++)
		define_path(dijkstra_info, i, i);
	pathcost(links_table);

	print_dijkstra(dijkstra_info);

	create_router(); //função que lê e cria os roteadores do arquivo roteadores.config

	pthread_create(&receiver_thread, NULL, receiver, NULL); //terceiro parametro é a função que a thread ira rodar
	pthread_create(&sender_thread, NULL, sender, NULL);

	pthread_join(receiver_thread, NULL);
	pthread_join(sender_thread, NULL);

	return 0;
}