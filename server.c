#include "library.h"

//***** Funcoes ******//

void parsing(char *buf, char *new_url, char *new_host)
{
    char *get = strstr(buf, "GET"); //Captura de requisicoes
    char *http = strstr(buf, "HTTP/1.1");
    char *host = strstr(buf, "Host:");
    long int i, j = 0;
    long int start_url = get - buf + 4; //manipula ponteiros
    long int end_url = http - buf - 1;
    long int start_host = host - buf + 6;

    for (i = start_url; i < end_url; i++)
    {
        new_url[j] = buf[i]; //armazena endereco
        j++;
    }
    j = 0;
    while (buf[start_host + j] != '\n' && buf[start_host + j] != '\r') //enquanto nao termina a url
    {
        new_host[j] = buf[start_host + j]; //recebe novo host
        j++;
    }
}

int get_host_by_name(char *new_url, char *new_host)
{ //Cria requisicao HTTP com o IP
    struct hostent *hp;
    char request[250];
    struct sockaddr_in cliente;
    bzero(request, 250);
    int on = 1, sock;
    printf("NEW host:%s\n", new_host);
    if ((hp = gethostbyname(new_host)) == NULL)
    {
        herror("gethostbyname");
    }
    bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
    cliente.sin_port = htons(80);
    cliente.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);                                 //cria um novo socket
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));       //Aloca memoria
    if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) //abre a conexao
    {
        perror("Nao foi possivel estabelecer conexao");
        exit(0);
    }
    else //se nao houve erros, captura requisicoes
    {
        strcat(request, "GET ");
        strcat(request, new_url);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, new_host);
        strcat(request, "\r\n\r\n");
    }
    write(sock, request, strlen(request));
    return sock;
}

int main(int argc, char *argv[])
{
    FILE *html_file = NULL, *html_tree = NULL, *request_file = NULL;                                          //Ponteiros abertura arquivo
    struct sockaddr_in servidor, cliente;                                                                     //Estrutura para server e client
    char *buf = malloc(TAM_BUFFER);                                                                           //Alocacao do tamanho buffer
    char index[150] = "\0", nano[200] = "\0", new_url[150] = "\0", new_host[150] = "\0", aux_url[150] = "\0"; //Inicia a URL como NULL (/0)
    char *href_list[HREF_LIST_SIZE];                                                                          //Guarda tamanho lista
    char *content_length, *header, size_content[50];
    int opt;
    bzero(href_list, 4096);      //buffer para uso
    int init_socket, new_socket; // sockets do servidor e cliente
    int tr = 1, c, i = 0, opcao = 0;
    long int message_len;
    long int timeout = 0, header_size, q = 0, file_size = 0;
    unsigned int addr_len;
    unsigned int cliente_lenght = sizeof(cliente);

    printf("--------------------------------------\n");
    printf("\tInspetor HTTP\n"); //Enviar request
    printf("--------------------------------------\n");

    if (argv[1])
    { // tratamento para o argumento do número da porta
        if (strcmp(argv[1], "-p") == 0)
            if (argv[2] != NULL)
                porta = atoi(argv[2]);
    }

    arvore *head_href = (arvore *)malloc(sizeof(arvore)); //Arvore para receber requisicoes
    initialize_node(head_href);                           //inicializa a arvore
    bzero(buf, TAM_BUFFER);
    bzero(new_url, 150); //Seguindo indicacao do monitor, guardando a pagina em buffer
    bzero(new_host, 150);

    init_socket = socket(AF_INET, SOCK_STREAM, 0); // cria o socket do servidor
    if (init_socket < 0)
    {
        perror("Erro ao criar socket!\n");
    }
    
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta); // conecta-se a porta 8228 ou a porta passada por argv

    addr_len = sizeof(struct sockaddr_in); //Recebe endereco

    if (setsockopt(init_socket, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1)
    {
        perror("Erro no socket");
        exit(0);
    }
    if (bind(init_socket, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
    {
        perror("Erro Bind\n"); //Faz a conexao ao servidor criado
    }

    if (listen(init_socket, 1) == -1)
    { //Inicia a escuta pela porta padrao 8228
        perror("Erro ao conectar a porta\n");
    }
    printf("Server iniciado!\n");
    printf("Porta: %d\n", porta);
    printf("Socket criado! Aguardando cliente para conexao...\n");

    // aceita o cliente que deseja-se conectar ao servidor e reserva um socket para ele
    if ((new_socket = accept(init_socket, (struct sockaddr *)&cliente, &cliente_lenght)) == -1)
    {
        perror("Erro ao aceitar o cliente\n");
    }
    printf("Cliente aceito.\n");

    close(init_socket);

    printf("Pressione ENTER para enviar request\n");
    getchar();

    if ((message_len = recv(new_socket, buf, TAM_BUFFER, 0)) > 0) //se nao houve erro -> request
    {
        buf[message_len - 1] = '\0';

        if (strstr(buf, "POST") != NULL)
        {
            printf("Requisicao do tipo POST.\n");
            close(new_socket); // se o tipo do request for POST, fecha o socket
            exit(0);
        }

        request_file = fopen("request.txt", "w"); //abre arquivo request.txt

        if (request_file == NULL)
        {
            printf("Arquivo nao foi aberto.\n");
            exit(1);
        }
        fputs(buf, request_file);
        fclose(request_file);
        system("nano request.txt"); // uso do 'nano' para modificar requisicao
        request_file = fopen("request.txt", "r");
        i = 0;
        bzero(buf, TAM_BUFFER);
        if (request_file)
        {
            while ((c = getc(request_file)) != EOF)
            {
                buf[i] = c;
                i++;
            }
            fclose(request_file);
        }
    }
    printf("Request: %s.\n", buf); //captura de informacoes de request
    parsing(buf, new_url, new_host);

    int sock = get_host_by_name(new_url, new_host); // Realiza GET no cliente

    bzero(buf, TAM_BUFFER);

    bzero(index, 150);
    strcpy(aux_url, new_url);
    for (i = 0; i < strlen(aux_url); i++)
    {
        if (aux_url[i] == '/' || aux_url[i] == ':')
            aux_url[i] = '-'; //tratamento da url
    }

    strcpy(index, aux_url);                      //realiza copia
    strcat(index, "/index.txt");                 //Inicia a URL como NULA e cria arquivo index com estrutura da pagina
    mkdir(aux_url, S_IRUSR | S_IWUSR | S_IXUSR); //Cria diretorio para guardar estrutura

    //Cria arquivo HTML
    html_file = fopen(index, "w");

    if (html_file == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        exit(0);
    }

    while (read(sock, buf, TAM_BUFFER - 1) != 0) //Escreve dados da pagina no arquivo
    {
        if ((header = strstr(buf, "\r\n")) != NULL)
        {
            if ((content_length = strstr(buf, "Content-Length:")) != NULL)
            {
                timeout = content_length - buf + 15;
                header_size = header - buf + 4;
                bzero(size_content, 50);
                while (buf[timeout + q] != '\r')
                {
                    size_content[q] = buf[timeout + q];
                    q++;
                }
                timeout = atoi(size_content);
                file_size = TAM_BUFFER - header_size;
                timeout -= file_size;
            }
        }
        timeout -= TAM_BUFFER;
        if (timeout == 0)
            break;
        fputs(buf, html_file);
        bzero(buf, TAM_BUFFER);
    }

    fclose(html_file);
    strcat(nano, "nano ");         //Abre opcao para modificar arquivo
    strcat(nano, index);           //Inclui arquivo para edicao
    system(nano);                  //Chamada do nano
    html_file = fopen(index, "r"); //Abertura do arquivo para leitura
    bzero(buf, TAM_BUFFER);
    while (fread(buf, 1, TAM_BUFFER, html_file) == TAM_BUFFER)
    {
        send(new_socket, buf, TAM_BUFFER, 0); // Envia bytes (pagina) ao socket
    }
    printf("------------------------------------------------------\n");
    printf("Digite a opcao desejada: \n");
    printf("1 - SPIDER \n"); //Execucao do spider
    printf("2 - DUMP \n");   //Execucao do spider
    printf("0 - Sair do programa\n");
    printf("------------------------------------------------------\n");
    scanf("%d", &opt);

    if (opt = 1)
    {
        system("clear");
        strcpy(head_href->href, new_url);
        spider(new_url, new_host, aux_url, head_href, aux_url);
        imprime_arvore(head_href, 0);
    }
    if (opt = 2)
    {
        html_tree = fopen(index, "a");
        if (html_tree != NULL)
        {
            fseek(html_tree, 0, SEEK_END);
            if (ftell(html_tree) == 0)
            {

                printf("Arquivo vazio!\n");
            }
            else
            {
                dump(new_url, new_host);
            }
        }
        else
        {
            printf("Nao foi possivel abrir o html tree.\n");
        }
    }
    else
    {
        fclose(html_file);
        close(new_socket); // fecha o socket do cliente
    }
}