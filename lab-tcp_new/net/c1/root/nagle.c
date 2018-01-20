#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#ifndef SOL_TCP
#define SOL_TCP 6
#endif

int usage(char *name)
{
    fprintf(stderr, "Usage: %s cork|nodelay|default <remote_ip> <remote_port> <delay_msec> [num_packets]\n", basename(name));
    return 1;
}

int main(int ac, char *av[])
{
    char *mode;
    int delay = 0;
    char *ip;
    int port;
    unsigned num_packets = -1;

    if (ac < 5) exit(usage(av[0]));

    mode      = av[1];
    ip        = av[2];
    port      = atoi(av[3]);
    delay     = atoi(av[4]);
    if (av[5]) num_packets = atoi(av[5]);

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    inet_aton(ip, &sa.sin_addr);

    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (connect(s, &sa, sizeof(sa)) < 0)
        perror("connect");

    int turn_on = 1;
    int option = -1;

    if (! strcmp(mode, "cork"))
        option = TCP_CORK;
    else if (! strcmp(mode, "nodelay"))
        option = TCP_NODELAY;
    else if (strcmp(mode, "default"))
        exit(usage(av[0]));

    if (option != -1)
        if (setsockopt(s, SOL_TCP, option, (void *) &turn_on, sizeof(int)) < 0)
            perror("setsockopt");

    char buf[100];
    memset(buf, ' ', sizeof(buf));
    for (unsigned i = 0; i < num_packets; ++i)
    {
        send(s, buf, sizeof(buf), 0);
        if (delay > 0)
            usleep(delay * 1000); //us -> ms
        else
            if (delay == 0)
                usleep(1); // to prevent buffering
    }
    return 0;
}
