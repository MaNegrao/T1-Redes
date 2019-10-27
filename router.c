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

void menu(){ //função menu
	sleep(3);
	system("clear");
		printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t┃                           Roteador %02d                        ┃\n", id_router+1);
		printf("\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t┃                     ➊ ─ Enviar mensagem ─ ➊                  ┃\n");
		printf("\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t┃               ➋ ─ Ver historico de mensagens ─ ➋             ┃\n");
		printf("\t┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
		printf("\t┃                         ⓿ ─ Sair ─ ⓿                         ┃\n");
		printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
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
		die("\t Não foi possivel abrir o arquvio de configuração dos roteadores! ");

	for (int i = 0; fscanf(config_file, "%d %d %s", &router[i].id, &router[i].port, router[i].ip) != EOF; i++);
	fclose(config_file);

	printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
	printf("\t┃                  Informações do Roteador %02d                  ┃\n", id_router+1);
	printf("\t┣━━━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");    
	printf("\t┃ ID Roteador ┃   Porta   ┃           Endereço de IP           ┃\n");
	printf("\t┣━━━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	printf("\t┃     %02d      ┃  %6d   ┃  %32s  ┃\n", router[id_router].id,  router[id_router].port,  router[id_router].ip);
	printf("\t┗━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
	sleep(2);

	if((router_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("\t Erro ao criar socket! ");

	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(router[id_router].port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(router_socket, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
		die("\t Erro ao conectar o socket a porta! ");
}

void show_messages(){
	for(int i = 0; i < qtd_message_in; i++){
		printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t┃      Historico de mensagens recebidas pelo roteador %02d        ┃\n", id_router+1);
		printf("\t┣━━━━━━━━━━━┳━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");    
		printf("\t┃ ID Origem ┃ Nº MSG ┃                 Mensagem                 ┃\n");
		printf("\t┗━━━━━━━━━━━┻━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
		printf("\t      %d         %02d     %40s \n", router[id_router].msg_in[i].origin+1, router[id_router].msg_in[i].num_pack, router[id_router].msg_in[i].content);
	}
}

void forward_message(int next_id, Package msg_out){

}


void send_message(int next_id, Package msg_out){//função que enviar mensagem
	int timeouts = 0;
	printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
	printf("\t┃ Enviando mensagem n°%02d para o roteador de ID %02d...           ┃\n", qtd_message, next_id+1);
	printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
	
	si_other.sin_port = htons(router[next_id].port); //enviando para o socket

	if(inet_aton(router[next_id].ip, &si_other.sin_addr) == 0)
		die("\t Erro ao tentar encontrar o IP destino inet_aton() ");

	else{
		do{
			if(msg_out.origin == id_router)
				router[id_router].waiting_ack = TRUE;
			
			if(sendto(router_socket, &msg_out, sizeof(msg_out), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
				die("\t Erro ao enviar a mensagem! sendto() ");

			int cont = 1;

			while(cont++ != TIMEOUT_MS && router[id_router].waiting_ack){
				if(!router[id_router].waiting_ack)
					break;
				else
					sleep(5);
			}

			if(router[id_router].waiting_ack){
				timeouts++;
				printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
				printf("\t┃ Tempo de envio esgotado, tentando enviar novamente...        ┃\n");
				printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
			}
		}while(timeouts < TIMEOUT_MAX && router[id_router].waiting_ack);

		if(!router[id_router].waiting_ack){
			printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
			printf("\t┃ ACK Confirmada! A mensagem foi recebida pelo destinatário... ┃\n");
			printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
		}
		else{
			if(msg_out.origin == id_router)
				router[id_router].waiting_ack = FALSE;
			printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
			printf("\t┃ Envio de mensagem cancelado! Número de tentativas excedido.. ┃\n");
			printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
		}
		sleep(6);
		menu();
	}	
}

void create_message(){//função cria mensagem
	int destination, next_id;
	Package msg_out; 

	do{ //verificação do roteador destino
		printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
		printf("\t┃ Digite o ID do roteador para enviar a mensagem...            ┃\n");
		printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n\t ");
		scanf("%d", &destination);
		destination = destination - 1;
		if(destination < 0 || destination >= N_ROT){
			printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
			printf("\t┃ O ID informado é inválido. Por favor digite novamente...     ┃\n");
			printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n\t ");
		}
	}while(destination < 0 || destination >= N_ROT);

	printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
	printf("\t┃ Digite o conteudo da mensagem a ser enviada para o roteador: ┃\n");
	printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n\t  ");

	__fpurge(stdin); //limpar o buffer de algum lixo de memória
	fgets(router[id_router].msg_out[qtd_message].content, MSG_SIZE, stdin);

	router[id_router].msg_out[qtd_message].num_pack = qtd_message;
	router[id_router].msg_out[qtd_message].origin = id_router;
	router[id_router].msg_out[qtd_message].dest = destination;
	router[id_router].msg_out[qtd_message].ack = FALSE;

	next_id = router_table.path[destination]; //quem vai receber a mensagem

	msg_out = router[id_router].msg_out[qtd_message];
	qtd_message++; //atualiza a quantidade de mensagem que foram enviadas

	send_message(next_id, msg_out);
}

void *sender(void *data){ //função da thread sender - transmissor
	int op;
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_addr.s_addr =  htonl(INADDR_ANY);

	while(1){
		menu();
		scanf("%d", &op);
		switch(op){
			case 0: //sair
				exit(0);
				break;
			case 1: //enviar mensagem
				create_message();
				break;
			case 2: //ver mensagens anteriores
				show_messages();
				break;
			default:
				printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
				printf("\t┃ Opção inválida, digite novamete.. ┃\n");
				printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n\t  ");
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
    
		if((recvfrom(router_socket, &message_in, sizeof(message_in), 0, (struct sockaddr *) &si_me, &slen)) == -1)
			die("\tErro ao receber mensagem! recvfrom() ");

		if(message_in.dest == id_router){
			if(!message_in.ack){
				printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
				printf("\t┃ Mensagem Nº %02d recebido do roteador com ID %02d...             ┃\n", message_out.num_pack+1, message_out.origin+1);
				printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
				printf("\t Mensagem: %50s \n", message_in.content);
				
				strcpy(router[id_router].msg_in[qtd_message_in].content, message_in.content);
				router[id_router].msg_in[qtd_message_in].num_pack = message_in.num_pack;
				router[id_router].msg_in[qtd_message_in].origin = message_in.origin;
				qtd_message_in++;

				Package ack_reply;
				ack_reply.origin = message_in.dest;
				ack_reply.dest = message_in.origin;
				ack_reply.ack = TRUE;

				si_other.sin_port = htons(router[ack_reply.dest].port); //enviando para o socket

				if(sendto(router_socket, &ack_reply, sizeof(ack_reply), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
					die("\tErro ao enviar a mensagem! sendto() ");
			}
			else if(router[id_router].waiting_ack)
				router[id_router].waiting_ack = FALSE;
		}
		else{
			message_out = message_in;
			next = router_table.path[message_out.dest];
			printf("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
			printf("\t┃ Encaminhado mensagem do roteador %02d para o roteador %02d...    ┃\n", id_router+1, next+1);
			printf("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
			sleep(2);
			send_message(next, message_out);
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
	printf("\t┏━━━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("\t┃   Vértice   ┃ Anterior  ┃   Custo   ┃      Menor Caminho     ┃\n");
    printf("\t┣━━━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
    for(int i = 0; i < N_ROT; i++){
   		printf("\t┃      %d      ┃     %d     ┃ %5d     ┃", i +1, dijkstra_info[i].prev+1, dijkstra_info[i].cost);
        print_dijkstra_line(dijkstra_info, i);
        printf("\n");
    }
    printf("\t┗━━━━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━┛\n\n");
	sleep(3);
}

int main(int argc, char *argv[]){
	int links_table[N_ROT][N_ROT];
	mat_dijkstra dijkstra_info[N_ROT];

	//faz uma comparação com o que veio de parametro no comando executável
	if(argc < 2)
		die("\tDigite o ID do roteador! ");
	else if(argc > 2)		
		die("\tDigite apenas um ID para o roteador! ");

	id_router = strtol(argv[1], NULL, 10) - 1; //função de casting do argv id para int 
	
	if(id_router >= N_ROT)
		die("\tID do roteador inválido! ");

	memset(links_table, -1, sizeof(int) * N_ROT * N_ROT); //limpa a tabela router

	read_links(links_table); //função que lê do arquivo enlaces.config

	inicializa_dijkstra(dijkstra_info); // inicializa matriz djkistra
	dijkstra_info[id_router].cost = 0;
    dijkstra(links_table, dijkstra_info, id_router); // algoritimo dijkstra recursivo
	
	for(int i = 0; i < N_ROT; i++)
		define_path(dijkstra_info, i, i);
	pathcost(links_table);

	create_router(); //função que lê e cria os roteadores do arquivo roteadores.config

	print_dijkstra(dijkstra_info);

	pthread_create(&receiver_thread, NULL, receiver, NULL); //terceiro parametro é a função que a thread ira rodar
	pthread_create(&sender_thread, NULL, sender, NULL);

	pthread_join(receiver_thread, NULL);
	pthread_join(sender_thread, NULL);

	return 0;
}
    