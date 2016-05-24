#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;
class user {
	private:
		int sockfd;
		double sum;
	public:
		user() {sockfd = -1; sum = 0;}
		user(int fd) {sockfd = fd; sum = 0;}
		user(int fd, double s) {sockfd = fd; sum = s;}
		void SetSum(double s) {sum = s;}
		int Write() {
			return send(sockfd, &sum, sizeof(double), 0);
		}
		~user() {};
};
int main() {
	int yes=1, sockfd, newfd, listen_sock;
	fd_set read_set, write_set, exception_set, master;
	struct addrinfo hints, *res, *rp;
	struct sockaddr_in newaddr;
	user users[1000];
	int user_count = 0;
	if (!memset(&hints, 0, sizeof(addrinfo))) {
		perror("memset error");
		return 1;
	}
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, "8800", &hints, &res)) {
		perror("getaddrinfo error");
		return 1;
	}
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		cout<<"first cycle"<<endl;
		listen_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); 
		if (listen_sock == -1) {cout<<"SOCKET FAILUR"<<endl; continue;}
		if (bind(listen_sock, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(listen_sock);
	}
	if (rp == NULL) {
		perror("Bind error");
		return 1;
	}
	freeaddrinfo(res);
	if (listen(listen_sock, 10)) {
		perror("Listen error");
		return 1;
	}
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&exception_set);
	FD_ZERO(&master);
	FD_SET(listen_sock, &master);
	FD_SET(listen_sock, &read_set);
	int r;
	int nfds = listen_sock;
	char ip[64];
	while (1) {
		read_set = master;
		fflush(0);
		cout<<"select is next"<<endl;
		r = select(nfds + 1, &read_set, &write_set,NULL , NULL);
		cout<<"CHECK"<<endl;
		if (r == -1) {
			perror("select error");
			break;
		}	
		if (r == 0) {
			continue;
		}	
		for (int i = 0; i <= nfds; i++) {
			if (i == listen_sock && FD_ISSET(i, &read_set)) {
				socklen_t len;
				newfd = accept(i, (struct sockaddr *)&newaddr, &len);
				if (newfd < 0) {
					perror("accept error");
					continue;
				}
				FD_SET(newfd, &master);
				user temp(newfd);
				users[newfd] = temp;
				user_count ++;
				if (inet_ntop(newaddr.sin_family, &(newaddr.sin_addr), ip, sizeof(ip)) == NULL) {
					perror("inet_ntop error");
				}
				printf("User connected. Ip:%s, socket fd:%d\n", ip, newfd);
				if (newfd > nfds) nfds = newfd;
				continue;
			}
			if (FD_ISSET(i, &read_set)) {
				int length;
				int quantity;
				int count = recv(i, &length, sizeof(int), 0);
				if (count == 0) {
					cout<<"User closed connection. Socket fd:"<<i<<endl;
					FD_CLR(i, &master);
					close(i);
					continue;
				}
				if (count < sizeof(int)) {
					cout<<"Dont want to handle partial recv. Socket fd"<<i<<endl;
					FD_CLR(i, &master);
					close(i);
					continue;
				}
				cout<<"L:"<<length<<endl;
				count = 0;
				count = recv(i, &quantity, sizeof(int), 0);
				if (count < sizeof(int)) {
					cout<<"User closed connection or other error. Socket fd:"<<i<<endl;
					FD_CLR(i, &master);
					close(i);
					continue;
				}
				if (quantity == -1) {
					FD_SET(i, &write_set);
					FD_CLR(i, &read_set);
					continue;
				}
				cout<<"Q:"<<quantity<<endl;
				double number;
				double sum = 0;
				for (int j = 0; j < quantity; j++ ) {
					count = recv(i, &number, sizeof(double), 0);
					if (count < sizeof(double) ) {
						cout<<"Wrong user data. Socket fd:"<<i<<endl;
						FD_CLR(i, &master);
						close(i);
						break;
					}	
					sum += number;
					cout<<"N:"<<number<<endl;
				}
				if (!FD_ISSET(i, &master)) continue ;	
				users[i].SetSum(sum);
				continue;
			}
			if (FD_ISSET(i, &write_set)) {
				int count = users[i].Write();
				if (count < sizeof(double)) {
					cout<<"Sending error. Socket fd:"<<i<<endl;
				}	
				FD_CLR(i, &master);
				FD_CLR(i, &write_set);
				close(i);
				continue;
			}
		}

	}
	return 0;
}
