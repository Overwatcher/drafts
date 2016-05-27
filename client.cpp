#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#define N 2048
using namespace std;
char *FormMessage(int *length, int *quantity, double *ar) {
	char *msg = new char[sizeof(int) + *length];
	int i = 0;
	int j;
	for (j = 0; j < sizeof(int); j++) {
		msg[i + j] = ((char *)length)[j];
	}
	i += j;
	for (j = 0; j < sizeof(int); j++) {
		msg[i + j] = ((char *)quantity)[j];
	}
	i += j;
	for (j = 0; j < *quantity * sizeof(double); j++) {
		msg[i + j] = ((char *)ar)[j];
	}
	cout<<"Length check : "<<*(int *)msg<<endl;
	cout<<"Q check : "<<*(int *)&msg[4]<<endl;
	cout<<"D check : "<<*(double *)&msg[8]<<endl;
	return msg;
}
int ReadDoubles(char *str, double *ar, const int limit) {
	int i = 0;
	int count = 0;
	char *temp = str;
	if (str == NULL) return 0;
	while (str[i] != 0) {
		while (str[i] !=0 && str[i] != ' ') {
			i++;
		}
		if (1 != sscanf(temp, "%lf", &ar[count])) return count;
		temp = &str[i];
		count++;
		i++;
	}
	return count;

}
int main() {
	cout<<"Enter the Ipv4 -> ";
	int size;
	size_t alloc =0;
	char *ip = NULL;
	size = getline(&ip, &alloc, stdin);
       	if (size == -1)	{
		perror("getline");
		free (ip);
		return 1;	
	}
	else ip[size -1] = 0;
	for (int i = 0; i < size - 1; i++) printf("%c", ip[i]);
	cout<<endl;
	cout<<"Enter the service -> ";
	char *port = NULL;
	alloc = 0;
	size = getline(&port, &alloc, stdin);
       	if (size == -1)	{
		perror("getline");
		free(port);
		return 1;	
	}
	else port[size - 1] = 0;
	cout<<endl;
	
	int sockfd;
	struct addrinfo *res, *rp, hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = 0;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	cout<<"Connecting ..."<<endl;
	
	if (getaddrinfo(ip, port, &hints, &res)) {
		perror("getaddrinfo");
		free(port);
	cout<<"Forming message..."<<endl;
		free(ip);
		return 1;
	}
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1) {
			perror("socket");
			continue;
		}
		if (-1 == connect(sockfd, rp->ai_addr, rp->ai_addrlen) ) {
			perror("connect");
			continue;
		}
		break;
	}
	if (rp == NULL) {
		perror("Couldn't connect");
		return 1;
	}
	freeaddrinfo(res);
	free(ip);
	free(port);
	cout<<"Connected!"<<endl;
	
	cout<<"Enter doubles(no more than "<<N<<" ) separated with space -> ";
	
	char *string = NULL;
	char *msg;
	double *array = new double[N];
	int quantity = 0;
	alloc = 0;
	size = getline(&string, &alloc, stdin);
       	if (size == -1)	{
		perror("getline");
		free(string);
		close(sockfd);
		return 1;	
	}
	else string[size - 1] = 0;
	cout<<"Forming message..."<<endl;
	quantity = ReadDoubles(string, array, N);
	int length = quantity * sizeof(double) + sizeof(int);
	msg = FormMessage(&length, &quantity, array);
	cout<<"Sending ..."<<endl;
	if (length + sizeof(int) != send(sockfd, msg, length + sizeof(int) , 0)) {
		cout<<"Couldn't send the entire data"<<endl;
		perror("send");
		close(sockfd);
		return 1;
	}
	cout<<"Sent!"<<endl;
	
	free(string);
	delete(msg);
	cout<<"Type 'get' to get sum -> ";
	char *get = NULL;
	alloc = 0;
	size = getline(&get, &alloc, stdin);
       	if (size == -1)	{
		perror("getline");
		free(get);
		close (sockfd);
		return 1;	
	}
	else get[size - 1] = 0;
	if (strcmp(get, "get") != 0) {
		cout<<"Unknown command"<<endl;
		close (sockfd);
		return 1;
	}
	int iget[2];
	iget[0] = sizeof(int);
	iget[1] = -1;
	if (sizeof(iget) != send(sockfd, iget, sizeof(iget), 0)) {
		cout<<"Couldn't send the entire date"<<endl;
		perror("send");
		close(sockfd);
		return 1;
	}
	free(get);
	cout<<"Waiting for a response..."<<endl;
	double sum = 0;
	int count = recv(sockfd, &sum, sizeof(double), 0);
	if (count == -1) {
		perror("recv");
		close(sockfd);
		return 1;
	}
	if (count != sizeof(double)) {
		cout<<"Not enough data";
		close(sockfd);
		return 1;
	}
	cout<<"Sum is -> "<<sum<<endl;
	close(sockfd);
	cout<<"Connection closed"<<endl;
	return 0;
}
