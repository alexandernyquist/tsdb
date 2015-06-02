#ifndef __TSDB_H
#define __TSDB_H

#include "sds.h"

#define TSDB_SERVER_PORT 6781
#define TSDB_IOBUF_LEN 1024
#define TSDB_MAX_ARG_COUNT 42

struct metric {
	char* name;
	unsigned int pointcount;
	unsigned int capacity;
	struct point** points;
};

struct point {
	int timestamp;
	unsigned long count;
};

struct tsdbServer {
	int port;
	char* addr;
	int socketfd;
	int numCommands;
};

struct tsdbClient {
	int clientfd;
	sds* argv;
	int argc;
	sds input;
};

typedef void tsdbCommandProc(struct tsdbClient* c);
struct tsdbCommand {
	char* name;
	tsdbCommandProc* proc;
};

struct tsdbClient* createClient(int fd);

void addReplyString(struct tscbClient* client, sds s);
void addReplyDouble(struct tscbClient* client, double d);

/* commands */
void echoCommand(struct tsdbClient* client);
void randCommand(struct tsdbClient* client);

#endif