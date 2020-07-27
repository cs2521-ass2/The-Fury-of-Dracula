////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
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

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct draculaView {
    // TODO: ADD FIELDS HERE
    GameView gv;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    DraculaView new = malloc(sizeof(*new));
    if (new == NULL) {
        fprintf(stderr, "Couldn't allocate DraculaView\n");
        exit(EXIT_FAILURE);
    }
    new->gv = GvNew(pastPlays, messages);
    return new;
}

void DvFree(DraculaView dv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GvFree(dv->gv);
    free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return GvGetTrapLocations(dv->gv, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

    PlaceId current_place = DvGetPlayerLocation(dv, PLAYER_DRACULA);
    Round current_round = DvGetRound(dv);
    
    // Hasn't made a move yet or invalid place
    if (current_round < 1 || current_place > MAX_REAL_PLACE 
        || current_place < MIN_REAL_PLACE) {
        *numReturnedMoves = 0;
        return NULL;
    }
    // All the locations that Dracula can go to
    int numReturnedLocs;
    PlaceId *location = DvWhereCanIGo(dv, &numReturnedLocs);
    
    // Last 5 moves
    bool canFree;
    int numReturned;
    PlaceId *history_moves = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5,
                        &numReturned, &canFree);
    // All the locations reachable by Dracula
    int num_reached;
    PlaceId *reached_location = GvGetReachable(dv->gv, PLAYER_DRACULA,
        current_round, current_place, &num_reached);
    
    bool hide_exist = false;
    bool double_back_exist = false;
    bool double_back[5] = {false};
    // To make sure whether double_back or hide moves exist in history_moves
    for (int i = 0; i < numReturned; i++) {
        if (history_moves[i] == HIDE) hide_exist = true;
        if (history_moves[i] == DOUBLE_BACK_1) double_back_exist = true;
        if (history_moves[i] == DOUBLE_BACK_2) double_back_exist = true;
        if (history_moves[i] == DOUBLE_BACK_3) double_back_exist = true;
        if (history_moves[i] == DOUBLE_BACK_4) double_back_exist = true;
        if (history_moves[i] == DOUBLE_BACK_5) double_back_exist = true;
        for (int j = 0; j < num_reached; j++) {
            // To make sure which double_back is available
            if (history_moves[i] == reached_location[j])
                double_back[numReturned - 1 - i] = true;
        }
    }
    if (canFree)
        free(history_moves);
    free(reached_location);
    if (double_back_exist && hide_exist && numReturnedLocs == 0) {
        // No moves available
        *numReturnedMoves = 0;
        return NULL;
    } else if (hide_exist && double_back_exist) {
        // Only move to locations
        *numReturnedMoves = numReturnedLocs;
        return location;
    } else if (hide_exist && numReturnedLocs == 0) {
        // Only double_back is available
        location = realloc(location, 5 * sizeof(PlaceId));
        int j = 0;
        for (int i = 0; i < 5; i++) {
            if (double_back[i]) {
                location[j] = DOUBLE_BACK_1 + i;
                j++;
            }
        }
        *numReturnedMoves = j;
        return location;
    } else if (double_back_exist && numReturnedLocs == 0) {
        // Only hide is available
        // whether in sea
        if (placeIdToType(current_place) == SEA) {
            *numReturnedMoves = 0;
            return NULL;
        }
        *numReturnedMoves = 1;
        location = realloc(location, sizeof(PlaceId));
        location[0] = HIDE;
        return location;
    } else if (double_back_exist) {
        // Only move to locations and hide
        // whether in sea
        if (placeIdToType(current_place) == SEA) {
            *numReturnedMoves = numReturnedLocs;
            return location;
        }
        *numReturnedMoves = numReturnedLocs + 1;
        location = realloc(location, (*numReturnedMoves) * sizeof(PlaceId));
        location[numReturnedLocs] = HIDE;
        return location;
    } else if (hide_exist) {
        // Only move to locations and double_back
        location = realloc(location, (numReturnedLocs + 5) * sizeof(PlaceId));
        int j = numReturnedLocs;
        for (int i = 0; i < 5; i++) {
            if (double_back[i]) {
                location[j] = DOUBLE_BACK_1 + i;
                j++;
            }
        }
        *numReturnedMoves = j;
        return location;
    } else if (numReturnedLocs == 0) {
        // Only double_back and hide
        // whether in sea
        if (placeIdToType(current_place) == SEA) {
            location = realloc(location, 5 * sizeof(PlaceId));
            int j = 0;
            for (int i = 0; i < 5; i++) {
                if (double_back[i]) {
                    location[j] = DOUBLE_BACK_1 + i;
                    j++;
                }
            }
            *numReturnedMoves = j;
            return location;
        }
        location = realloc(location, 6 * sizeof(PlaceId));
        int j = 0;
        for (int i = 0; i < 5; i++) {
            if (double_back[i]) {
                location[j] = DOUBLE_BACK_1 + i;
                j++;
            }
        }
        location[j] = HIDE;
        *numReturnedMoves = j + 1;
        return location;
    } else {
        // Move to locations and hide and double_back
        // whether in sea
        if (placeIdToType(current_place) == SEA) {
            location = realloc(location, (numReturnedLocs + 5) * 
                sizeof(PlaceId));
            int j = numReturnedLocs;
            for (int i = 0; i < 5; i++) {
                if (double_back[i]) {
                    location[j] = DOUBLE_BACK_1 + i;
                    j++;
                }
            }
            *numReturnedMoves = j;
            return location;
        }
        location = realloc(location, (numReturnedLocs + 6) * 
            sizeof(PlaceId));
        int j = numReturnedLocs;
        for (int i = 0; i < 5; i++) {
            if (double_back[i]) {
                location[j] = DOUBLE_BACK_1 + i;
                j++;
            }
        }
        location[j] = HIDE;
        *numReturnedMoves = j + 1;
        return location;
    }

}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return DvWhereCanTheyGo(dv, PLAYER_DRACULA, numReturnedLocs);
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return DvWhereCanTheyGoByType(dv, PLAYER_DRACULA, road, false, boat,
        numReturnedLocs);
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

    PlaceId current_place = DvGetPlayerLocation(dv, player);
    Round current_round = DvGetRound(dv);
    // Hasn't made a move
    if (current_round < 1 || current_place > MAX_REAL_PLACE 
        || current_place < MIN_REAL_PLACE) {
        *numReturnedLocs = 0;
        return NULL;
    }
    // For hunter, it is next round
    if (player != PLAYER_DRACULA)
        current_round += 1;
    
    PlaceId *place = GvGetReachable(dv->gv, player, current_round,
        current_place, numReturnedLocs);
    
    if (player == PLAYER_DRACULA) {
        // all the trials in the last 5 rounds
        int numReturned;
        bool canFree = true;
        PlaceId *history_moves = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5,
            &numReturned, &canFree);
        // check whether the place is in the trial
        int i = 0;
        while (i < *numReturnedLocs) {
            bool place_in_moves = false;
            for (int j = 0; j < numReturned; j++) {
                if (place[i] == history_moves[j]) {
                    place_in_moves = true;
                    break;
                }
            }
            if (place_in_moves) {
                //remove the current place, swap to last one
                *numReturnedLocs -= 1;
                place[i] = place[*numReturnedLocs];
                place = realloc(place, (*numReturnedLocs) * sizeof(PlaceId));
            } else {
                i++; 
            }
        }
        if (canFree)
            free(history_moves);
    }
    return place;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
    // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    PlaceId current_place = DvGetPlayerLocation(dv, player);
    Round current_round = DvGetRound(dv);
    // Hasn't made a move
    if (current_round < 1 || current_place > MAX_REAL_PLACE 
        || current_place < MIN_REAL_PLACE) {
        *numReturnedLocs = 0;
        return NULL;
    }
    // For hunter, it is next round
    if (player != PLAYER_DRACULA)
        current_round += 1;
    
    PlaceId *place = GvGetReachableByType(dv->gv, player, current_round,
        current_place, road, rail, boat, numReturnedLocs);
    
    if (player == PLAYER_DRACULA) {
        // all the trials in the last 5 rounds
        int numReturned;
        bool canFree = true;
        PlaceId *history_moves = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5,
            &numReturned, &canFree);
        // check whether the place is in the trial
        int i = 0;
        while (i < *numReturnedLocs) {
            bool place_in_moves = false;
            for (int j = 0; j < numReturned; j++) {
                if (place[i] == history_moves[j]) {
                    place_in_moves = true;
                    break;
                }
            }
            if (place_in_moves) {
                //remove the current place, swap to last one
                *numReturnedLocs -= 1;
                place[i] = place[*numReturnedLocs];
                place = realloc(place, (*numReturnedLocs) * sizeof(PlaceId));
            } else {
                i++; 
            }
        }
        if (canFree)
            free(history_moves);
    }
    return place;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
