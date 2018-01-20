#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 512

int main(int argc, char **argv) {
    struct sockaddr_in addr;    /* для адреса сервера */ 
    socklen_t addrlen;          /* размер структуры с адресом */
    int sk;                     /* файловый дескриптор сокета */
    char buf[BUFSIZE];          /* буфер для сообщений */
    int len;
    
    if (argc != 2) {
        printf("Usage: echoc <ip>\nEx.:   echoc 10.30.0.2\n");
        exit(0);
    }
    
    /* создаём TCP-сокет */
    if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("socket");
        exit(1);
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(1996);
    
    /* соединяемся с сервером */
    if (connect(sk, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }
    
    printf("Connected to Echo server. Type /q to quit.\n");
    while (1) {
        printf("> ");
        fgets(buf, BUFSIZE, stdin);
        len = strlen(buf);
        if (len == 0)
            continue;
        buf[len-1] = '\0'; /* удаляем \n */
        if (strcmp(buf, "/q") == 0)
            break;
            
        if (send(sk, buf, len, 0) < 0) {
            perror("send");
            exit(1);
        }
        len = recv(sk, buf, BUFSIZE, 0);
        if (len < 0) {
            perror("recv");
            exit(1);
        } else if (len == 0) {
            printf("Remote host has closed the connection\n");
            exit(1);
        }
        
        buf[len] = '\0';
        printf("<< %s\n", buf);
    }
    
    sprintf(buf, "/q");
    if (send(sk, buf, strlen(buf), 0) < 0) {
        perror("send");
        exit(1);
    }
    close(sk);
    return 0;   
}
        
        
