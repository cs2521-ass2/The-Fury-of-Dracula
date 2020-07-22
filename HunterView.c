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
#include "Queue.h"
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
    PlaceId *last_locations;
    Round current_round = HvGetRound(hv);
    last_locations = GvGetLastLocations(hv->gv, PLAYER_DRACULA, 6,
                            &numReturnedLocs, false);
    for (int i = numReturnedLocs - 1; i >= 0; i--) {
        if (last_locations[i] >= MIN_REAL_PLACE && 
            last_locations[i] <= MAX_REAL_PLACE) {
            *round = current_round - numReturnedLocs + i;
            return last_locations[i];    
        }
    }
    return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
    PlaceId source = HvGetPlayerLocation(hv, player);
    int Max = PlaceId placeNameToId(Zurich);
    Round tmp_round = HvGetRound(hv);
    int *tmp_returnedLocs = 0;
    
    //make new queue for places to visit
    struct Queue* ToVisit = NewQueue(Max);
    //initialise 2D array of visited paths.
    //set all to -3 since it is not an existing PlaceId.
    PlaceId PathsVisited[Max][Max]; 
    for (i=0;i <=Max; i++){
	    for(j=0; j<=Max; j++){
		    PathsVisited[i][j] = -3;
        }
    }  
    PathsVisited[source][source] = source;
    enqueue(ToVisit, source);
    //while there is still places to visit, 
    //repeatedly call gvgetreachable to enqueue. 
    //if the destination is found, return its path.
    while (!isEmpty(ToVisit)) {
        PlaceId tmp_loc = dequeue(ToVisit);
        if (tmp_loc == dest) {
            for(int i=0;i<=Max && PathsVisited[tmp_loc][i] != -3; i++);
	        *pathLength = i;
	        return &PathsVisited[tmp_loc];
	    }
	    PlaceId *to_enqueue = gvgetreachable(hv, player, tmp_round, 
	                          tmp_loc, tmp_returnedLocs);
	    int i=0;
	    while (to_enqueue[i] <= MAX_REAL_PLACE && 
	                            to_enqueue[i] >= MIN_REAL_PLACE) {
	        int QueueCheck = isNotInQueue(ToVisit, to_enqueue[i])
	        if (QueueCheck == 1) {
	            enqueue(ToVisit, to_enqueue[i]);
	            i++;
	        } else {
	            i++;
	        }
	    }
        //copy and overlap the paths taken to get to a certain place from source.
	    for (int j=0; PathsVisited[tmp_loc][j] != -1; j++) {
            PathsVisited[tmp_loc][j] = PathsVisited[p][j];
            PathsVisited[p][j+1]=p;
        }
        tmp_round += 1;
    }
	      
    return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
    curr_player = HvGetPlayer(hv);
    return HvWhereCanTheyGo(hv,curr_player, numReturnedLocs);
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
    curr_player = HvGetPlayer(hv);
    return HvWhereCanTheyGoByType(hv, curr_player, road, rail, boat, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
    PlaceId curr_loc = GvGetPlayerLocation(hv->gv, player);
    Round curr_round = HvGetRound(hv);
    
    if (curr_round < 1 || curr_loc = NOWHERE) {    
        *numReturnedLocs = 0;
        return NULL;
    }
    
    if (player == PLAYER_DRACULA) {
        if (GvGetPlayerLocation (hv->gv, player) == CITY_UNKNOWN ||
            GvGetPlayerLocation (hv->gv, player) == SEA_UNKNOWN) {
            *numReturnedLocs = 0;
            return NULL;
        } else {
            curr_round += 1;        
        }
    }   
        
    PlaceId *place = GvGetReachable(hv->gv, player, curr_round, curr_loc, numReturnedLocs);
    return place;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
    PlaceId curr_loc = GvGetPlayerLocation(hv->gv, player);
    Round curr_round = HvGetRound(hv);
    
    if (curr_round < 1 || curr_loc = NOWHERE) {    
        *numReturnedLocs = 0;
        return NULL;
    }
    if (player == PLAYER_DRACULA) {
        if (GvGetPlayerLocation (hv->gv, player) == CITY_UNKNOWN ||
            GvGetPlayerLocation (hv->gv, player) == SEA_UNKNOWN) {
            *numReturnedLocs = 0;
            return NULL;
        } else {
            curr_round += 1;
        }       
    }        
    
    PlaceId *place = GvGetReachableByType(hv->gv, player, curr_round, 
                    road, rail, boat, curr_loc, numReturnedLocs);
    return place;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
