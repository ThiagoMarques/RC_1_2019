//Bibliotecas padrao
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
//Bibliotecas exportadas
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netdb.h>


struct Tree{ 
    char href[256];
    struct Tree *filhos[20];
};
typedef struct Tree arvore; //Chamada da arvore Tree por arvore

void make_tree(char*, struct Tree *, char *, char *, char *, char *);
void initialize_node(struct Tree *);
int walk_tree(char*, struct Tree *);