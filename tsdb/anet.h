#ifndef __TSDB_ANET_H
#define __TSDB_ANET_H

int anetTcpServer(int port, char* addr);
int anetTcpAccept(int serversock);
int anetRead(int fd, char* buf, int count);
int anetWrite(int fd, char* buf, int count);
void anetClose(int fd);
#endif