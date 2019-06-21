#include "library.h"

#define TAM_HREF 4096

//Construcao da arvore


int porta = 8228; //Definicao da porta padrao
int BUFFER = 16384; //Definicao do tamanho do buffer 


int main(int argc, char *argv[]) {

    FILE *arq_html = NULL, *tree_html = NULL, *arq_request = NULL; //Abertura de arquivos vazias
    //Socket cliente e servidor
    int socket_padrao, novo_socket; 
    char *lista_href[TAM_HREF];
    char *buf = malloc(BUFFER);
    buffer_inicial(lista_href, 16384);
    struct sockaddr_in servidor, cliente; //Uso dos sockets para conexao
    //URL's
    char url[150] = "\0"; //Inicia a URL como NULL (/0)
    char host_url[150] = "\0"; //Inicia a URL como NULL (/0)
    char aux_url[150] = "\0"; //Inicia a URL como NULL (/0)
    unsigned int addr_len; //Inteiros positivos sem sinal

    if(argv[1]){
        if(strcmp(argv[1], "-p") == 0) //Confere se a string com argumento "-p" esta vazia
            if(argv[2] != NULL)
                porta = atoi(argv[2]); //Trata a porta
    }
    //Baseado no arquivo disponibilizado server.cpp

    arvore *head_href = (arvore *) malloc(sizeof(arvore));  // arvore *lista_href = (arvore *)malloc(sizeof(arvore)); 
    initialize_node(head_href); //Inicializa a arvore alocada
    buffer_inicial(buf, BUFFER);
    buffer_inicial(url, 150); //Inicia com 150bytes;
    buffer_inicial(host_url, 150); //Inicia com 150bytes;
    socket_padrao = socket(AF_INET, SOCK_STREAM, 0); //Uso do socket_type.h para criar um novo socket
    if(socket_padrao < 0) {
        perror("Erro ao criar socket do servidor");
    }
    printf("Socket criado com sucesso!");

    //Conexao a porta (dflt = 8228) (Biblioteca in.h e socket.h)
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta); 

    addr_len = sizeof(struct sockaddr_in);

}
