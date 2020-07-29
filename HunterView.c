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
#include "HVQueue.h"


struct hunterView {
    GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
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
    GvFree(hv->gv);
    free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
    return GvGetRound(hv->gv);
}

Player HvGetPlayer(HunterView hv)
{
    return GvGetPlayer(hv->gv);
}

int HvGetScore(HunterView hv)
{
    return GvGetScore(hv->gv);
}

int HvGetHealth(HunterView hv, Player player)
{
    return GvGetHealth(hv->gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
    return GvGetPlayerLocation(hv->gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
    return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
    int numReturnedLocs = 0;
    Round current_round = HvGetRound(hv);
    bool canFree;
    PlaceId *last_moves = GvGetLastMoves(hv->gv, PLAYER_DRACULA, 6,
                            &numReturnedLocs, &canFree);
    
    // Check whether the last 6 moves are real locations
    for (int i = numReturnedLocs - 1; i >= 0; i--) {
        if (last_moves[i] >= MIN_REAL_PLACE && 
            last_moves[i] <= MAX_REAL_PLACE) {
            // Change the round and return the last real location
            *round = current_round - numReturnedLocs + i;
            PlaceId last_location = last_moves[i];
            if (canFree)
                free(last_moves);
            return last_location;    
        }
    }
    if (canFree)
        free(last_moves);
    // Not found
    return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
    PlaceId source = HvGetPlayerLocation(hv, hunter);
    Round tmp_round = HvGetRound(hv);
    int tmp_returnedLocs = 0;
    bool found = false;
    
    // use bfs to tranverse the graph
    struct Queue* bfs = NewQueue(NUM_REAL_PLACES);
    // array of predecessors.
    PlaceId Pred[NUM_REAL_PLACES]; 
    for(int i = 0; i < NUM_REAL_PLACES; i++)
        Pred[i] = NOWHERE;
    
    Pred[source] = source;
    enqueue(bfs, source);
    // save the round of the source node
    Round save = tmp_round;
    while (!isEmpty(bfs) && found == false) {
        PlaceId head = dequeue(bfs);
        // level means the pathlength from the head to the source
        int level = 0;
        PlaceId current = head;
        while (current != source) {
            level++;
            current = Pred[current];
        }
        tmp_round += level;     
        if (head == dest) {
            found = true;
        } else {
            // all the reachable locations
            PlaceId *to_enqueue = GvGetReachable(hv->gv, hunter, tmp_round,
                head, &tmp_returnedLocs);
            for (int i = 0; i < tmp_returnedLocs; i++){
                if (Pred[to_enqueue[i]] == NOWHERE) {
                    // add to the queue and link
                    enqueue(bfs, to_enqueue[i]);
                    Pred[to_enqueue[i]] = head;
                }
            }
            free(to_enqueue);
        }
        tmp_round = save;
    }
    dropHunterQueue(bfs);
    
    if (found) {
        // calculate the pathLength
        int count = 0;
        PlaceId i = dest;
        while (i != NOWHERE && i != source) {
            count++;
            i = Pred[i];
        }  
        i = dest;
        int j = count - 1;
        *pathLength = count;
        // store the shortest path
        PlaceId *path = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
        while (j >= 0 && i != source) {
            path[j] = i;
            i = Pred[i];
            j--;
        }
        return path;
    } else {
        // not found
        *pathLength = 0;
        return NULL;
    }
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
    Player curr_player = HvGetPlayer(hv);
    return HvWhereCanTheyGo(hv,curr_player, numReturnedLocs);
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
    Player curr_player = HvGetPlayer(hv);
    return HvWhereCanTheyGoByType(hv, curr_player, road, rail, boat, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
    PlaceId curr_loc = GvGetPlayerLocation(hv->gv, player);
    Round curr_round = HvGetRound(hv);
    Player curr_player = HvGetPlayer(hv);
    // Hasn't made a move
    if (curr_loc == NOWHERE) {    
        *numReturnedLocs = 0;
        return NULL;
    }
    // place not revealed
    if (player == PLAYER_DRACULA) {
        if (curr_loc == CITY_UNKNOWN || curr_loc == SEA_UNKNOWN) {
            *numReturnedLocs = 0;
            return NULL;
        } 
    }   
    // next move
    if (player < curr_player) {
         curr_round += 1; 
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
    Player curr_player = HvGetPlayer(hv);
    // Hasn't made a move
    if (curr_loc == NOWHERE) {    
        *numReturnedLocs = 0;
        return NULL;
    }
    // place not revealed
    if (player == PLAYER_DRACULA) {
        if (curr_loc == CITY_UNKNOWN || curr_loc == SEA_UNKNOWN) {
            *numReturnedLocs = 0;
            return NULL;
        } 
    }   
    // next move
    if (player < curr_player)
         curr_round += 1;      

    PlaceId *place = GvGetReachableByType(hv->gv, player, curr_round, curr_loc,
                    road, rail, boat, numReturnedLocs);
    return place;
}

