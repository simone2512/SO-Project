#include "../include/inputReader.h"
/**
 * \file inputReader.c
 * \brief Implementation of the input reader
 */

/**
 * \fn int is_number(const char* s)
 * \brief Check if a string is a number
 * \param s The string
 * \return 1 if the string is a number, 0 otherwise
 */
int is_number(const char *s)
{
    int i = 0;
    int p = 0;
    while (s[i] != '\0')
        switch (s[i++])
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            break;
        case '.':
            if (!p)
                p = 1;
            else
                return 0;
            break;
        default:
            return 0;
        }
    return 1;
}

/**
 * \fn int read_input(char* path, Configs* configs)
 * \brief Read the input from a file
 * \param path The path of the file
 * \param configs The configurations
 * \return 0 if the input is correct, -1 otherwise
 */
int read_input(char *path, Configs *configs)
{

    FILE *f;
    char tokens[MAX_LINE][MAX_TOKEN];
    int i;
    int nt = 0;
    int commento = 0;

    f = fopen(path, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Al file %s;\nErrore al comando fopen(%s): errno = %d (%s);\n", __FILE__, path, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (memset(configs, -1, sizeof(Configs)) == NULL)
    {
        fprintf(stderr, "Al file %s;\nErrore al comando memset: errno = %d (%s);\n", __FILE__, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (!feof(f))
    {
        fscanf(f, "%s", tokens[nt]);
        if (!strcmp(tokens[nt], "/*"))
            commento = 1;
        else if (!strcmp(tokens[nt], "*/"))
            commento = 0;
        if (!commento)
            if (strcmp(tokens[nt], "=") && strcmp(tokens[nt], ":"))
                nt++;
    }

    for (i = 0; i < nt - 1; i++)
    {

        if (!strcmp(tokens[i], "N_ATOM_MAX"))
        {
            if (is_number(tokens[i + 1]))
                configs->N_ATOM_MAX = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "ENERGY_DEMAND"))
        {
            if (is_number(tokens[i + 1]))
                configs->ENERGY_DEMAND = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "N_ATOM_INIT"))
        {
            if (is_number(tokens[i + 1]))
                configs->N_ATOM_INIT = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "N_NUOVI_ATOMI"))
        {
            if (is_number(tokens[i + 1]))
                configs->N_NUOVI_ATOMI = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "MIN_N_ATOMICO"))
        {
            if (is_number(tokens[i + 1]))
                configs->MIN_N_ATOMICO = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "SIM_DURATION"))
        {
            if (is_number(tokens[i + 1]))
                configs->SIM_DURATION = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "ENERGY_EXPLODE_THRESHOLD"))
        {
            if (is_number(tokens[i + 1]))
                configs->ENERGY_EXPLODE_THRESHOLD = strtod(tokens[i + 1], NULL);
        }
        else if (!strcmp(tokens[i], "STARTING_ENERGY"))
        {
            if (is_number(tokens[i + 1]))
                configs->STARTING_ENERGY = (int)strtol(tokens[i + 1], NULL, 10);
        }
        else if (!strcmp(tokens[i], "STEP_ALIMENTAZIONE"))
        {
            if (is_number(tokens[i + 1]))
                configs->STEP_ALIMENTAZIONE = (int)strtol(tokens[i + 1], NULL, 10);
        }
    }

    fclose(f);

    if (configs->N_ATOM_MAX < 1 || configs->ENERGY_DEMAND > configs->STARTING_ENERGY || configs->ENERGY_DEMAND < 1 || configs->N_ATOM_INIT < 1 || configs->MIN_N_ATOMICO < 1 || configs->N_NUOVI_ATOMI < 1 || configs->SIM_DURATION <= 0 || configs->ENERGY_EXPLODE_THRESHOLD < 1 || configs->STARTING_ENERGY < 1)
    {
        printf("Errore nella lettura degli input\n"
               "Usage:\n"
               "N_ATOM_MAX: integer > 0\n"
               "ENERGY_DEMAND : integer < STARTING_ENERGY\n"
               "N_ATOM_INIT: integer > 0\n"
               "N_NUOVI_ATOMI: integer > 0\n"
               "MIN_N_ATOMICO: integer > 0\n"
               "SIM_DURATION: integer > 0\n"
               "ENERGY_EXPLODE_THRESHOLD: integer > 0\n"
               "STARTING_ENERGY: integer > 0\n");
        return -1;
    }

    return 0;
}

/**
 * \fn int set_input(char* path, Configs* configs)
 * \brief Set the input
 * \param path The path of the file
 * \param configs The configurations
 * \return 0 if the input is correct, -1 otherwise
 */
int set_input(char *path, Configs *configs)
{
    int c;
    int res = 1;
    int redo = 1;
    char ini;

    while (redo)
    {
        redo = 0;
        printf("Vuoi attivare l'inibitore? [y/n]\n");

        fscanf(stdin, "%c", &ini);
        if (ini == 'y')
        {
            configs->FLAGINIBITORE = 1;
            printf("Inibitore attivo\n");
        }
        else
        {
            configs->FLAGINIBITORE = 0;
            printf("Inibitore non attivo\n");
        }

        printf("Scegliere la configurazione di input:\n"
               "-0 Running to explode\n"
               "-1 Fast BlackOut\n"
               "-2 Timeout simulation\n"
               "-3 10 min simulation\n"
               "-4 Lots of atoms\n"
               "-5 Custom Configuration\n"
               "-6 Exit\n\n");

        fscanf(stdin, "%d", &c);

        switch (c)
        {
        case 0:
            configs->N_ATOM_MAX = 118;
            configs->ENERGY_DEMAND = 5;
            configs->N_ATOM_INIT = 100;
            configs->N_NUOVI_ATOMI = 1;
            configs->MIN_N_ATOMICO = 1;
            configs->SIM_DURATION = 200;
            configs->ENERGY_EXPLODE_THRESHOLD = 5000;
            configs->STARTING_ENERGY = 1000;
            configs->STEP_ALIMENTAZIONE = 3;
            return 1;
            break;
        case 1:
            configs->N_ATOM_MAX = 118;
            configs->ENERGY_DEMAND = 900;
            configs->N_ATOM_INIT = 10;
            configs->N_NUOVI_ATOMI = 1;
            configs->MIN_N_ATOMICO = 1;
            configs->SIM_DURATION = 200;
            configs->ENERGY_EXPLODE_THRESHOLD = 10000;
            configs->STARTING_ENERGY = 1000;
            configs->STEP_ALIMENTAZIONE = 3;
            return 1;
            break;
        case 2:
            configs->N_ATOM_MAX = 118;
            configs->ENERGY_DEMAND = 50;
            configs->N_ATOM_INIT = 10;
            configs->N_NUOVI_ATOMI = 1;
            configs->MIN_N_ATOMICO = 1;
            configs->SIM_DURATION = 10;
            configs->ENERGY_EXPLODE_THRESHOLD = 850000;
            configs->STARTING_ENERGY = 1000;
            configs->STEP_ALIMENTAZIONE = 3;
            return 1;
            break;
        case 3:
            configs->N_ATOM_MAX = 118;
            configs->ENERGY_DEMAND = 50;
            configs->N_ATOM_INIT = 10;
            configs->N_NUOVI_ATOMI = 1;
            configs->MIN_N_ATOMICO = 1;
            configs->SIM_DURATION = 600;
            configs->ENERGY_EXPLODE_THRESHOLD = 5000000;
            configs->STARTING_ENERGY = 1000;
            configs->STEP_ALIMENTAZIONE = 3;
            return 1;
            break;
        case 4:
            configs->N_ATOM_MAX = 118;
            configs->ENERGY_DEMAND = 50;
            configs->N_ATOM_INIT = 5000;
            configs->N_NUOVI_ATOMI = 1;
            configs->MIN_N_ATOMICO = 1;
            configs->SIM_DURATION = 600;
            configs->ENERGY_EXPLODE_THRESHOLD = 5000000;
            configs->STARTING_ENERGY = 1000;
            configs->STEP_ALIMENTAZIONE = 3;
            return 1;
            break;
        case 5:
            res = read_input(path, configs);
            break;
        case 6:
            return 0;
        default:
            printf("Inserire un numero tra 0 e 5\n");
            redo = 1;
        }
    }

    return res;
}
