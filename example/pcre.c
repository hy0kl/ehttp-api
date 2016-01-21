/**
 * @describe:
 * @author: Jerry Yang(hy0kle@gmail.com)
 * */

/** gcc pcre.c -lpcre */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <err.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <pcre.h>

#define OVECCOUNT 30    /* should be a multiple of 3 */

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s pattern text\n", argv[0]);
        return 1;
    }

    const char * pattern = argv[1];
    const char * subject = argv[2];
    const char * errptr = NULL;
    pcre * p_pcre = NULL;
    int erroffset = -1;

    if (NULL == (p_pcre = pcre_compile(pattern, 0 | PCRE_UNGREEDY, &errptr, &erroffset, NULL))) {
        printf("PCRE compilation failed at offset %d, %s\n", erroffset, errptr);
        return 1;
    }

    int ovector[OVECCOUNT];
    int rc =  pcre_exec(p_pcre, NULL, subject, strlen(subject), 0, 0, ovector, OVECCOUNT);
    if (rc < 0) {
        if (rc == PCRE_ERROR_NOMATCH){
            printf("%s doesn't match %s\n", subject, pattern);
        } else {
            printf("Matching error %d\n", rc);
        }
    } else {
        printf("OK, %d matched ...\n", rc);
        printf("%s matches %s\n", subject, pattern);

        int i;
        for (i = 0; i < rc; i++) {
            const char *substring_start = subject + ovector[2 * i];
            int substring_length = ovector[2 * i + 1] - ovector[2 * i];
            printf("%2d: %.*s\n", i, substring_length, substring_start);
        }
    }

    free(p_pcre);

    return 0;
}

/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

