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

extern int TAM_BUFFER;
extern int porta;
extern int nivel;
#define TRUE 1
#define FALSE 0
#define HREF_LIST_SIZE 4096
#define N 20
#define MAX_NIVEL 3


struct Tree{
    char href[256];
    struct Tree *filhos[N];
};
typedef struct Tree arvore;

int busca_href_arvore(char*, struct Tree *);
int get_host_by_name(char*, char*);
void parsing(char*, char*, char*);
void dump(char*, char*);
void make_tree(char*, struct Tree *, char *, char *, char *, char *);
void initialize_node(struct Tree *);
void spider(char *, char *, char *, struct Tree *, char *);
void imprime_arvore(struct Tree *, int);
void zera_arvore(struct Tree *, int);
