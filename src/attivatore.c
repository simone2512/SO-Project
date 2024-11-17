#include "../include/attivatore.h"

/**
 * \file attivatore.c
 * \brief Implementation of the activator
 */

int scissionRate;
int main(void)
{
    scissionRate = 1;
    setipcs();
    set_sigactions();
    srand(time(NULL));
    struct sembuf sops = {0, -1, 0};
    semop(ids->START, &sops, 1);
    struct sembuf sops2 = {0, 0, 0};
    semop(ids->START, &sops2, 1);
    printf(GREEN_COLOR "(Activator: %d)\n" RESET_COLOR, getpid());
    while (1)
    {
        for (int i = 0; i < scissionRate; i++)
        {
            reserveSem(ids->SEMCONFIGS, 0);
            if (configs->FLAGINIBITORE == 0)
            {
                attivaScissione();
                reserveSem(ids->SEMSTATS, 0);
                stats->activations++;
                stats->activations1s++;
                releaseSem(ids->SEMSTATS, 0);
            }
            else
            {
                double randomValue = (double)rand() / (double)RAND_MAX;
                if (randomValue <= 0.5)
                {
                    reserveSem(ids->SEMSTATS, 0);
                    struct timeval currentTime;
                    gettimeofday(&currentTime, NULL);
                    struct tm *localTime = localtime(&currentTime.tv_sec);
                    char timeString[15];
                    strftime(timeString, sizeof(timeString), "%H:%M:%S", localTime);
                    sprintf(timeString + strlen(timeString), ".%02ld", currentTime.tv_usec / 10000);
                    strcat(stats->logOperation, "[");
                    strcat(stats->logOperation, timeString);
                    strcat(stats->logOperation, "] ");
                    strcat(stats->logOperation, "Scisson Blocked\n");
                    releaseSem(ids->SEMSTATS, 0);
                }
                else
                {
                    attivaScissione();
                    reserveSem(ids->SEMSTATS, 0);
                    stats->activations++;
                    stats->activations1s++;
                    releaseSem(ids->SEMSTATS, 0);
                }
            }
            releaseSem(ids->SEMCONFIGS, 0);
            CHECK(nanosleep((const struct timespec[]){{0, 250000000L}}, NULL), -1, "nanosleep((const struct timespec[]){{0, 250000000L}}, NULL)")
        }
    }
}
/**
 * \fn static void setipcs()
 * \brief Set the IPCS
 */
static void setipcs()
{
    CHECK((id = shmget(SHMKEY, sizeof(Ids), 0666)), -1, " shmget(getppid(), ...)")
    CHECK((ids = shmat(id, NULL, SHM_RDONLY)), (void *)-1, "shmat(id, ...)")
    CHECK((pidatomi = msgget(ids->PIDATOMI, 0)), -1, "msgget(ids->PIDATOMI)")
    CHECK((numAtomId = shmat(ids->NUMATOMI, NULL, SHM_RDONLY)), (void *)-1, "shmat(ids->PIDATOMI)")
    CHECK((stats = shmat(ids->STATS, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)")
    CHECK((configs = shmat(ids->CONFIGS, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)")
}

/**
 * \fn static void attivaScissione()
 * \brief Activate the scission
 */
static void attivaScissione()
{
    Msg msg;
    reserveSem(ids->SEMPID, 0);
    reserveSem(ids->SEMNUMATOMI, 0);
    reserveSem(ids->SEMSTATS, 0);
    srand(time(NULL));
    int numRand = rand() % *numAtomId + numAtomId[5] + stats->scorie;
    while (numRand == numAtomId[2] || numRand == numAtomId[3] || numRand == numAtomId[4] || msgrcv(pidatomi, &msg, sizeof(int), numRand, IPC_NOWAIT) == -1)
    {
        numRand = rand() % *numAtomId + numAtomId[5] + stats->scorie;
    }
    printf(GREEN_COLOR "***atom pid for scission***: %d\n" RESET_COLOR, numRand);
    DO(kill(numRand, SIGUSR1), -1, "kill(pidSig, SIGUSR1)")
    releaseSem(ids->SEMSTATS, 0);
    releaseSem(ids->SEMPID, 0);
    releaseSem(ids->SEMNUMATOMI, 0);
}

/**
 * \fn static void terminator()
 * \brief Terminate the process
 */
void terminator()
{
    printf(GREEN_COLOR "(Activator: %d) killed\n" RESET_COLOR, getpid());
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
        terminator();
        break;
    }
}

/**
 * \fn static void set_sigactions()
 * \brief Set the sigactions
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
