#include "../include/atomo.h"

/**
 * \file atomo.c
 * \brief Implementation of the atom
 */
int atomicNumber;
int main(int argc, char **argv)
{
    if (argc != 2)
        exit(EXIT_FAILURE);
    srand(getpid());
    setipcs();
    set_sigactions();
    struct sembuf sops2 = {0, 0, 0};
    semop(ids->START, &sops2, 1);
    atomicNumber = atoi(argv[1]);
    // printf(CYAN_COLOR "(Atom of %s: %d) atomicNumber: %d\n" RESET_COLOR,getAtomName(atomicNumber), getpid(), atomicNumber);
    while (1)
    {
        pause();
    }
}

/**
 * \fn void scissione(int newAtomicNumber)
 * \brief Create a new atom
 * \param newAtomicNumber The atomic number of the new atom
 */
void scissione(int newAtomicNumber)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        reserveSem(ids->SEMSTATS, 0);
        stats->scissions++;
        stats->scissions1s++;
        releaseSem(ids->SEMSTATS, 0);
        char *argp[3];
        // printf(CYAN_COLOR"(Atom %s) atomicNumber:%d, divided in %s(%d) and %s(%d)"RESET_COLOR"\n", getAtomName(atomicNumber + newAtomicNumber), atomicNumber + newAtomicNumber, getAtomName(atomicNumber), atomicNumber, getAtomName(newAtomicNumber), newAtomicNumber);
        energyRealease(atomicNumber, newAtomicNumber);
        char *newAtomicNumberString = malloc(sizeof(int));
        sprintf(newAtomicNumberString, "%d", newAtomicNumber);
        argp[0] = ATOMO_PATH;
        argp[1] = newAtomicNumberString;
        argp[2] = NULL;

        execv(argp[0], argp);
        perror("execv");
        exit(EXIT_FAILURE);
        free(newAtomicNumberString);
    }
    else if (pid > 0)
    {
        printf("**Nuovo pid :%d\n", pid);
        reserveSem(ids->SEMNUMATOMI, 0);
        reserveSem(ids->SEMPID, 0);
        *numAtomId = *numAtomId + 1;
        Msg msg = {pid, pid};
        CHECK(msgsnd(pidatomi, &msg, sizeof(pid_t), 0), -1, "msgsnd(pidatomi, &msg, sizeof(pid_t), 0)")
        releaseSem(ids->SEMNUMATOMI, 0);
        releaseSem(ids->SEMPID, 0);
    }
    else if (pid < 0)
    {
        printf(YELLOW_COLOR "\n\n(MASTER) MELTDOWN\n\n" RESET_COLOR);
        kill(getppid(), SIGTERM);
        exit(EXIT_FAILURE);
    }
}

/**
 * \fn void terminator()
 * \brief Terminate the process and his child
 */
void terminator()
{
    exit(EXIT_SUCCESS);
}

/**
 * \fn void handle_scissione()
 * \brief Handle the scission
 */
void handle_scissione()
{
    Msg msg;
    if (atomicNumber <= configs->MIN_N_ATOMICO)
    {
        reserveSem(ids->SEMNUMATOMI, 0);
        printf(YELLOW_COLOR "scission not possible\n" RESET_COLOR);
        reserveSem(ids->SEMSTATS, 0);
        stats->scorie++;
        stats->scorie1s++;
        releaseSem(ids->SEMSTATS, 0);
        *numAtomId = *numAtomId - 1;
        releaseSem(ids->SEMNUMATOMI, 0);
        terminator();
    }

    else
    {
        reserveSem(ids->SEMPID, 0);
        printf("Pid: %d\n", getpid());
        Msg msg = {getpid(), getpid()};
        CHECK(msgsnd(pidatomi, &msg, sizeof(pid_t), 0), -1, "msgsnd(pidatomi, &msg, sizeof(pid_t), 0)")

        releaseSem(ids->SEMPID, 0);
        printf(CYAN_COLOR "atomicNumber: %d\n" RESET_COLOR, atomicNumber);
        int newAtomicNumber = rand() % (atomicNumber - 1) + 1;
        printf(CYAN_COLOR "newAtomicNumber: %d\n" RESET_COLOR, newAtomicNumber);
        atomicNumber = atomicNumber - newAtomicNumber;
        scissione(newAtomicNumber);
    }
}

/**
 * \fn void setipcs()
 * \brief Set the IPCS
 */
void setipcs()
{
    CHECK((id = shmget(SHMKEY, 0, 0666)), -1, " shmget(getppid(), ...)")
    CHECK((ids = shmat(id, NULL, 0)), (void *)-1, "shmat(id, ...)")
    CHECK((pidatomi = msgget(ids->PIDATOMI, 0)), -1, "msgget(ids->PIDATOMI)")
    CHECK((numAtomId = shmat(ids->NUMATOMI, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)")
    CHECK((stats = shmat(ids->STATS, NULL, 0)), (void *)-1, "shmat(stats, ...)")
    CHECK((configs = shmat(ids->CONFIGS, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)")
}

/**
 * \fn void set_sigactions()
 * \brief Set the sigactions
 */
void signalHandler(int sig)
{
    switch (sig)
    {
    case SIGTERM:
        terminator();
        break;
    case SIGUSR1:
        handle_scissione();
        break;
    }
}

/**
 * \fn void set_sigactions()
 * \brief Set the sigactions
 */
void set_sigactions()
{
    struct sigaction sa;
    sigset_t mask;

    CHECK(sigemptyset(&mask), -1, "sigemptyset(&mask)")

    sa.sa_handler = signalHandler;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigemptyset(&mask);
}

/**
 * \fn void energyRealease(int atomicNumber, int atomicNumber2)
 * \brief Release the energy
 * \param atomicNumber The atomic number of the atom
 * \param atomicNumber2 The atomic number of the atom
 */
void energyRealease(int atomicNumber, int atomicNumber2)
{
    int maxenergyreleased = 59 * 59 - 59;
    int inibitionFactor = configs->ENERGY_DEMAND / maxenergyreleased;
    int energyRealeased = atomicNumber * atomicNumber2 - max(atomicNumber, atomicNumber2);
    reserveSem(ids->SEMCONFIGS, 0);
    if (configs->FLAGINIBITORE == 0)
    {
        reserveSem(ids->SEMSTATS, 0);
        stats->totalProducedEnergy += energyRealeased;
        stats->totalProducedEnergy1s += energyRealeased;
        stats->totalEnergy += energyRealeased;
        releaseSem(ids->SEMSTATS, 0);
    }
    else
    {
        reserveSem(ids->SEMSTATS, 0);
        char *energiaString = malloc(sizeof(int));
        sprintf(energiaString, "%d", (energyRealeased - (energyRealeased * inibitionFactor)));
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        struct tm *localTime = localtime(&currentTime.tv_sec);
        char timeString[15];
        strftime(timeString, sizeof(timeString), "%H:%M:%S", localTime);
        sprintf(timeString + strlen(timeString), ".%02ld", currentTime.tv_usec / 10000);
        strcat(stats->logOperation, "[");
        strcat(stats->logOperation, timeString);
        strcat(stats->logOperation, "] ");
        strcat(stats->logOperation, "Inhibitor absorbed [");
        strcat(stats->logOperation, energiaString);
        strcat(stats->logOperation, "]\n");
        stats->totalProducedEnergy += (energyRealeased * inibitionFactor);
        stats->absorbedEnergy += energyRealeased - (energyRealeased * inibitionFactor);
        stats->totalProducedEnergy1s += (energyRealeased * inibitionFactor);
        stats->totalEnergy += (energyRealeased * inibitionFactor);
        releaseSem(ids->SEMSTATS, 0);
    }
    releaseSem(ids->SEMCONFIGS, 0);
}

/**
 * \fn int max(int a, int b)
 * \brief Return the max between two numbers
 * \param a The first number
 * \param b The second number
 * \return The max between a and b
 */
int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

/**
 * \fn char* getAtomName(int atomicNumber)
 * \brief Get the name of the atom
 * \param atomicNumber The atomic number of the atom
 * \return The name of the atom
 */
char *getAtomName(int atomicNumber)
{
    FILE *file = fopen("data/elementi", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    char *atomName = NULL;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int number;
        char name[MAX_LINE_LENGTH];

        if (sscanf(line, "%d %s", &number, name) == 2)
        {
            if (number == atomicNumber)
            {
                atomName = strdup(name);
                break;
            }
        }
    }

    fclose(file);

    return atomName;
}
