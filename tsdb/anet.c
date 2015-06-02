#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

int anetTcpServer(int port, char* addr) {
	// TODO: Kolla WSAPoll
	WSADATA wsaData;
	int sockfd;
	int y;
	struct sockaddr_in sockaddr;

	WSAStartup(MAKEWORD(1,1), &wsaData);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printf("could not create socket");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	/*y = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));*/

	if(bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1) {
		printf("could not bind socket: %d", sockfd);
		exit(1);
	}

	if(listen(sockfd, 511) == -1) {
		printf("could not listen on socket: %d", sockfd);
		exit(1);
	}

	return sockfd;
}

int anetTcpAccept(int serversock) {
	int clientfd;
	struct sockaddr_in addr;
	int addr_size;

	addr_size = sizeof(struct sockaddr_in);
	clientfd = accept(serversock, (struct sockaddr*)&addr, &addr_size);
	if(clientfd == -1) {
		printf("could not accept client on socket: %d", serversock);
		exit(1);
	}

	return clientfd;
}

int anetRead(int fd, char* buf, int len) {
	int nread, totlen = 0;
    while(totlen != len) {
        nread = recv((SOCKET)fd,buf,len-totlen,0);
        if (nread == 0) return totlen;
        if (nread == -1) return -1;
        totlen += nread;
        buf += nread;
    }
    return totlen;
}

int anetWrite(int fd, char* buf, int len) {
	send((SOCKET)fd, buf, len, 0);
}

void anetClose(int fd) {
	closesocket((SOCKET)fd);
}