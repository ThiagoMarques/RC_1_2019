#include "library.h"

int porta = 8228;
int nivel;
int TAM_BUFFER = 16384; // tamanho do buffer

void spider(char *url, char *host, char *dir, struct Tree *head_href, char *root_dir) {
    printf("SPIDER ON URL: %s\n", url);
    printf("SPIDER ON DIR: %s\n", dir);
    FILE *html_tree, *html_file;
    char *href, buf[TAM_BUFFER], c;
    char *needle;
    char final_url[250] = "\0", href_tree[300] = "\0", href_dir[300] = "\0";
    size_t href_size = 256;
    long int i = 0, j=0;
    href = (char *)malloc(href_size * sizeof(char));
    bzero(href, 256);
    bzero(buf, TAM_BUFFER);
    if(!head_href){
        printf("Sem memoria disponivel!\n");
        exit(0);
    }

    strcpy(href_dir, dir);
    strcat(href_dir, "/index.txt");
    html_file = fopen(href_dir, "r");
    strcpy(href_tree, dir);
    strcat(href_tree, "/html_tree.txt");
    html_tree = fopen(href_tree, "a");

    if(html_file == NULL){
        printf("Erro ao abrir o arquivo. SPIDER HTML_FILE\n");
        exit(1);
    }

    if(html_tree == NULL){
        printf("Erro ao abrir o arquivo. SPIDER HTML_TREE\n");
        exit(2);
    }

    if(strcmp(url, host) == 0)
        strcpy(final_url, host);
    else
        strcpy(final_url, url);

    while(getline(&href, &href_size, html_file) != -1) {
        if((needle = strstr(href, "href=")) != NULL){
            i = needle - href + 6;
            while((c = href[i])!= '"'){
                buf[j] = c;
                i++;
                j++;
            }
            strcat(buf, "\r\n");
            if(strstr(buf, final_url) != NULL){
                make_tree(buf, head_href, href_tree, host, dir, root_dir);
            }else if(buf[0] == '/' && buf[1]!='/' ){ //pega hrefs que sejam do tipo "/...."
                make_tree(buf, head_href, href_tree, host, dir, root_dir);
            }else if(buf[0] != 'h' && buf[0]!='#' && (strstr(buf, "@")==NULL)){ //pega hrefs que sejam do tipo "files/...."
                make_tree(buf, head_href, href_tree, host, dir, root_dir);
            }
            bzero(buf, TAM_BUFFER);
            j = 0;
        }
    }

    fclose(html_file);
    fclose(html_tree);
}

int walk_tree(char *href, struct Tree *head_href){ // checa a existencia do href na arvore
    int i;
    arvore *temp = head_href;
    if(strcmp(head_href->href, href) != 0){
        for (i=0; i<N; i++){
            if(temp->filhos[i] != NULL){
                if(strcmp(temp->filhos[i]->href, href) != 0){
                    walk_tree(href, temp->filhos[i]);
                }
                else
                    return TRUE; // achou a referencia
            }
        }
        return FALSE; // nao achou a referencia
    }
    return TRUE; // achou a referencia
}

void make_tree(char *href, struct Tree *head_href, char *href_tree, char*host, char*dir, char *root_dir){
    struct hostent *hp;
    struct sockaddr_in cliente;
    int on = 1, sock, i;
    char buf[TAM_BUFFER];
    char request[500] = "\0";
    char new_dir[300] = "\0";
    char system_call[200] = "\0";
    char aux_dir[300] = "\0";
    FILE *html_tree;
    arvore *temp = head_href;

    if(walk_tree(href, head_href) == FALSE){ // nao tem essa ocorrencia de href na arvore
        arvore *novo_href = (arvore *)malloc(sizeof(arvore));
        initialize_node(novo_href);
        strcpy(novo_href->href, href);
        for(i=0;i<N;i++){
            if(temp->filhos[i] == NULL){
                temp->filhos[i] = novo_href;
                i = N;
            }
        }
        html_tree = fopen(href_tree, "a");
        if(html_tree == NULL){
            printf("Erro ao abrir o arquivo. SPIDER make_tree.\n");
            exit(1);
        }
        fputs(href, html_tree);
        fclose(html_tree);

        if((strstr(href, ".html") != NULL || strstr(href, ".") == NULL) && strlen(href) > 3){
            i = 0;
            size_t href_size = 256;
            char *aux_href = (char *)malloc(href_size * sizeof(char));
            bzero(aux_href, 256);
            char temp_dir[300] = "\0";
            strcat(temp_dir, root_dir);
            strcat(temp_dir, "\temp.txt");
            FILE *fp = fopen(temp_dir, "r");
            if(fp){
                while(getline(&aux_href, &href_size, fp) != -1) {
                    if(strcmp(aux_href, href) == 0){
                        return; // esse href ja existe, nao precisa ser consultado!
                    }
                }
                fclose(fp);
            }
            fp = fopen(temp_dir, "a"); // abre o arquivo para escrever no final o href novo encontrado
            fputs(href, fp);
            fclose(fp);
            printf("HREF valido para download: %s\n", href);
            if ((hp = gethostbyname(host)) == NULL){
                herror("gethostbyname");
            }
            bcopy(hp->h_addr, &cliente.sin_addr, hp->h_length);
            cliente.sin_port = htons(80);
            cliente.sin_family = AF_INET;
            sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
            if (connect(sock, (struct sockaddr *)&cliente, sizeof(struct sockaddr_in)) == -1) {
                perror("nao foi possivel conectar. SPIDER");
                exit(4);
            }
            strcpy(request, "GET ");
            strcat(request, href);
            request[strlen(request)-2] = '\0';
            strcat(request, " HTTP/1.1\r\nHost: ");
            strcat(request, host);
            strcat(request, "\r\n\r\n");
            printf("%s", request);
            write(sock, request, strlen(request));
            strcat(new_dir, dir);
            strcat(new_dir, href);
            new_dir[strlen(new_dir)-2] = '\0';
            strcpy(system_call, "mkdir -p ");
            strcat(system_call, new_dir);
            strcpy(aux_dir, new_dir);
            strcat(new_dir, "/index.txt");

            system(system_call);
            FILE *file = fopen(new_dir, "w");
            if(file != NULL){
                while(read(sock, buf, TAM_BUFFER-1) != 0){
                    fwrite(buf, 1, sizeof(buf), file);
                }
            }
            fclose(file);
            nivel++;
            if(nivel <= MAX_NIVEL)
                spider(href, host, aux_dir, novo_href, root_dir);
        }
    }
}

void initialize_node(struct Tree *head_href){
    int i;
    for(i=0;i<N;i++)
        head_href->filhos[i] = NULL;
}
void imprime_arvore(struct Tree *node_href, int n_tab){ //impressao da arvore com links da pagina
    int i, j, control = 0;
    char buf[500], buf_tab[500];
    bzero(buf, 500);
    bzero(buf_tab, 500);
    int flag = 0;
    if(node_href != NULL){
        for(i=0;i<N;i++){
            FILE *fp = fopen("tree.txt", "a");
            FILE *fp_notab = fopen("tree_tab.txt", "a");
            for(j = control;j < n_tab;j++){
                printf("\t");
                strcat(buf, "\t");
            }
            if(node_href != NULL){
                if(flag == 0){
                    printf("%s\n", node_href->href);
                    flag = 1;
                    strcat(buf, node_href->href);
                    strcat(buf_tab, node_href->href);
                    if(strstr(buf, "\r\n") == NULL)
                        strcat(buf, "\r\n");
                    fprintf(fp,"%s", buf);
                    fprintf(fp_notab,"%s", buf_tab);
                    fclose(fp);
                    fclose(fp_notab);
                }
                bzero(buf, 300);
                if(node_href->filhos[i] != NULL){
                    imprime_arvore(node_href->filhos[i], n_tab+1);
                    if(flag == 1)
                        control = n_tab;
                        j = control;
                }else{
                    i = N;
                    j = n_tab;
                }
            } else{
                i = N;
                j = n_tab;
            }
        }
    }
}

void zera_arvore(struct Tree *head_href, int contador_href){ //Apaga o conteudo da arvore hypertext
    int i, contador = 0;
    arvore *temp = head_href;
    while (strcmp(temp->href, "\0") != 0) {
        for(i=0;i<N;i++){
            if(temp->filhos[i] == NULL)
                contador++;
            else{
                zera_arvore(temp->filhos[i], contador_href);
                contador++;
            }
        }
        if(contador == contador_href || contador == N)
            bzero(temp->href, 256);

    }
}
