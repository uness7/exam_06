#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>

typedef struct s_client {
	int	    id;
	char	msg[100000];
}	t_client;


t_client	clients[2048*2];
fd_set		read_set, write_set, current;
int			maxfd = 0, gid = 0;
char		recv_buffer[120000];
char		send_buffer[120000];
int			PROTOCOL = 0;
int 		CONNECTIONS = 100;



void	error(char *msg) {
		if (msg) {
				write(2, msg, strlen(msg));
		} else {
				write(2, "Fatal error", 11);
		}
		write(2, "\n", 1);
		exit(1);
}

void	send_to_all(int except) {
		for (int i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &write_set) && i != except) {
					if (send(i, send_buffer, strlen(send_buffer), 0) == -1) {
							error(NULL);
						}
			}
		}
}

int	main(int ac, char **av) {
	if (ac != 2) {
		error("Wrong number of arguments");
	}

	struct sockaddr_in	serveraddr;
	socklen_t		    len = sizeof(struct sockaddr_in);
	int			        serverfd = socket(AF_INET, SOCK_STREAM, PROTOCOL);
	if (serverfd == -1) {
		error(NULL);
	}

	maxfd = serverfd;

	FD_ZERO(&current);
	FD_SET(serverfd, &current);
	bzero(clients, sizeof(clients));
	bzero(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi(av[1]));

	if (bind(serverfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) {
		error(NULL);
	}

	if (listen(serverfd, CONNECTIONS) == -1) {
		error(NULL);
	}

	while (1) {
		read_set = write_set = current;
		if (select(maxfd + 1, &read_set, &write_set, 0, 0) == -1) {
			continue;
		}

		for (int i = 0; i <= maxfd; i++) {
				if (FD_ISSET(i, &read_set)) {
						if (i == serverfd) {
								int client = accept(serverfd, (struct sockaddr *) &serveraddr, &len);
								if (client == -1) {
										continue;
								}
								if (client > maxfd) {
										maxfd = client;
								}
								clients[client].id = gid++;
								FD_SET(client, &current);
								sprintf(send_buffer, "server: client %d just arrived\n", clients[client].id);
								send_to_all(client);
								break ;
						} else {
								int ret = recv(i, recv_buffer, sizeof(recv_buffer), 0);
								//int ret = read(i, recv_buffer, sizeof(recv_buffer));

								if (ret <= 0) {
										sprintf(send_buffer, "server: client %d just left\n", clients[i].id);
										send_to_all(i);
										FD_CLR(i, &current);
										close(i);
										bzero(clients[i].msg, sizeof(clients[i].msg));
										break ;
								} else {
										recv_buffer[ret] = '\0';
										for (int k = 0, j = strlen(clients[i].msg); k < ret; k++, j++)
										{
											clients[i].msg[j] = recv_buffer[k];
											if (clients[i].msg[j] == '\n') {
													clients[i].msg[j] = '\0';
													sprintf(send_buffer, "client %d: %s\n", clients[i].id, clients[i].msg);
													send_to_all(i);
													bzero(clients[i].msg, sizeof(clients[i].msg));
													j = -1;
											}
										}
								}
						}
			} 
		}
	}

	return 0;
}
	






	


