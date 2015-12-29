#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#include "server.h"

const char *
get_message(g_error_code_e code);

void
default_router(evhtp_request_t *req, void *arg);

void clean(void);
#endif

