#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>

#include "struct.h"
#include "BinarySemaphoreProtocol.h"

#define ATOMO_PATH "bin/atomo"
#define INIBITORE_PATH "bin/inibitore"
#define ATTIVATORE_PATH "bin/attivatore"
#define ALIMENTATORE_PATH "bin/alimentatore"
#define INPUT_PATH "data/input.txt"

#define SHMKEY 111

#define SEM_VAL_INIT 1

// Codici di colore ANSI per il testo
#define RESET_COLOR "\033[0m"
#define BLACK_COLOR "\033[0;30m"
#define RED_COLOR "\033[0;31m"
#define GREEN_COLOR "\033[0;32m"
#define YELLOW_COLOR "\033[0;33m"
#define BLUE_COLOR "\033[0;34m"
#define MAGENTA_COLOR "\033[0;35m"
#define CYAN_COLOR "\033[0;36m"
#define WHITE_COLOR "\033[0;37m"

// Codici di colore ANSI per il testo in grassetto
#define BOLD_BLACK_COLOR "\033[1;30m"
#define BOLD_RED_COLOR "\033[1;31m"
#define BOLD_GREEN_COLOR "\033[1;32m"
#define BOLD_YELLOW_COLOR "\033[1;33m"
#define BOLD_BLUE_COLOR "\033[1;34m"
#define BOLD_MAGENTA_COLOR "\033[1;35m"
#define BOLD_CYAN_COLOR "\033[1;36m"
#define BOLD_WHITE_COLOR "\033[1;37m"

#define MAX_LINE_LENGTH 100

pid_t firtstpid;
int pidatomi;
int id;
Ids *ids;
int *pidatomiaux;
int *numAtomId;
Stats *stats;
Configs *configs;

#define CHECK(cmd, err_v, strcmd)                                                                \
	if (cmd == err_v && errno != 0)                                                              \
	{                                                                                            \
		fprintf(stderr, "Al file %s (pid: %ld);\nErrore al comando ", __FILE__, (long)getpid()); \
		fprintf(stderr, strcmd);                                                                 \
		fprintf(stderr, ": errno = %d (%s);\n", errno, strerror(errno));                         \
		exit(EXIT_FAILURE);                                                                      \
	}                                                                                            \
	else                                                                                         \
	{                                                                                            \
	}

#define DO(cmd, err_v, strcmd)                                                                       \
	while (cmd == err_v)                                                                             \
	{                                                                                                \
		if (errno == ENOMSG)                                                                         \
			break;                                                                                   \
		else if (errno != EINTR)                                                                     \
		{                                                                                            \
			fprintf(stderr, "Al file %s (pid: %ld);\nErrore al comando ", __FILE__, (long)getpid()); \
			fprintf(stderr, strcmd);                                                                 \
			fprintf(stderr, ": errno = %d (%s);\n", errno, strerror(errno));                         \
			exit(EXIT_FAILURE);                                                                      \
		}                                                                                            \
	}
