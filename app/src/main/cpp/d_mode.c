//
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//   Functions and definitions relating to the game type and operational
//   mode.
//

#include "doomtype.h"
#include "d_mode.h"

// Valid game mode/mission combinations, with the number of
// episodes/maps for each.

static struct {
    GameMission_t mission;
    GameMode_t mode;
    int episode;
    int map;
} valid_modes[] = {
    { pack_chex, shareware,  1, 5 },
    { doom,      shareware,  1, 9 },
    { doom,      registered, 3, 9 },
    { doom,      retail,     4, 9 },
    { doom2,     commercial, 1, 32 },
    { pack_tnt,  commercial, 1, 32 },
    { pack_plut, commercial, 1, 32 },
    { pack_hacx, commercial, 1, 32 },
    { heretic,   shareware,  1, 9 },
    { heretic,   registered, 3, 9 },
    { heretic,   retail,     5, 9 },
    { hexen,     commercial, 1, 60 },
    { strife,    commercial, 1, 34 },
};

boolean D_ValidEpisodeMap(GameMission_t mission, GameMode_t mode,
                          int episode, int map)
{
    // Hacks for Heretic secret episodes
    if (mission == heretic)
    {
        if (mode == retail && episode == 6)
        {
            return map >= 1 && map <= 3;
        }
        else if (mode == registered && episode == 4)
        {
            return map == 1;
        }
    }

    // Find the table entry for this mission/mode combination.

    for (int i = 0; i < ARRLEN(valid_modes); ++i)
    {
        if (mission == valid_modes[i].mission &&
            mode == valid_modes[i].mode)
        {
            return episode >= 1 && episode <= valid_modes[i].episode
                && map >= 1 && map <= valid_modes[i].map;
        }
    }

    // Unknown mode/mission combination
    return False;
}

// Get the number of valid episodes for the specified mission/mode.

int D_GetNumEpisodes(GameMission_t mission, GameMode_t mode)
{
    int episode;

    episode = 1;

    while (D_ValidEpisodeMap(mission, mode, episode, 1))
    {
        ++episode;
    }

    return episode - 1;
}

// Table of valid versions

static struct {
    GameMission_t mission;
    GameVersion_t version;
} valid_versions[] = {
    { doom,    exe_doom_1_9    },
    { doom,    exe_hacx        },
    { doom,    exe_ultimate    },
    { doom,    exe_final       },
    { doom,    exe_final2      },
    { doom,    exe_chex        },
    { heretic, exe_heretic_1_3 },
    { hexen,   exe_hexen_1_1   },
    { strife,  exe_strife_1_2  },
    { strife,  exe_strife_1_31 },
};

boolean D_ValidGameVersion(GameMission_t mission, GameVersion_t version)
{
    // All Doom variants can use the Doom versions.

    if (mission == doom2 || mission == pack_plut || mission == pack_tnt
     || mission == pack_hacx || mission == pack_chex)
    {
        mission = doom;
    }

    for (int i = 0; i < ARRLEN(valid_versions); ++i)
    {
        if (valid_versions[i].mission == mission 
         && valid_versions[i].version == version)
        {
            return True;
        }
    }

    return False;
}

char *D_GameMissionString(GameMission_t mission)
{
    switch (mission)
    {
    case none:
    default:
        return "none";
    case doom:
        return "doom";
    case doom2:
        return "doom2";
    case pack_tnt:
        return "tnt";
    case pack_plut:
        return "plutonia";
    case pack_hacx:
        return "hacx";
    case pack_chex:
        return "chex";
    case heretic:
        return "heretic";
    case hexen:
        return "hexen";
    case strife:
        return "strife";
    }
}
