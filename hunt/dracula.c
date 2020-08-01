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
static PlaceId *draculaTrails(DraculaView dv);
static bool trail_revealed(PlaceId *trails, DraculaView dv);


void decideDraculaMove(DraculaView dv)
{
    Round current_round = DvGetRound(dv);
    PlaceId current_place = DvGetPlayerLocation(dv, PLAYER_DRACULA);
    // get all the dangerousPlaces that hunters can possibly go to
    int possible_places = 0;
    PlaceId *dangerousPlaces = hunterPossiblePlaces(dv, &possible_places);
    bool dangerous;
    bool safe_castle = true;
    
    for (int i = 0; i < possible_places; i++) {
        if (dangerousPlaces[i] == CASTLE_DRACULA) {
            safe_castle = false;
            break;
        }
            
    }
    
    if (current_round == 0) {
        // initialize
        PlaceId initial_places[NUM_REAL_PLACES];
        for (int i = 0; i < NUM_REAL_PLACES; i++) {
            initial_places[i] = NOWHERE;
        }
        // add all safe places to initial_places
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
                // if castle is safe, go to castle directly
                if (i == CASTLE_DRACULA) {
                    registerBestPlay(placeIdToAbbrev(i), "Wryyyyyyyyyyyyy!");
                    free(dangerousPlaces);
                    return;
                }
            }
        }
        // generate a random place from initial_places
        srandom(time(NULL));
        registerBestPlay(placeIdToAbbrev(initial_places[random() % numMoves]), 
            "Wryyyyyyyyyyyyy!");
        free(dangerousPlaces);
        return;
    } else {
        
        
        // get all possible moves of dracula
        int numReturnedMoves;
        int numReturnedLocs;
        int numSeaLocs;
        PlaceId *moves = DvGetValidMoves(dv, &numReturnedMoves);
        PlaceId *locations = DvWhereCanIGo(dv, &numReturnedLocs);
        PlaceId *locations_sea = DvWhereCanIGoByType(dv, false, true,
                             &numSeaLocs);      
        PlaceId *trails = draculaTrails(dv);
        
        bool revealed = trail_revealed(trails, dv);

        
        if (current_place == CASTLE_DRACULA) {
            for (int i = 0; i < numReturnedMoves; i++) {
                if (moves[i] == HIDE && safe_castle) {
                    registerBestPlay("HI", "Mwahahahaha");
                    return;
                }
            }  
            registerBestPlay("GA", "Mwahahahaha");
            return;
        }
        if (current_place == GALATZ) {
            registerBestPlay("CN", "Mwahahahaha");
            return;
        }
/*        if (current_place == CONSTANTA) {*/
/*            registerBestPlay("BS", "Mwahahahaha");*/
/*            return;*/
/*        }*/
/*        if (current_place == BLACK_SEA) {*/
/*            registerBestPlay("IO", "Mwahahahaha");*/
/*            return;*/
/*        }*/
/*        if (current_place == IONIAN_SEA) {*/
/*            registerBestPlay("TS", "Mwahahahaha");*/
/*            return;*/
/*        }*/
/*        if (current_place == TYRRHENIAN_SEA) {*/
/*            registerBestPlay("GO", "Mwahahahaha");*/
/*            return;*/
/*        }*/
        
        // if blood points are low or no possible moves
        if (DvGetHealth(dv, PLAYER_DRACULA) <= 10 || numReturnedMoves == 0){
            registerBestPlay("TP", "Bye bye");
            return;
        }
        
        
        if (numReturnedLocs == 0) {
            if (!revealed) {
                srandom(time(NULL));
                registerBestPlay(placeIdToAbbrev(moves[random() % numReturnedMoves]), 
                    "Mwahahahaha");
                return;
            } else {
                registerBestPlay("TP", "Bye bye");
                return;
            }
        }
        
        // remove dangerous places from possible moves
        int i = 0;
        while (i < numReturnedLocs) {
            dangerous = false;
            for (int j = 0; j < possible_places; j++) {
                if (locations[i] == dangerousPlaces[j] &&
                    placeIdToType(locations[i]) != SEA) {
                    dangerous = true;
                    break;
                }
            }
            if (dangerous) {
                locations[i] = locations[--numReturnedLocs];
            } else {
                i++;
            }
        }
        
        i = 0;
        while (i < numReturnedMoves) {
            dangerous = false;
            for (int j = 0; j < possible_places; j++) {
                if (moves[i] == dangerousPlaces[j] &&
                    placeIdToType(moves[i]) != SEA) {
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
        
        // no possible moves
        if (numReturnedMoves == 0) {
            registerBestPlay("TP", "Bye bye");
            free(moves);
            return;
        }
        
        
        if (numReturnedLocs == 0) {       
            if (!revealed) {
                srandom(time(NULL));
                registerBestPlay(placeIdToAbbrev(moves[random() % numReturnedMoves]), 
                    "Mwahahahaha");
                return;
            } else {
                registerBestPlay("TP", "Bye bye");
                return;
            }
        }
        
        
        if (revealed && locations_sea != NULL) {
            registerBestPlay(placeIdToAbbrev(locations_sea[0]), 
            "Mwahahahaha");
            return;
        }


        // generate a random place from possible moves
        srandom(time(NULL));
        registerBestPlay(placeIdToAbbrev(locations[random() % numReturnedLocs]), 
            "Mwahahahaha");
        free(moves);
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
    // store current places of hunters
    PlaceId currS = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
    if (currS != currG) {
        places[1] = currS;
        *possible_places += 1;
    }
    // store current places of hunters
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
    // store current places of hunters
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
    PlaceId *playerG = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, 
        &numReturnedLocs);
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
    // store places that hunters can go to
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
    // store places that hunters can go to
    PlaceId *playerH = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, 
        &numReturnedLocs);
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
    // store places that hunters can go to
    PlaceId *playerM = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, 
        &numReturnedLocs);
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



static PlaceId *draculaTrails(DraculaView dv) {
    PlaceId *trails = malloc(6 * sizeof(PlaceId));
    for (int i = 0; i < 6; i++) {
        trails[i] = NOWHERE;
    }
    if (DvGetVampireLocation(dv) != NOWHERE) {
        trails[0] = DvGetVampireLocation(dv);
    }
    int numTraps;
    PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
    for (int i = 0; i < numTraps; i++) {
        bool exist = false;
        int j;
        for (j = 0; trails[j] != NOWHERE && j < 6; j++) {
            if (traps[i] == trails[j]) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            trails[j] = traps[i];
        }
    }
    free(traps);
    return trails;
}



static bool trail_revealed(PlaceId *trails, DraculaView dv) {
    PlaceId currG = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
    
    PlaceId currS = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
    
    PlaceId currH = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
    
    PlaceId currM = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);
    
    for (int i = 0; i < 6 && trails[i] != NOWHERE; i++) {
        if (currG == trails[i]) return true;
        if (currS == trails[i]) return true;
        if (currH == trails[i]) return true;
        if (currM == trails[i]) return true;
    }
    return false;
}




