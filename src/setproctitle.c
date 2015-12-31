/*
 *  set process title for ps (from sendmail)
 *
 *  Clobbers argv of our main procedure so ps(1) will display the title.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "setproctitle.h"

/*
 * To change the process title in Linux and Solaris we have to set argv[1]
 * to NULL and to copy the title to the same place where the argv[0] points to.
 * However, argv[0] may be too small to hold a new title.  Fortunately, Linux
 * and Solaris store argv[] and environ[] one after another.  So we should
 * ensure that is the continuous memory and then we allocate the new memory
 * for environ[] and copy it.  After this we could use the memory starting
 * from argv[0] for our process title.
 *
 * The Solaris's standard /bin/ps does not show the changed process title.
 * You have to use "/usr/ucb/ps -w" instead.  Besides, the UCB ps does not
 * show a new title if its length less than the origin command line length.
 * To avoid it we append to a new title the origin command line in the
 * parenthesis.
 */

extern char **environ;

static char *os_argv_last;

int
init_setproctitle(int argc, char **argv)
{
    char      *p;
    size_t       size;
    int        i;

    size = 0;

    for (i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }

    p = (char *)malloc(size);
    if (p == NULL) {
        return -1;
    }

    os_argv_last = argv[0];

    for (i = 0; argv[i]; i++) {
        if (os_argv_last == argv[i]) {
            os_argv_last = argv[i] + strlen(argv[i]) + 1;
        }
    }

    for (i = 0; environ[i]; i++) {
        if (os_argv_last == environ[i]) {

            size = strlen(environ[i]) + 1;
            os_argv_last = environ[i] + size;

            strncpy(p, (char *) environ[i], size);
            environ[i] = (char *) p;
            p += size;
        }
    }

    os_argv_last--;

    return 0;
}


void
setproctitle(char *title)
{
    char     *p;

    os_argv[1] = NULL;

    p = strncpy((char *)os_argv[0], title,
            os_argv_last - os_argv[0]);
    zlog_debug(g_zc, "title: %s", p);

    size_t title_len = strlen(p);
    p += title_len;

    if (os_argv_last -  (char *) p) {
        memset(p, 0, os_argv_last -  (char *) p);
    }

    zlog_debug(g_zc, "setproctitle: %s", os_argv[0]);
}
