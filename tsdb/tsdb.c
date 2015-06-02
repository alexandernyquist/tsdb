#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <time.h>

#include "anet.h"
#include "tsdb.h"

// INCR <METRIC> NOW 1\r\n
// INCR <METRIC> <TS> 1\r\n
// RANGE <METRIC> <TSFROM> <TSTO>\r\n

/* Global variables */
struct tsdbServer server;

struct tsdbCommand commandTable[] = {
	{"echo", echoCommand},
	{"rand", randCommand}
};

void* lookupCommand(sds name) {
	int i;
	for(i = 0; i < server.numCommands; i++) {
		if(strcmp(name, commandTable[i].name) == 0) {
			return &commandTable[i];
		}
	}
	return NULL;
}

void randCommand(struct tsdbClient* client) {
	addReplyDouble(client, rand());
}

void echoCommand(struct tsdbClient* client) {
	addReplyString(client, client->input);
}

void addReplyString(struct tsdbClient* client, sds s) {
	anetWrite(client->clientfd, s, sdslen(s));
}

void addReplyDouble(struct tsdbClient* client, double d) {
	char buf[50];
	int l;
	l = sprintf_s(buf, 50, "%.2f", d);
	anetWrite(client->clientfd, buf, l);
}

void initServerConfig() {
	srand((unsigned int)(time(NULL)));

	server.port = TSDB_SERVER_PORT;
	server.addr = "127.0.0.1";
	server.numCommands = sizeof(commandTable) / sizeof(struct tsdbCommand);
}

struct tsdbClient* createClient(int fd) {
	struct tsdbClient* client = (struct tsdbClient*)malloc(sizeof(struct tsdbClient));
	client->clientfd = fd;
	client->argc = 0;
	client->argv = (sds*)malloc(sizeof(sds) * TSDB_MAX_ARG_COUNT);
	client->input = sdsempty();
	return client;
}

void freeClient(struct tsdbClient* client) {
	if(client == NULL) {
		return;
	}

	if(client->argv != NULL) {
		free(client->argv);
	}
	
	if(client->input != NULL) {
		sdsfree(client->input);
	}

	free(client);
}

int main(int argc, char** argv) {
	// We need an array of metrics
	initServerConfig();
	server.socketfd = anetTcpServer(server.port, server.addr);

	printf("listening on port 1339\n");
	printf("listening for clients..\n");

	for(;;) {
		int fd;
		int nread;
		struct tsdbClient* client;
		struct tsdbCommand* cmd;

		fd = anetTcpAccept(server.socketfd);
		client = createClient(fd);

		printf("client accepted: %d\n", server.socketfd);
		
		for(;;) {
			// read command from client
			do {
				char buf[TSDB_IOBUF_LEN];
				nread = recv((SOCKET)fd, buf, TSDB_IOBUF_LEN, 0);
				client->input = sdscatlen(client->input, buf, nread);
				if(strchr(buf, '\r') != NULL) {
					client->input = sdsnewlen(client->input, sdslen(client->input)-2); //  exclude \r\n
					break;
				}
			} while(nread > 0);

			// handle quit commands
			if(strcmp(client->input, "quit") == 0) {
				break;
			}

			// parse command
			client->argc = sdssplit(client->input, ' ', client->argv);

			// invoke command
			cmd = lookupCommand(client->argv[0]);
			if(cmd != NULL) {
				printf("got command %s\n", cmd->name);
				cmd->proc(client);
			} else {
				addReplyString(client, sdsnew("error: unknown command"));
			}

			// reset raw input
			client->input = sdsempty();
		}

		anetClose(client->clientfd);
		freeClient(client);
	}

	_CrtDumpMemoryLeaks();
	return 0;
}