#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/sys.h>
#include <sys/socket.h>
#include <unistd.h>

// Step 1: Define a t_client that has a message and a id
typedef struct s_client {
	int	    age;
	char	*msg[100000];
}	t_client;


// Step 2: we need to declare 8 global variables
t_client	clients[2048];
fd_set		read_set, write_set, current_set;
int		max_fd = 0, gid = 0;
char		recv_buffer[120000];
char		send_buffer[120000];
int		PROTOCOL = 0;
int 		CONNECTIONS = 100;

// Step 2: Write send_to_all() function and err() function
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
	for (int i = 0; i <= max_fd; ++i) {
		// check that the current fd is set and is not the exception
		if (FD_ISSET(i, &write_set) && i != except) {
			// send func takes in (the current fd, the message to send, the len, and a flag set to 0)
			if (send(i, send_buffer, strlen(send_buffer), 0) == -1) {
				printf("Failed to send to client: %d\n", i);
				error(NULL);
			}
		}
	}
}

// Step 3: write the main function
int	main(int ac, char **av) {
	if (ac != 2) {
		error("Wrong number of arguments");
	}

	/* Declare variables */
	struct sockaddr_in	serveraddr;
	socklen_t		    len = sizeof(struct sockaddr_in);
	int			        serverfd = socket(AF_INET, SOCKSTREAM, PROTOCOL);

	// ---
	if (serverfd == -1) {
		err(NULL);
	}
	
	max_fd = serverfd;

	FD_ZERO(&current);
	FD_SET(serverfd, &current);


	// init clients structure and serveraddr
	bzero(clients, sizeof(t_client));
	bzero(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi(av[1]));

	// bind the port av[1] to the server
	if (bind(serverfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) {
		err(NULL);
	}


	// listen to clients' connections
	if (listen(serverfd, CONNECTIONS) == -1) {
		err(NULL);
	}

	// main while loop
	while (1) {
		//
	}

	return 0;
}
	






	


