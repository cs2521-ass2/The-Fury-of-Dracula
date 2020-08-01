////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01   v1.1    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Places.h"


static PlaceId *hunterPossiblePlaces(DraculaView dv, int *possible_places);


void decideDraculaMove(DraculaView dv)
{
    Round current_round = DvGetRound(dv);
    int possible_places = 0;
    PlaceId *dangerousPlaces = hunterPossiblePlaces(dv, &possible_places);
    bool dangerous;
    if (current_round == 0) {
        PlaceId initial_places[NUM_REAL_PLACES];
        for (int i = 0; i < NUM_REAL_PLACES; i++) {
            initial_places[i] = NOWHERE;
        }
        int numMoves = 0;
        for (int i = MIN_REAL_PLACE; i <= MAX_REAL_PLACE; i++) {
            dangerous = false;
            for (int j = 0; j < possible_places; j++) {
                if (i == dangerousPlaces[j] && placeIdToType(i) != SEA) {
                    dangerous = true;
                    break;
                }
            }
            if (!dangerous) {
                initial_places[numMoves++] = i;
            }
        }
        srandom(time(NULL));
        registerBestPlay(placeIdToAbbrev(initial_places[random() % numMoves]), 
            "Wryyyyyyyyyyyyy!");
        return;
    } else {
        int numReturnedMoves;
        PlaceId *moves = DvGetValidMoves(dv, &numReturnedMoves);
        if (DvGetHealth(dv, PLAYER_DRACULA) <= 10 || numReturnedMoves == 0){
            registerBestPlay("TP", "Bye bye");
            return;
        }
            
        int i = 0;
        while (i < numReturnedMoves) {
            dangerous = false;
            for (int j = 0; j < possible_places; j++) {
                if (moves[i] == dangerousPlaces[j] && placeIdToType(moves[i]) != SEA) {
                    dangerous = true;
                    break;
                }
            }
            if (dangerous) {
                moves[i] = moves[--numReturnedMoves];
            } else {
                i++;
            }
        }
        if (numReturnedMoves == 0) {
            registerBestPlay("TP", "Bye bye");
            return;
        }
        srandom(time(NULL));
        registerBestPlay(placeIdToAbbrev(moves[random() % numReturnedMoves]), 
            "Mwahahahaha");
        return;
    }
}





static PlaceId *hunterPossiblePlaces(DraculaView dv, int *possible_places) {
    // dangerousPlaces
    PlaceId *places = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
    for (int i = 0; i < NUM_REAL_PLACES; i++) {
        places[i] = NOWHERE;
    }
    // store current places of hunters
    PlaceId currG = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
    places[0] = currG;
    *possible_places += 1;
    PlaceId currS = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
    if (currS != currG) {
        places[1] = currS;
        *possible_places += 1;
    }
    PlaceId currH = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
    bool exist = false;
    int i;
    for (i = 0; places[i] != NOWHERE; i++) {
        if (places[i] == currH) {
            exist = true;
            break;
        }
    }
    if (!exist) {
        places[i] = currH;
        *possible_places += 1;
    }
    PlaceId currM = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);
    exist = false;
    for (i = 0; places[i] != NOWHERE; i++) {
        if (places[i] == currM) {
            exist = true;
            break;
        }
    }
    if (!exist) {
        places[i] = currM;
        *possible_places += 1;
    }
    // store places that hunters can go to
    int numReturnedLocs;
    PlaceId *playerG = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numReturnedLocs);
    for (int j = 0; j < numReturnedLocs; j++) {
        exist = false;
        for (i = 0; places[i] != NOWHERE && i < NUM_REAL_PLACES; i++) {
            if (places[i] == playerG[j]) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            places[i] = playerG[j];
            *possible_places += 1;
        }
    }
    PlaceId *playerS = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numReturnedLocs);
    for (int j = 0; j < numReturnedLocs; j++) {
        exist = false;
        for (i = 0; places[i] != NOWHERE && i < NUM_REAL_PLACES; i++) {
            if (places[i] == playerS[j]) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            places[i] = playerS[j];
            *possible_places += 1;
        }
    }
    PlaceId *playerH = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &numReturnedLocs);
    for (int j = 0; j < numReturnedLocs; j++) {
        exist = false;
        for (i = 0; places[i] != NOWHERE && i < NUM_REAL_PLACES; i++) {
            if (places[i] == playerH[j]) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            places[i] = playerH[j];
            *possible_places += 1;
        }
    }
    PlaceId *playerM = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &numReturnedLocs);
    for (int j = 0; j < numReturnedLocs; j++) {
        exist = false;
        for (i = 0; places[i] != NOWHERE && i < NUM_REAL_PLACES; i++) {
            if (places[i] == playerM[j]) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            places[i] = playerM[j];
            *possible_places += 1;
        }
    }
    return places;
}





