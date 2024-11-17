#pragma once

typedef struct
{
    int CONFIGS;     // Id Shared mem for saving initial configs.
    int START;       // Id Sem for the initial sincronisation.
    int PIDATOMI;    // Key Message queue for saving atoms pid.
    int SEMPID;      // Id Sem for access to pid's shared memory.
    int NUMATOMI;    // Id Shared mem for the number of atoms.
    int SEMNUMATOMI; // Id sem for access to numatomi's shared memory.
    int STATS;       // Id Shared mem for saving stats.
    int SEMSTATS;    // Id sem for access to stats's shared memory.
    int SEMCONFIGS;  // Id sem for access to configs's shared memory.
} Ids;

typedef struct
{
    int N_ATOM_MAX;
    int ENERGY_DEMAND;
    int N_ATOM_INIT;
    int N_NUOVI_ATOMI;
    int MIN_N_ATOMICO;
    int SIM_DURATION;
    int ENERGY_EXPLODE_THRESHOLD;
    int STARTING_ENERGY;
    int STEP_ALIMENTAZIONE;
    int FLAGINIBITORE;
} Configs;

typedef struct
{
    int activations;
    int scissions;
    int totalProducedEnergy;
    int totalUsedEnergy;
    int scorie;
    int absorbedEnergy;
    char logOperation[100000];
    int totalEnergy;
    int activations1s;
    int scissions1s;
    int totalProducedEnergy1s;
    int totalUsedEnergy1s;
    int scorie1s;

} Stats;

typedef struct
{
    long mtype;
    int mpid;
} Msg;
