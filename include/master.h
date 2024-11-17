#include "header.h"
#include "struct.h"
#include "inputReader.h"


static void atomsCreation(int);
static void powerSupplyCreation();
static void activatorCreation();
static void InhibitorCreation();
static void printStatistics();
static void setIpcs();
void set_sigactions(void);
void startSimulation();
void handle_sigalrm();
void handle_sigTerm();
void handle_sigUsr2();
void signalHandler(int);
void removeIpcs();