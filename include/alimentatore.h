#include "header.h"
#include "struct.h"

static void setipcs();
void creaAtomi();
void creaAtomo();
void set_sigactions(void);
void terminator();
void handle_sigInt();
void signalHandler(int);
char *getAtomName(int);