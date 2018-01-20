//

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


#define CMD_NOOP 0
// write $arg bytes to socket
#define CMD_SEND 1
// sleep for $arg milliseconds
#define CMD_WAIT 2
// set socket option TCP_CORK to $arg
#define CMD_CORK 3
// set socket option TCP_NODELAY to $arg
#define CMD_NODELAY 4
// set eth0 delay to $arg milliseconds via netem
#define CMD_DELAY 5
// go to command number $arg (counting from one)
#define CMD_GOTO 6
// syntax error in source file
#define CMD_SYNTAX_ERROR 7

char *CMD_NAMES[] = {
    "noop", "send", "wait", "tcp_cork", "tcp_nodelay", "/etc/delay", "goto", "syntax-error"
};

struct cmd {
    int cmd;
    int arg;
};

#define MAX_CMDS 129
#define LINE_LEN 256

struct scenario {
    struct cmd cmds[MAX_CMDS];
    int cmd_count;
};

void remove_newlines(char *str) {
    int i, shift;
    i = 0; shift = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n' || str[i] == '\r') shift++;
        else str[i - shift] = str[i];
        i++;
    }
    str[i - shift] = '\0';
};

void parse_scenario(FILE *input, struct scenario *scn) {
    int r;
    struct cmd c;
    char linebuf[LINE_LEN + 1];
    char cmdbuf[LINE_LEN + 1];

    scn->cmd_count = 0;

    while (scn->cmd_count < MAX_CMDS && !feof(input)) {
        c.cmd = CMD_NOOP;
        c.arg = 0;
        if (!fgets(linebuf, LINE_LEN, input)) {
            linebuf[0] = '\0';
        }
        remove_newlines(linebuf);

        if (linebuf[0] != '#' && linebuf[0] != '\0') {
            r = sscanf(linebuf, "%s %d", cmdbuf, &c.arg);

            if (r != 2) c.cmd = CMD_SYNTAX_ERROR;
            else if (strcmp(cmdbuf, "send") == 0) c.cmd = CMD_SEND;
            else if (strcmp(cmdbuf, "wait") == 0) c.cmd = CMD_WAIT;
            else if (strcmp(cmdbuf, "tcp_cork") == 0) c.cmd = CMD_CORK;
            else if (strcmp(cmdbuf, "tcp_nodelay") == 0) c.cmd = CMD_NODELAY;
            else if (strcmp(cmdbuf, "/etc/delay") == 0) c.cmd = CMD_DELAY;
            else if (strcmp(cmdbuf, "goto") == 0) c.cmd = CMD_GOTO;
        }
        scn->cmds[scn->cmd_count++] = c;
    }
}

void print_scenario(struct scenario *scn) {
    int i;
    struct cmd *c;

    for (i = 0; i < scn->cmd_count; i++) {
        c = &scn->cmds[i];
        printf("%2d: %s %d\n", i + 1, CMD_NAMES[c->cmd], c->arg);
    }
    printf("Total number of commands: %d\n", scn->cmd_count);
}

void set_socket_option(int s, int option, int value) {
    int option_value = 0;
    if (value) option_value = 1;
    if (setsockopt(s, SOL_TCP, option, (void *) &option_value, sizeof(int)) < 0)
        perror("setsockopt");
}

void play_scenario(struct scenario *scn) {
    /* TODO remove hardcode */
    char *ip = "10.40.0.2";
    int port = 9;
	struct sockaddr_in sa;
	int option_value = 1;
    int s;
    char *buf = malloc(65535);
    int pc;
    struct cmd cmd;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	inet_aton(ip, &sa.sin_addr);

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (connect(s, &sa, sizeof(sa)) < 0)
		perror("connect");

    pc = 0;
    while (pc < scn->cmd_count) {
        cmd = scn->cmds[pc++];
        switch (cmd.cmd) {
            case CMD_NOOP: break;
            case CMD_SYNTAX_ERROR: break;
            case CMD_GOTO: pc = cmd.arg; break;
            case CMD_CORK: set_socket_option(s, TCP_CORK, cmd.arg); break;
            case CMD_NODELAY: set_socket_option(s, TCP_NODELAY, cmd.arg); break;
            case CMD_DELAY:
                sprintf(buf, "/etc/delay %d", cmd.arg);
                if(system(buf) != 0) {
                    fprintf(stderr, "Error on system(\"%s\")\n", buf);
                }
                break;
            case CMD_WAIT: usleep(cmd.arg * 1000); break;
            case CMD_SEND:
                if (send(s, buf, cmd.arg, 0) < 0) {
                    perror("send");
                }
                break;
        }
    }
    printf("Replay finished.\n");
}


int usage(char *name)
{
	fprintf(stderr, "Usage: %s <replay_filename>\n", basename(name));
	return 1;
}

int main(int ac, char *av[]) 
{
    struct scenario scn;
    FILE *scn_file;

	if (ac < 2) exit(usage(av[0]));

    scn_file = fopen(av[1], "r");
    if (!scn_file) {
        fprintf(stderr, "File `%s` cannot be opened\n", av[1]);
        exit(1);
    }

    parse_scenario(scn_file, &scn);
    fclose(scn_file);
    print_scenario(&scn);
    play_scenario(&scn);

    return 0;
}
