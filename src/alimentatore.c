#include "../include/alimentatore.h"
/**
 * \file alimentatore.c
 * \brief Implementation of the power supply
 */
int pidatomi;
int main(void)
{
    setipcs();
    set_sigactions();
    struct sembuf sops = {0, -1, 0};
    semop(ids->START, &sops, 1);
    struct sembuf sops2 = {0, 0, 0};
    semop(ids->START, &sops2, 1);
    printf(RED_COLOR "(PowerSupply: %d)" RESET_COLOR "\n", getpid());
    while (1)
    {
        creaAtomi();
        nanosleep((const struct timespec[]){{0, 100000000L * (configs->STEP_ALIMENTAZIONE)}}, NULL);
    }
}

/**
 * \fn void creaAtomi()
 * \brief Create the atoms
 */
void creaAtomi()
{
    for (int i = 0; i < configs->N_NUOVI_ATOMI; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {

            srand(getpid());

            char *argp[3];
            char numAtomico[sizeof(int)];
            int numAtomicoInt = (random()) % configs->N_ATOM_MAX + 1;
            sprintf(numAtomico, "%d", numAtomicoInt);
            argp[0] = ATOMO_PATH;
            argp[1] = numAtomico;
            argp[2] = NULL;

            printf(RED_COLOR "(PowerSupply)%s atom created %d atomic number: %d" RESET_COLOR "\n", getAtomName(numAtomicoInt), getpid(), numAtomicoInt);

            execv(argp[0], argp);
            perror("execv");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {

            reserveSem(ids->SEMPID, 0);
            reserveSem(ids->SEMNUMATOMI, 0);
            Msg msg = {pid, pid};
            CHECK(msgsnd(pidatomi, &msg, sizeof(int), 0), -1, "msgsnd(pidatomi, &msg, sizeof(pid_t), 0)")
            *numAtomId = *numAtomId + 1;
            releaseSem(ids->SEMNUMATOMI, 0);
            releaseSem(ids->SEMPID, 0);
        }
        else if (pid < 0)
        {
            printf(YELLOW_COLOR "\n\n(MASTER) MELTDOWN\n\n" RESET_COLOR);
            DO(kill(getppid(), SIGTERM), -1, "kill(0, SIGTERM)")
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * \fn void setipcs()
 * \brief Set the IPCS
 */
static void setipcs()
{
    CHECK((id = shmget(SHMKEY, sizeof(Ids), 0666)), -1, " shmget(getppid(), ...)");
    CHECK((ids = shmat(id, NULL, SHM_RDONLY)), (void *)-1, "shmat(id, ...)");
    CHECK((configs = shmat(ids->CONFIGS, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)")
    CHECK((pidatomi = msgget(ids->PIDATOMI, 0)), -1, "msgget(ids->PIDATOMI)")
    CHECK((numAtomId = shmat(ids->NUMATOMI, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)");
}

/**
 * \fn void terminator()
 * \brief Terminate the process
 */
void terminator()
{
    printf(RED_COLOR "(PowerSupply: %d) killed" RESET_COLOR "\n", getpid());
    exit(EXIT_SUCCESS);
}

/**
 * \fn void signalHandler(int sig)
 * \brief Handle the signals
 * \param sig the signal
 */
void signalHandler(int sig)
{
    switch (sig)
    {
    case SIGTERM:
        terminator();
        break;
    }
}

/**
 * \fn void set_sigactions()
 * \brief Set the signal actions
 */
void set_sigactions()
{
    struct sigaction sa;
    sigset_t mask;

    CHECK(sigemptyset(&mask), -1, "sigemptyset(&mask)")

    sa.sa_handler = signalHandler;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGTERM, &sa, NULL);
    sigemptyset(&mask);
}

/**
 * \fn char* getAtomName(int atomicNumber)
 * \brief Get the atom name
 * \param atomicNumber the atomic number
 * \return the atom name
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
