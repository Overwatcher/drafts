#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
int main() {
	int yes, sockfd, newfd, listen_sock;
	struct addrinfo hints, *res, *rp;
	if (!memset(&hints, 0, sizeof(addrinfo))) {
		perror("memset error");
		return 1;
	}
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = PF_INET;
	if (!getaddrinfo("127.0.0.1", 8800, &hints, &res)) {
		perror("getaddrinfo error");
		return 1;
	}
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		listen_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (listen_sock == -1) continue;
		if (bind(listen_sock, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(listen_sock);
	}
	if (rp == NULL) {
		perror("Bind error");
		return 1;
	}
	freeaddrinfo(res);
	while (1) {
		
	}
	return 0;
}
