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
    struct Queue* bfs = NewQueue(NUM_REAL_PLACES);
    //array of predecessors.
    PlaceId Pred[NUM_REAL_PLACES];
    
    for(int i = 0; i < NUM_REAL_PLACES; i++)
        Pred[i] = NOWHERE;
    
    Pred[source] = source;
    enqueue(bfs, source);
    
   // printf("%d dest\n", dest);
   // printf("%d source\n", source);
    
    while (!isEmpty(bfs) && found == false) {
        PlaceId head = dequeue(bfs);
        //printf("%d\n", head);
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
            PlaceId *to_enqueue = GvGetReachable(hv->gv, hunter, tmp_round,
                head, &tmp_returnedLocs);
            for (int i = 0; i < tmp_returnedLocs; i++){
                if (Pred[to_enqueue[i]] == NOWHERE) {
                    enqueue(bfs, to_enqueue[i]);
                    Pred[to_enqueue[i]] = head;
                }
            }
            
        }
    }
    if (found) {
        int count = 0;
        PlaceId i = dest;
        while (i != NOWHERE && i != source) {
            count++;
            i = Pred[i];
        }
        
        i = dest;
        int j = count - 1;
        *pathLength = count;
        PlaceId *path = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
        while (j >= 0 && i != source) {
         //   printf("i = %d\n", i);
            path[j] = i;
            i = Pred[i];
            j--;
        }
       // printf("i = %d\n", i);
        return path;
    } else {
       // printf("not found\n");
        *pathLength = 0;
        return NULL;
    }
    
    
   /* PlaceId source = HvGetPlayerLocation(hv, hunter);
    Round tmp_round = HvGetRound(hv);
    int tmp_returnedLocs = 0;
    //make new queue for places to visit
    struct Queue* ToVisit = NewQueue(MAX_REAL_PLACE);
    //initialise 2D array of visited paths.
    //set all to -3 since it is not an existing PlaceId.
   // PlaceId PathsVisited[MAX_REAL_PLACE][MAX_REAL_PLACE];    
    
    PlaceId **PathsVisited = malloc((MAX_REAL_PLACE + 1) * sizeof(PlaceId *)); 
    for (int i = 0; i <= MAX_REAL_PLACE; i++)
        PathsVisited[i] = malloc((MAX_REAL_PLACE + 1) * sizeof(PlaceId));
    for (int i=0;i <=MAX_REAL_PLACE; i++){
        for(int j=0; j<=MAX_REAL_PLACE; j++){
            PathsVisited[i][j] = NOWHERE;
        }
    }  
    PathsVisited[source][0] = source;
    enqueue(ToVisit, source);
    //while there is still places to visit, 
    //repeatedly call gvgetreachable to enqueue. 
    //if the destination is found, return its path.
    while (!isEmpty(ToVisit)) {
        PlaceId tmp_loc = dequeue(ToVisit);
        if (tmp_loc == dest) {
            for (int i = 0; i <= MAX_REAL_PLACE && PathsVisited[tmp_loc][i] != NOWHERE ; i++) {
                *pathLength = i + 1;
            }
            return PathsVisited[tmp_loc];
        }
        PlaceId *to_enqueue = GvGetReachable(hv->gv, hunter, tmp_round, 
                              tmp_loc, &tmp_returnedLocs);
      for(int i = 0; i < tmp_returnedLocs; i++) {
            int QueueCheck = isNotInQueue(ToVisit, to_enqueue[i]);
            if (QueueCheck == 1) {
                enqueue(ToVisit, to_enqueue[i]);
                //Copy the path of tmp_loc into path to to_enqueue[i]  
                int j = 0;
                for (; PathsVisited[tmp_loc][j] <= MAX_REAL_PLACE &&  PathsVisited[tmp_loc][j] >= MIN_REAL_PLACE ; j++) 
                    PathsVisited[to_enqueue[i]][j] = PathsVisited[tmp_loc][j];
                PathsVisited[to_enqueue[i]][j+1] = to_enqueue[i];       
            }
            tmp_round += 1;
        }
    }
    return NULL; */
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

    if (curr_loc == NOWHERE) {    
        *numReturnedLocs = 0;
        return NULL;
    }

    if (player == PLAYER_DRACULA) {
        if (curr_loc == CITY_UNKNOWN || curr_loc == SEA_UNKNOWN) {
            *numReturnedLocs = 0;
            return NULL;
        } 
    }   
    
    if (player < curr_player)
         curr_round += 1;   

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

    if (curr_loc == NOWHERE) {    
        *numReturnedLocs = 0;
        return NULL;
    }

    if (player == PLAYER_DRACULA) {
        if (curr_loc == CITY_UNKNOWN || curr_loc == SEA_UNKNOWN) {
            *numReturnedLocs = 0;
            return NULL;
        } 
    }   
    
    if (player < curr_player)
         curr_round += 1;      

    PlaceId *place = GvGetReachableByType(hv->gv, player, curr_round, curr_loc,
                    road, rail, boat, numReturnedLocs);
    return place;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

