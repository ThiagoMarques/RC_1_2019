#include "library.h"
//***** Funcoes ******//

void parsing(char* buf, char *new_url, char *new_host){ //Realiza o parsing para capturar requisicoes
    char *get = strstr(buf, "GET");
    char *http = strstr(buf, "HTTP/1.1");
    char *host = strstr(buf, "Host:");
    long int i, j = 0;
    long int start_url = get - buf + 4;
    long int end_url = http - buf - 1;
    long int start_host = host - buf + 6;

    for(i=start_url;i<end_url;i++){
        new_url[j] = buf[i];
        j++;
    }
    j = 0;
    while(buf[start_host + j]!= '\n' && buf[start_host + j] != '\r'){
        new_host[j] = buf[start_host + j];
        j++;
    }
}

int get_host_by_name(char *new_url, char *new_host){ //Cria requisicao HTTP com o IP
    struct hostent *hp;
    char request[250];
    struct sockaddr_in cliente;
    bzero(request, 250);
    int on = 1, sock;
    printf("NEW host:%s\n", new_host);
    if ((hp = gethostbyname(new_host)) == NULL){
        herror("gethostbyname");
    }
    bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
    cliente.sin_port = htons(80);
    cliente.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
    if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
        perror("Nao foi possivel conectar.");
        exit(4);
    } if(strcmp(new_url, new_host) == 0) {
        strcat(request, "GET / HTTP/1.1\r\nHost: ");
        strcat(request, new_host);
        strcat(request, "\r\n\r\n");
    }else {
        strcat(request, "GET ");
        strcat(request, new_url);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, new_host);
        strcat(request, "\r\n\r\n");
    } write(sock, request, strlen(request));
    return sock;
}

int main(int argc, char *argv[] ){
    FILE *html_file = NULL, *html_tree = NULL, *request_file = NULL; //Ponteiros abertura arquivo
    struct sockaddr_in servidor, cliente; //Estrutura para server e client
    char *buf = malloc(TAM_BUFFER); //Alocacao do tamanho buffer
    char dir[150] = "\0", nano[200] = "\0", new_url[150] = "\0", new_host[150] = "\0", aux_url[150] = "\0"; //Inicia a URL como NULL (/0)
    char *href_list[HREF_LIST_SIZE]; 
    char *content_length, *header, size_content[50];
    char opt;
    bzero(href_list, 4096);
    int init_socket, new_socket; // sockets do servidor e cliente
    int tr=1, c, i=0, opcao = 0;
    long int message_len;
    long int timeout = 0, header_size, q = 0, file_size = 0;
    unsigned int addr_len;
    unsigned int cliente_lenght = sizeof(cliente);
    
    arvore *head_href = (arvore *)malloc(sizeof(arvore)); //Arvore para receber requisicoes
    initialize_node(head_href); //inicializa a arvore
    bzero(buf, TAM_BUFFER);
    bzero(new_url, 150);
    bzero(new_host, 150);

    init_socket = socket(AF_INET, SOCK_STREAM, 0); // cria o socket do servidor
    if(init_socket < 0){
        perror("Erro ao criar socket!\n");
    }
    printf("Socket criado! Aguardando cliente para conexao...\n");

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta); // conecta-se a porta 8228 ou a porta passada por argv

    addr_len = sizeof(struct sockaddr_in); //Recebe endereco
    
    if (setsockopt(init_socket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
        perror("Erro no socket");
        exit(1);
    } if(bind(init_socket, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Erro Bind\n"); //Faz a conexao ao servidor criado
    }

    if(listen(init_socket, 1) == -1) { //Inicia a escuta pela porta 8228
        perror("Erro ao conectar a porta\n");
    }
    printf("Server iniciado!\n");
    printf("Porta: %d\n", porta);

    // aceita o cliente que deseja-se conectar ao servidor e reserva um socket para ele
    if ((new_socket = accept(init_socket,(struct sockaddr *) &cliente, &cliente_lenght)) == -1) {
        perror("Erro ao aceitar o cliente\n");
    }
    printf("Cliente aceito.\n");

    close(init_socket);

    if((message_len = recv(new_socket, buf, TAM_BUFFER, 0)) > 0) {  // recebe o request
        buf[message_len - 1] = '\0';

        if(strstr(buf, "POST") != NULL) { // se o tipo do request for POST, fecha o socket 
            printf("Requisicao do tipo POST.\n");
            close(new_socket);
            // break;
            exit(0);
        }

        request_file = fopen("request.txt", "w");

        if(request_file == NULL){
            printf("Arquivo nao foi aberto.\n");
            exit(1);
        }
        fputs(buf, request_file);
        fclose(request_file);
        system("nano request.txt"); // abre a opção para o usuário editar o request recebido
        request_file = fopen("request.txt", "r");

        i = 0;
        bzero(buf, TAM_BUFFER);
        if (request_file) {
            while ((c = getc(request_file)) != EOF){
                buf[i] = c;
                i++;
            }
            fclose(request_file);
        }
    }

    printf("Request: %s.\n", buf);
    parsing(buf, new_url, new_host); // realiza o parsing para retirar informações úteis do request

    // faz novo GET para o cliente
    int sock = get_host_by_name(new_url, new_host);

    bzero(buf, TAM_BUFFER); //limpa o buffer antigo

    bzero(dir, 150);
    strcpy(aux_url, new_url);
    for(i=0;i<strlen(aux_url);i++){
        if(aux_url[i] == '/' || aux_url[i] == ':')
            aux_url[i] = '-';
    }

    strcpy(dir, aux_url);
    strcat(dir, "/index.txt");
    mkdir(aux_url, S_IRUSR | S_IWUSR | S_IXUSR);
    html_file = fopen(dir, "w");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo!\n");
        exit(2);
    }

    
    while(read(sock, buf, TAM_BUFFER - 1) != 0){
        if( (header = strstr(buf, "\r\n")) != NULL){
            if( (content_length = strstr(buf, "Content-Length:")) != NULL){
                timeout = content_length - buf + 15;
                header_size = header - buf + 4;
                bzero(size_content, 50);
                while(buf[timeout+q] != '\r'){
                    size_content[q] = buf[timeout+q];
                    q++;
                }
                timeout = atoi(size_content);
                file_size = TAM_BUFFER - header_size;
                timeout-=file_size;
            }
        }
        timeout-=TAM_BUFFER;
        if(timeout == 0)
            break;
        fputs(buf, html_file);
        bzero(buf, TAM_BUFFER);
    }

    fclose(html_file);
    strcat(nano, "nano ");
    strcat(nano, dir);
    system(nano);
    html_file = fopen(dir, "r");
    bzero(buf, TAM_BUFFER);
    while(fread(buf, 1, TAM_BUFFER, html_file) == TAM_BUFFER){ // envia de volta para o browser a página html
        send(new_socket, buf, TAM_BUFFER, 0);
    }
    
    printf("Deseja executar spider?\n");
    scanf("%c", &opt);
    getchar();
    if(opt == 's') {
        system("clear");
        strcpy(head_href->href, new_url);
                    spider(new_url, new_host, aux_url, head_href, aux_url);
                    imprime_arvore(head_href, 0);
    }


    
    fclose(html_file);
    close(new_socket); // fecha o socket do cliente
}