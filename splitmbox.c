/*
 * Copyright (c) 2005 Jan Schaumann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void check_dir(void);
int do_split(FILE *);
void usage(void);

#define MAX_LINE_LENGTH		1024
#define NAME 			"splitmbox"
#define VERSION 		"0.1"

int mcount;
char *dir;

/* Split mbox input into individual mail files. */
int
main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind;

	FILE *f;
	int ch, ret, cnt, num;

	dir = ".";
	mcount = 0;

	while ((ch = getopt(argc, argv, "d:hv")) != -1) {
		switch(ch) {
		case 'd':
			dir = optarg;
			break;
		case 'h':
		case '?':
			usage();
			exit(EXIT_SUCCESS);
			/* NOTREACHED */
			break;
		case 'v':
			(void)printf("%s: Version %s\n", NAME, VERSION);
			exit(EXIT_SUCCESS);
			/* NOTREACHED */
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
			/* NOTREACHED */
		}
	}

	argc -= optind;
	argv += optind;

	cnt = argc;

	ret = 0;
			
	check_dir();

	if (cnt == 0) {
		return do_split(stdin);
	} else {
		for (num = 0; num < argc; num++) {
			char *file;
			file = argv[num];

			if (strcmp(file, "-") == 0)
				ret += do_split(stdin);
			else if ((f = fopen(file, "r")) == NULL) {
				(void)fprintf(stderr, "%s: %s: %s\n",
					      NAME, file,
					      strerror(errno));
			} else {
				ret += do_split(f);
				fclose(f);
			}
		}
	}

	return ret;
}

void
check_dir()
{
	struct stat sb;

	if (stat(dir, &sb) == -1) {
		if (mkdir(dir, S_IRUSR|S_IWUSR|S_IXUSR) == -1) {
			(void)fprintf(stderr, "%s: Unable to create directory %s: %s\n",
				      NAME, dir, strerror(errno));
			exit(EXIT_FAILURE);
			/* NOTREACHED */
		}
		return;
	}

	if (!(sb.st_mode & S_IFDIR)) {
		(void)fprintf(stderr, "%s: %s is not a directory!\n",
			      NAME, dir);
		exit(EXIT_FAILURE);
		/* NOTREACHED */
	}
}

void
usage() {
	(void)printf("Usage: %s [-d dir] [-h] [-v] [file ...]\n", NAME);
	(void)printf("Split mbox input into individual mail files.\n");
	(void)printf("Supported options:\n");
	(void)printf("\t-d dir \tspecify directory to create output in\n");
	(void)printf("\t-h     \tdisplay this message and exit\n");
	(void)printf("\t-v     \tdisplay version information and exit\n");
}


/* performs the actual splitting, returning 0 on success, 1 on error */
int
do_split(FILE *rfile) {
	char buf[MAX_LINE_LENGTH];
	char out[PATH_MAX];

	int retval;
	int i, wrflg, wfd;

	retval = wrflg = 0;
	wfd = -1;
	memset(&buf, '\0', MAX_LINE_LENGTH);
	memset(&out, '\0', PATH_MAX);

	while (fgets(buf, MAX_LINE_LENGTH, rfile) != NULL) {
		if ((i = strncmp(buf, "From ", 5)) == 0) {
			mcount++;
			if (wfd != -1) {
				(void)close(wfd);
				memset(&out, '\0', PATH_MAX);
				wfd = -1;
			}

			wrflg= 1;
			if (snprintf(out, PATH_MAX, "%s/%06d", dir, mcount) == -1) {
				(void)fprintf(stderr,
					 "%s: Unable to create filename: %s\n",
					 NAME, strerror(errno));
				retval = 1;
				continue;
			}
		}

		if (wfd == -1) {
			if ((wfd = open(out, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR)) == -1) {
				(void)fprintf(stderr,
					  "%s: Unable to create file %s: %s\n",
					  NAME, out, strerror(errno));
				retval = 1;
				continue;
			}
		}
		if (wrflg) {
			int written;
			int len = strlen(buf);

			while ((written = write(wfd, buf, len)) != -1 &&
					written != len) {
				len = len - written;
			}
			if (written == -1) {
				(void)fprintf(stderr,
					  "%s: Unable to write to %s: %s\n",
					  NAME, out, strerror(errno));
				retval = 1;
				continue;
			}
		}
	}
	(void)close(wfd);

	return retval;

}
