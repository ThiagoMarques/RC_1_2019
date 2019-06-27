#include "library.h"

void dump(char *url, char *host) {
    FILE *html_tree;
    struct hostent *hp;
    struct sockaddr_in cliente;
    int on = 1, sock;
    long int i, j = 0, k = 0;
    char buf[TAM_BUFFER], aux_buf[TAM_BUFFER];
    char index[500];
    char reverse_index[500];
    char href[256], c;
    char request[500];
    char *needle;
    bzero(index, 500);
    bzero(href, 256);
    bzero(request, 500);
    bzero(reverse_index, 500);
    index[0] = '\0';
    reverse_index[0] = '\0';

    html_tree = fopen("tree_tab.txt", "r");

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo. 3\n");
        exit(3);
    }

    while (fgets(href, sizeof(href), html_tree)) {
        j = 0;
        k = 0;
        i = 0;
        if ((hp = gethostbyname(host)) == NULL){
            herror("gethostbyname");
        }
        bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
        cliente.sin_port = htons(80);
        cliente.sin_family = AF_INET;
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
        if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
            perror("nao foi possivel conectar.");
            exit(4);
        }
        memset(index, '\0', 150);
        memset(reverse_index, '\0', 150);
        if((needle = strstr(href, "\r\n")) != NULL){
            i = needle - href - 1;
            while((c = href[i]) != '/')
                i--;
            while(((c = href[i-1]) != '/')){
                reverse_index[j] = c;
                j++;
                i--;
            }
            j = strlen(reverse_index);
            while(j > 0){
                index[k] = reverse_index[j-1];
                j--;
                k++;
            }
            k = 0;
            mkdir(index, S_IRUSR | S_IWUSR | S_IXUSR);
        }
        bzero(reverse_index, 150);
        if((needle = strstr(href, "\r\n")) != NULL){
            i = needle - href - 1;
            j = 0;
            while((c = href[i]) != '/')
                i--;
            while(((c = href[i+1]) != '\r')){

                reverse_index[j] = c;
                i++;
                j++;
            }
        }


        i = strlen(href) - 1;
        k = 0;
        int p = 0;
        char new_href[256];
        bzero(new_href, 256);
        char *pos;

        href[i-1] = '\0';
        if((pos = strstr(href, "\t"))){
            long int position = pos - href + 1;
            for (k = position; k <= i; k++) {
                new_href[p] = href[k];
                p++;
            }
            strcpy(href, new_href);
        }

        strcat(request, "GET ");
        strcat(request, href);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, host);
        strcat(request, "\r\n\r\n");
        printf("%s\n", request);
        write(sock, request, strlen(request));
        memset(buf, 0x0, TAM_BUFFER); //limpa o buffer antigo
        if(strcmp(reverse_index, "") == 0)
            strcpy(reverse_index, "dump");
        if(strlen(index) != 0){
            strcat(index, "/");
            strcat(index, reverse_index);
        }else{
            strcpy(index, reverse_index);
            if((strstr(index, "/") == NULL) || strstr(index, ".") == NULL){
                strcat(index, ".txt");
            }
        }

        FILE *file = fopen(index, "w");
        if(file != NULL){
            j = 0;
            while(read(sock, buf, TAM_BUFFER-1) != 0){
                if((needle = strstr(buf, "\r\n\r\n"))){
                    i = needle - buf + 4;
                    while(buf[i] != '\0'){
                        aux_buf[j] = buf[i];
                        i++;
                        j++;
                    }
                    fwrite(aux_buf, 1, sizeof(aux_buf), file);
                }
                else
                    fwrite(buf, 1 , sizeof(buf), file);
                bzero(aux_buf, TAM_BUFFER);
                bzero(buf, TAM_BUFFER);
            }
            fclose(file);
        }
        else{
            printf("Erro ao abrir o arquivo 5.\n");
            exit(5);
        }
        close(sock);
        bzero(index,150);
        bzero(reverse_index,150);
        bzero(href, 256);
        bzero(request, 500);
    }
}
