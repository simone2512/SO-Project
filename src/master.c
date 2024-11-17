#include "../include/master.h"
/**
 * \file master.c
 * \brief Implementation of the master
 */

int main(void)
{
    setIpcs();
    set_sigactions();
    atomsCreation(configs->N_ATOM_INIT);
    powerSupplyCreation();
    activatorCreation();
    InhibitorCreation();
    reserveSem(ids->SEMSTATS, 0);
    stats->totalEnergy = configs->STARTING_ENERGY;
    releaseSem(ids->SEMSTATS, 0);
    int n = configs->SIM_DURATION;
    printf(YELLOW_COLOR "(MASTER) Simulation duration: %d\n" RESET_COLOR, n);
    alarm(n);
    startSimulation();
}

/**
 * \fn static void startSimulation()
 * \brief Start the simulation
 */
void startSimulation()
{
    struct sembuf sops = {0, -1, 0};
    semop(ids->START, &sops, 1);
    struct sembuf sops2 = {0, 0, 0};
    semop(ids->START, &sops2, 1);
    printf(YELLOW_COLOR "(MASTER) Simulation started\n" RESET_COLOR);

    while (1)
    {
        if (stats->totalEnergy >= configs->ENERGY_EXPLODE_THRESHOLD)
        {
            printf(YELLOW_COLOR "\n\n(MASTER) EXPLODE\n" RESET_COLOR);
            printStatistics();
            DO(kill(0, SIGTERM), -1, "kill(0, SIGALRM)")
            exit(EXIT_SUCCESS);
        }
        else if (stats->totalEnergy <= 0)
        {
            printf(YELLOW_COLOR "\n\n(MASTER) BLACKOUT \n" RESET_COLOR);
            printStatistics();
            DO(kill(0, SIGTERM), -1, "kill(0, SIGALRM)")
            exit(EXIT_SUCCESS);
        }
        reserveSem(ids->SEMSTATS, 0);
        stats->totalEnergy -= configs->ENERGY_DEMAND;
        stats->totalUsedEnergy += configs->ENERGY_DEMAND;
        releaseSem(ids->SEMSTATS, 0);

        printStatistics();
        reserveSem(ids->SEMSTATS, 0);
        stats->totalProducedEnergy1s = 0;
        stats->scorie1s = 0;
        stats->activations1s = 0;
        stats->totalUsedEnergy1s = configs->ENERGY_DEMAND;
        stats->scissions1s = 0;
        releaseSem(ids->SEMSTATS, 0);
        nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
    }
}

/**
 * \fn static void handle_sigalrm()
 * \brief Handle the SIGALRM signal
 */
void handle_sigalrm()
{
    printf(YELLOW_COLOR "\n\n(MASTER) TIMEOUT Simulation time is over\n" RESET_COLOR);
    printStatistics();
    DO(kill(0, SIGTERM), -1, "kill(0, SIGTERM)")
}

/**
 * \fn static void handle_sigTerm()
 * \brief Handle the SIGTERM signal
 */
void handle_sigTerm()
{
    removeIpcs();
    DO(kill(0, SIGTERM), -1, "kill(0, SIGTERM)")
    exit(EXIT_SUCCESS);
}

/**
 * \fn static void signalHandler(int sig)
 * \brief Handle the signals
 * \param sig the signal
 */
void signalHandler(int sig)
{
    switch (sig)
    {
    case SIGTERM:
        handle_sigTerm();
        break;
    case SIGALRM:
        handle_sigalrm();
        break;
    }
}

/**
 * \fn static void atomsCreation(int numAtomi)
 * \brief Create the atoms
 * \param numAtomi the number of atoms
 */
static void atomsCreation(int numAtomi)
{
    for (int i = 0; i < numAtomi; i++)
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

            // printf(BOLD_MAGENTA_COLOR "(MASTER)Atom child process %d, atomic number: %d" RESET_COLOR "\n", getpid(), numAtomicoInt);
            struct sembuf sops = {0, -1, 0};
            semop(ids->START, &sops, 1);

            execv(argp[0], argp);
            perror("execv");
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            if (*numAtomId == 0)
                numAtomId[5] = pid;
            reserveSem(ids->SEMPID, 0);
            reserveSem(ids->SEMNUMATOMI, 0);
            Msg msg = {pid, pid};
            // printf("PId: %d\n", pid);
            CHECK(msgsnd(pidatomi, &msg, sizeof(pid_t), 0), -1, "msgsnd(pidatomi, &msg, sizeof(pid_t), 0)")
            *numAtomId = *numAtomId + 1;
            releaseSem(ids->SEMNUMATOMI, 0);
            releaseSem(ids->SEMPID, 0);
        }
        else if (pid < 0)
        {
            printf(YELLOW_COLOR "\n\n(MASTER) MELTDOWN\n\n" RESET_COLOR);
            DO(kill(0, SIGTERM), -1, "kill(0, SIGTERM)")
            exit(EXIT_SUCCESS);
        }
    }
}

/**
 * \fn static void powerSupplyCreation()
 * \brief Create the power supply
 */
static void powerSupplyCreation()
{

    pid_t pid = fork();

    if (pid == 0)
    {
        char *argp[2];
        argp[0] = ALIMENTATORE_PATH;
        argp[1] = NULL;

        printf(BOLD_MAGENTA_COLOR "(MASTER)Power Supply child process: %d" RESET_COLOR "\n", getpid());
        numAtomId[3] = getpid();
        execv(argp[0], argp);
        perror("execv");
        exit(EXIT_SUCCESS);
    }
    else if (pid < 0)
    {
        printf(YELLOW_COLOR "\n\n(MASTER) MELTDOWN\n\n" RESET_COLOR);
        DO(kill(0, SIGTERM), -1, "kill(0, SIGTERM)")
        exit(EXIT_SUCCESS);
    }
}

/**
 * \fn static void activatorCreation()
 * \brief Create the activator
 */
static void activatorCreation()
{

    pid_t pid = fork();

    if (pid == 0)
    {
        char *argp[2];
        argp[0] = ATTIVATORE_PATH;
        argp[1] = NULL;
        printf(BOLD_MAGENTA_COLOR "(MASTER)Activator child process: %d " RESET_COLOR "\n", getpid());

        numAtomId[2] = getpid();
        execv(argp[0], argp);
        perror("execv");
        exit(EXIT_SUCCESS);
    }
    else if (pid < 0)
    {
        printf(YELLOW_COLOR "\n\n(MASTER) MELTDOWN\n\n" RESET_COLOR);
        DO(kill(0, SIGTERM), -1, "kill(0, SIGTERM)")
        exit(EXIT_SUCCESS);
    }
}

/**
 * \fn static void InhibitorCreation()
 * \brief Create the inhibitor
 */
static void InhibitorCreation()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        char *argp[2];
        argp[0] = INIBITORE_PATH;
        argp[1] = NULL;

        printf(BOLD_MAGENTA_COLOR "(MASTER)Inhibitor child process: %d" RESET_COLOR "\n", getpid());
        numAtomId[4] = getpid();
        execv(argp[0], argp);
        perror("execv");
        exit(EXIT_SUCCESS);
    }
    else if (pid < 0)
    {
        printf(YELLOW_COLOR "\n\n(MASTER) MELTDOWN\n\n" RESET_COLOR);
        DO(kill(0, SIGTERM), -1, "kill(0, SIGTERM)")
        exit(EXIT_SUCCESS);
    }
}

/**
 * \fn static void printStatistics()
 * \brief Print the statistics
 */
static void printStatistics()
{
    printf(BOLD_MAGENTA_COLOR "\n(MASTER)Statistics: ...\nnumber of atctivation by the activator: %d\nnumer of scissions: %d\nproduced energy quantity: %d\nconsumed energy quantity: %d\nnumber of scorie: %d\nabsorbed energy quantity: %d\nenergy of the sistem: %d\nnumber of activations (last second): %d\nnumber of scissions (last second): %d\nproduced energy (last second): %d\nconsumed energy (last second): %d\nnumber of scorie (last second): %d\n\n" RESET_COLOR, stats->activations, stats->scissions, stats->totalProducedEnergy, stats->totalUsedEnergy, stats->scorie, stats->absorbedEnergy, stats->totalEnergy, stats->activations1s, stats->scissions1s, stats->totalProducedEnergy1s, stats->totalUsedEnergy1s, stats->scorie1s);
    printf(BOLD_MAGENTA_COLOR "Inhibitor Log:\n%s\n" RESET_COLOR, stats->logOperation);
}

/**
 * \fn static void setIpcs()
 * \brief Set the IPCS
 */
static void setIpcs()
{
    int i;
    CHECK((id = shmget(SHMKEY, sizeof(Ids), IPC_CREAT | IPC_EXCL | 0666)), -1, " shmget(SHMKEY, ...)")
    CHECK((ids = shmat(id, NULL, 0)), (void *)-1, "shmat(id, ...)")

    ids->PIDATOMI = ftok("src/master.c", 'A');

    CHECK((ids->CONFIGS = shmget(IPC_PRIVATE, sizeof(Configs), IPC_CREAT | IPC_EXCL | 0666)), -1, "shmget(IPC_PRIVATE, .1..)")
    CHECK((configs = shmat(ids->CONFIGS, NULL, 0)), (void *)-1, "shmat(configs, ...)")

    i = set_input(INPUT_PATH, configs);
    if (i < 0)
        handle_sigTerm();

    CHECK((pidatomi = msgget(ids->PIDATOMI, IPC_CREAT | IPC_EXCL | 0666)), -1, "msgget(ids->PIDATOMI)")

    CHECK((ids->NUMATOMI = shmget(IPC_PRIVATE, sizeof(int) * 6, IPC_CREAT | IPC_EXCL | 0666)), -1, "shmget(IPC_PRIVATE, 2...)")
    CHECK((numAtomId = shmat(ids->NUMATOMI, NULL, 0)), (void *)-1, "shmat(numAtomId, ...)")

    CHECK((ids->STATS = shmget(IPC_PRIVATE, sizeof(Stats), IPC_CREAT | IPC_EXCL | 0666)), -1, " shmget(IPC_PRIVATE, 3...)")
    CHECK((stats = shmat(ids->STATS, NULL, 0)), (void *)-1, "shmat(stats, ...)")

    CHECK((ids->START = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666)), -1, " shmget(IPC_PRIVATE, .4..)")
    semctl(ids->START, 0, SETVAL, configs->N_ATOM_INIT + 4);

    CHECK((ids->SEMNUMATOMI = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666)), -1, "shmget(IPX_PRIVATE, ..5.)")
    semctl(ids->SEMNUMATOMI, 0, SETVAL, SEM_VAL_INIT);

    CHECK((ids->SEMPID = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666)), -1, " shmget(IPC_PRIVATE, 6...)")
    semctl(ids->SEMPID, 0, SETVAL, SEM_VAL_INIT);

    CHECK((ids->SEMSTATS = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666)), -1, " shmget(IPC_PRIVATE, .7..)")
    semctl(ids->SEMSTATS, 0, SETVAL, SEM_VAL_INIT);

    CHECK((ids->SEMCONFIGS = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666)), -1, " shmget(IPC_PRIVATE, ..8.)")
    semctl(ids->SEMCONFIGS, 0, SETVAL, SEM_VAL_INIT);
}

/**
 * \fn static void removeIpcs()
 * \brief Remove the IPCS
 */
void removeIpcs()
{

    shmdt(numAtomId);
    shmdt(stats);
    shmdt(configs);

    shmctl(ids->NUMATOMI, IPC_RMID, NULL);
    shmctl(ids->STATS, IPC_RMID, NULL);
    shmctl(ids->CONFIGS, IPC_RMID, NULL);

    semctl(ids->START, 0, IPC_RMID, 0);
    semctl(ids->SEMNUMATOMI, 0, IPC_RMID, 0);
    semctl(ids->SEMPID, 0, IPC_RMID, 0);
    semctl(ids->SEMSTATS, 0, IPC_RMID, 0);
    semctl(ids->SEMCONFIGS, 0, IPC_RMID, 0);

    msgctl(pidatomi, IPC_RMID, NULL);

    shmdt(ids);
    shmctl(id, IPC_RMID, NULL);
}

/**
 * \fn static void set_sigactions()
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
    sigaction(SIGALRM, &sa, NULL);
    sigemptyset(&mask);
}