#pragma once

#include "struct.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LINE 200
#define MAX_TOKEN 50

int is_number(const char *s);
int read_input(char *path, Configs *configs);
int set_input(char *path, Configs *configs);
