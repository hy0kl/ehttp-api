#ifndef UTIL_LINUX_SETPROCTITLE_H
#define UTIL_LINUX_SETPROCTITLE_H

#include "server.h"

int init_setproctitle(int argc, char **argv);

void setproctitle(char *title);

#endif
