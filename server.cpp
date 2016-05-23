#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
class user {
	private:
		int sockfd;
		double sum;
	public:
		user(int fd) {sockfd = fd;}
		user(int fd, double s) {sockfd = fd; sum = s;}
		void SetSum(double s) {sum = s;}
		int Write() {
			return send(sockfd, &sum, sizeof(double);
		}
		~user() {};
};
int main() {
	int yes=1, sockfd, newfd, listen_sock;
	fd_set read_set, write_set, exception_set, master;
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
		setsockopt(listen_sock, SOL_SOCK, SO_REUSEADDR, &yes, sizeof(int)); 
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
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&exception_set);
	FD_ZERO(&master);
	FD_SET(listen_sock, &master);
	FD_SET(listen_sock, &read_set);
	int r;
	int nfds = listen_sock;
	while (1) {
		r = select(...);		
	}
	return 0;
}
