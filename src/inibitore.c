#include "../include/inibitore.h"
/**
 * \file inibitore.c
 * \brief Implementation of the inhibitor
 */
int main(void)
{
    setipcs();
    set_sigactions();
    struct sembuf sops = {0, -1, 0};
    semop(ids->START, &sops, 1);
    struct sembuf sops2 = {0, 0, 0};
    semop(ids->START, &sops2, 1);
    printf(WHITE_COLOR "(Inhibitor: %d)" RESET_COLOR "\n", getpid());
    while (1)
    {
        pause();
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
    CHECK((configs = shmat(ids->CONFIGS, NULL, 0)), (void *)-1, "shmat(ids->PIDATOMI)")
}

/**
 * \fn void terminator()
 * \brief Terminate the process
 */
void terminator()
{
    printf(WHITE_COLOR "(Inhibitor: %d) killed" RESET_COLOR "\n", getpid());
    exit(EXIT_SUCCESS);
}

/**
 * \fn void handle_sigInt()
 * \brief Handle the SIGINT
 */
void handle_sigUsr1()
{
    reserveSem(ids->SEMCONFIGS, 0);
    if (configs->FLAGINIBITORE == 0)
    {
        configs->FLAGINIBITORE = 1;
        printf(YELLOW_COLOR "(Inhibitor) Inhibitor activated\n" RESET_COLOR);
    }
    else
    {
        configs->FLAGINIBITORE = 0;
        printf(YELLOW_COLOR "(Inhibitor) Inhibitor deactivated\n" RESET_COLOR);
    }
    releaseSem(ids->SEMCONFIGS, 0);
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
    case SIGUSR1:
        handle_sigUsr1();
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
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigemptyset(&mask);
}
