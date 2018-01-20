/* Простейший TCP-сервер, отправляющий все полученные данные обратно клиенту */
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

/* обслуживание одного клиента */
void serve(int sk) {
    char buf[BUFSIZE];
    int len;
    while (1) {
        len = recv(sk, buf, BUFSIZE, 0);
        if (len < 0) {
            perror("recv");
            break;
        } else if (len == 0) {
            printf("Remote host has closed the connection\n");
            break;
        }
        buf[len] = '\0';
        if (strcmp(buf, "/q") == 0)
            break;
        if (send(sk, buf, strlen(buf), 0) < 0) {
            perror("send");
            break;
        }
    }
    printf("Done serving\n");
    close(sk);
}

int main(void) {
    struct sockaddr_in addr;    /* для своего адреса */
    struct sockaddr_in sender;  /* для адреса клиента */
    socklen_t addrlen;          /* размер структуры с адресом */
    int sk;                     /* файловый дескриптор сокета */
    int client;                 /* сокет клиента */

    /* создаём TCP-сокет */
    if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /* указываем адрес и порт */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(1996);

    /* связываем сокет с адресом и портом */
    if (bind(sk, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    /* переводим сокет в состояние ожидания соединений  */
    if (listen(sk, SOMAXCONN) < 0) {
        perror("listen");
        exit(1);
    };
    printf("Echo server listening --- press Ctrl-C to stop\n");

    while (1) {
        /* accept блокируется, пока не появится входящее соединение */
        addrlen = sizeof(sender);
        if ((client = accept(sk, (struct sockaddr *) &sender, &addrlen)) < 0) {
            perror("accept");
            exit(1);
        }
        printf("new client accepted\n");
        serve(client);
    }
    /* unreachable code */
    close(sk);
    return 0;
}
