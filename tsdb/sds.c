/* SDS (Simple Dynamic Strings), A C dynamic strings library.
 *
 * Copyright (c) 2006-2014, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include <stdlib.h>
#include "sds.h"

sds sdsempty() {
	return sdsnewlen("", 0);
}

sds sdsnew(const char* buf) {
	size_t initlen = buf == NULL ? 0 : strlen(buf);
	return sdsnewlen(buf, initlen);
}

sds sdsnewlen(const char* buf, size_t initlen) {
	struct sdshdr* sh;
	sh = (struct sdshdr*)malloc(sizeof(struct sdshdr) + initlen + 1); // +1 for \0

	sh->len = (int)initlen;
	if(initlen) {
		if(buf) {
			memcpy(sh->buf, buf, initlen);
		} else {
			memset(sh->buf, 0, initlen);
		}
	}
	sh->buf[initlen] = '\0';
	return (char*)sh->buf;
}

int sdslen(sds s) {
	struct sdshdr *sh = (s-(sizeof(struct sdshdr)));
	return sh->len;
}

int sdsindexof(sds s, char c) {
	const char* ptr = strchr(s, ' ');
	if(ptr) {
		return ptr-s;
	}
	return 0;
}

size_t sdsavail(const sds s) {
    struct sdshdr *sh = (s-(sizeof(struct sdshdr)));
    return sh->free;
}

sds sdscatlen(sds s, void* t, size_t len) {
	// create a new string combining s and buf
	struct sdshdr *sh;
    size_t curlen = sdslen(s);

    s = sdsMakeRoomFor(s,len);
    if (s == NULL) return NULL;
    sh = (s-(sizeof(struct sdshdr)));
    memcpy(s+curlen, t, len);
    sh->len = (int)(curlen+len);
    sh->free = (int)(sh->free-len);
    s[curlen+len] = '\0';
    return s;
}

sds sdsMakeRoomFor(sds s, size_t addlen) {
    struct sdshdr *sh, *newsh;
    size_t free = sdsavail(s);
    size_t len, newlen;

    if (free >= addlen) return s;
    len = sdslen(s);
    sh = (void*) (s-(sizeof(struct sdshdr)));
    newlen = (len+addlen);
    if (newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;
    newsh = realloc(sh, sizeof(struct sdshdr)+newlen+1);
	if (newsh == NULL)
		return NULL;

    newsh->free = (int)(newlen - len);
    return newsh->buf;
}

void sdsfree(sds s) {
	struct sdshdr *sh = (s-(sizeof(struct sdshdr)));
	free(sh);
}

int sdssplit(sds s, char c, sds* out) {
	int i;
	int len;
	int p; // length of current part
	char* buf; // current part
	
	int count = 0; // total number of splitted items
	p = 0;
	buf =  (char*)malloc(sizeof(char) * 255);

	for(i = 0, len = sdslen(s); i < len; i++) {
		if(s[i] == c) {
			out[count++] = sdsnewlen(buf, p);
			buf = (char*)realloc(buf, 255);
			p = 0;
		} else {
			buf[p++] = s[i];
		}
	}

	if(p > 0) {
		out[count++] = sdsnewlen(buf, p);
	}

	return count;
}