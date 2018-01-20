//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <libgen.h>
#include <unistd.h>

int usage(char *name)
{
    fprintf(stderr, "Usage: %s <port> <delay_sec> [max_packets]\n", basename(name));
    return 1;
}

int main(int ac, char *av[])
{
    int delay = 0, port = 0;
    int reuseaddr = 1;
    unsigned max_packets = -1;

    if (ac < 3) 
        exit(usage(av[0]));
    port  = atoi(av[1]);
    delay = atoi(av[2]);
    if (av[3]) 
        max_packets = atoi(av[3]);

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) < 0)
        perror("reuseaddr");
    if (bind(s, &sa, sizeof(sa)) < 0)
        perror("bind");
    if (listen(s, 1))
        perror("listen");
    
    int fd = accept(s, NULL, NULL);
    if (delay > 0) 
        sleep(delay);

    char buf[1024];
    while (recv(fd, buf, sizeof(buf), 0) > 0 && --max_packets);

    return 0;
}
