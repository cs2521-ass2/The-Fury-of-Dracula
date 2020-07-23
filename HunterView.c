////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01   v1.1    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
    // TODO: ADD FIELDS HERE
    GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    HunterView new = malloc(sizeof(*new));
    if (new == NULL) {
        fprintf(stderr, "Couldn't allocate HunterView!\n");
        exit(EXIT_FAILURE);
    }
    new->gv = GvNew(pastPlays, messages);
    return new;
}

void HvFree(HunterView hv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GvFree(hv->gv);
    free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetRound(hv->gv);
}

Player HvGetPlayer(HunterView hv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetPlayer(hv->gv);
}

int HvGetScore(HunterView hv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetScore(hv->gv);
}

int HvGetHealth(HunterView hv, Player player)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetHealth(hv->gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetPlayerLocation(hv->gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    int numReturnedLocs = 0;
    Round current_round = HvGetRound(hv);
    bool canFree;
    PlaceId *last_moves = GvGetLastMoves(hv->gv, PLAYER_DRACULA, 6,
                            &numReturnedLocs, &canFree);
    for (int i = numReturnedLocs - 1; i >= 0; i--) {
        if (last_moves[i] >= MIN_REAL_PLACE && 
            last_moves[i] <= MAX_REAL_PLACE) {
            *round = current_round - numReturnedLocs + i;
            PlaceId last_location = last_moves[i];
            if (canFree)
                free(last_moves);
            return last_location;    
        }
    }
    if (canFree)
        free(last_moves);
    return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
