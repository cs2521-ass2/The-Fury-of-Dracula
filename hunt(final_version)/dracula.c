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
//static PlaceId *draculaTrails(DraculaView dv);
//static bool trail_revealed(DraculaView dv);


// decide the next move of dracula
void decideDraculaMove(DraculaView dv)
{
    PlaceId port_city[29] = {CONSTANTA, VARNA, SALONICA, ATHENS, VALONA,
        BARI, NAPLES, ROME, VENICE, GENOA, CAGLIARI, HAMBURG, AMSTERDAM, 
        MARSEILLES, BARCELONA, ALICANTE, CADIZ, LISBON, SANTANDER, 
        BORDEAUX, NANTES, LE_HAVRE, PLYMOUTH, LONDON, SWANSEA, 
        LIVERPOOL, EDINBURGH, DUBLIN, GALWAY};
    Round current_round = DvGetRound(dv);
    PlaceId current_place = DvGetPlayerLocation(dv, PLAYER_DRACULA);
    // get all the dangerousPlaces that hunters can possibly go to
    int possible_places = 0;
    PlaceId *dangerousPlaces = hunterPossiblePlaces(dv, &possible_places);
    bool dangerous;
    bool safe_castle = true;
    // initialize
    for (int i = 0; i < possible_places; i++) {
        if (dangerousPlaces[i] == CASTLE_DRACULA) {
            safe_castle = false;
            break;
        }         
    }
    // the first round
    if (current_round == 0) {
        // if castle is safe, go to castle directly
        if (safe_castle) {
            registerBestPlay("CD", "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
            return;
        }
        // initialize
        PlaceId initial_places[29] = {0};
        // add all safe places to initial_places
        int numMoves = 0;
        for (int i = 0; i < 29; i++) {
            dangerous = false;
            for (int j = 0; j < possible_places; j++) {
                if (port_city[i] == dangerousPlaces[j]) {
                    dangerous = true;
                    break;
                }
            }
            if (!dangerous ) {
                initial_places[numMoves++] = port_city[i];       
            }
        }
        // generate a random place from initial_places
        srandom(time(NULL));
        registerBestPlay(placeIdToAbbrev(initial_places[random() % numMoves]), 
            "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
        free(dangerousPlaces);
        return;
    } else {
        
        
        // get all possible moves of dracula and trail
        int numReturnedMoves;
        int numReturnedLocs;
        PlaceId *moves = DvGetValidMoves(dv, &numReturnedMoves);
        PlaceId *locations = DvWhereCanIGo(dv, &numReturnedLocs);

        
        
        // if no possible moves
        if (numReturnedMoves == 0){
            registerBestPlay("TP", "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
            return;
        }
        
        // only hide or double back available
        if (numReturnedLocs == 0) {
            dangerous = false;
            for (int j = 0; j < possible_places; j++) {
                if (current_place == dangerousPlaces[j]) {
                    dangerous = true;
                    break;
                }
            }
            for (int j = 0; j < numReturnedMoves;) {
                if (moves[j] == DOUBLE_BACK_1 && dangerous) {
                    j++;
                    continue;
                } else if (moves[j] == HIDE && dangerous) {
                    j++;
                    continue;    
                }
                registerBestPlay(placeIdToAbbrev(moves[j]), 
                    "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                return;
            }
            srandom(time(NULL));
            registerBestPlay(placeIdToAbbrev(moves[random() % numReturnedMoves]), 
                "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
            return;           
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
        
        if (numReturnedLocs == 0) {
            // only hide or double back available  
            if (numReturnedMoves != 0 ) {
                dangerous = false;
                for (int j = 0; j < possible_places; j++) {
                    if (current_place == dangerousPlaces[j]) {
                        dangerous = true;
                        break;
                    }
                }
                for (int j = 0; j < numReturnedMoves;) {
                    if (moves[j] == DOUBLE_BACK_1 && dangerous) {
                        j++;
                        continue;
                    } else if (moves[j] == HIDE && dangerous) {
                        j++;
                        continue;    
                    }
                    registerBestPlay(placeIdToAbbrev(moves[j]), 
                        "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                    return;
                }
            }
         
            int numGo;  
            PlaceId *locations2 = DvWhereCanIGo(dv, &numGo);
            int numGreach;
            PlaceId *G_reach = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numGreach);
            int numSreach;
            PlaceId *S_reach = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numSreach);
            int numHreach;
            PlaceId *H_reach = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &numHreach);
            int numMreach;
            PlaceId *M_reach = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &numMreach);
            
            //intersections array to store the number of intersections of paths
            int intersections[numGo];
            for (int y = 0; y < numGo; y++) {
                intersections[y] = 0;
            } // array to store lowest hp of each intersection
            int lowestHP[numGo];
            for (int y = 0; y < numGo; y++) {
                lowestHP[y] = 9;
            }
                               
            int y = 0;
            while (y < numGo) {
                int x = 0; 
                while (x < numGreach) { 
                    if (locations2[y] == G_reach[x]) {
                        intersections[y] += 1;
                        lowestHP[y] = DvGetHealth(dv, PLAYER_LORD_GODALMING);                      
                    }
                    x++;
                }
                y++;
            }
            
            for (int i = 0; i < numGo; i++) {
                if (locations2[i] == DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING)) {
                    intersections[i] += 1;
                    int HP = DvGetHealth(dv, PLAYER_LORD_GODALMING);
                    // if there are more than 1 lowest-intersections, 
                    // take the one with lowest-hp hunter.
                    if (HP < lowestHP[i]) {
                        lowestHP[i] = HP; 
                    }
                }
                if (locations2[i] == DvGetPlayerLocation(dv, PLAYER_DR_SEWARD)) {
                    intersections[i] += 1;
                    int HP = DvGetHealth(dv, PLAYER_DR_SEWARD);
                    // if there are more than 1 lowest-intersections, 
                    // take the one with lowest-hp hunter.
                    if (HP < lowestHP[i]) {
                        lowestHP[i] = HP; 
                    }
                }
                if (locations2[i] == DvGetPlayerLocation(dv, PLAYER_VAN_HELSING)) {
                    intersections[i] += 1;
                    int HP = DvGetHealth(dv, PLAYER_VAN_HELSING);
                    // if there are more than 1 lowest-intersections, 
                    // take the one with lowest-hp hunter.
                    if (HP < lowestHP[i]) {
                        lowestHP[i] = HP; 
                    }
                }
                if (locations2[i] == DvGetPlayerLocation(dv, PLAYER_MINA_HARKER)) {
                    intersections[i] += 1;
                    int HP = DvGetHealth(dv, PLAYER_MINA_HARKER);
                    // if there are more than 1 lowest-intersections, 
                    // take the one with lowest-hp hunter.
                    if (HP < lowestHP[i]) {
                        lowestHP[i] = HP; 
                    }
                }
                    
            }
            
            y = 0;
            
            while (y < numGo) {
                int x = 0; 
                while (x < numSreach) {
                    if (locations2[y] == S_reach[x]) {
                        intersections[y] += 1;
                        int HP = DvGetHealth(dv, PLAYER_DR_SEWARD);
                        // if there are more than 1 lowest-intersections, 
                        // take the one with lowest-hp hunter.
                        if (HP < lowestHP[y]) {
                            lowestHP[y] = HP; 
                        }                     
                    }
                    x++;
                }
                y++;
            }        
            y = 0;
            
            while (y < numGo) {
                int x = 0; 
                while (x < numHreach) {
                    if (locations2[y] == H_reach[x]) {
                        intersections[y] += 1;
                        int HP = DvGetHealth(dv, PLAYER_VAN_HELSING);
                        if (HP < lowestHP[y]) {
                            lowestHP[y] = HP; 
                        }                     
                    }
                    x++;
                }
                y++;
            } 
            y = 0;
            
            while (y < numGo) {
                int x = 0; 
                while (x < numMreach) {
                    if (locations2[y] == M_reach[x]) {
                        intersections[y] += 1;
                        int HP = DvGetHealth(dv, PLAYER_MINA_HARKER);
                        if (HP < lowestHP[y]) {
                            lowestHP[y] = HP; 
                        }                     
                    }
                    x++;
                }
                y++;
            }  
            
            int ToGo = 0; 
            int z = 1;
            while (z < numGo) {
                if(intersections[z] > intersections[ToGo]) {
                    z++;
                } else if (intersections[z] < intersections[ToGo]) {
                    ToGo = z;
                    z++;
                } else { //if equal
                    if (lowestHP[z] < lowestHP[ToGo]) {
                        ToGo = z;
                    }
                    z++;
                }
            }
            registerBestPlay(placeIdToAbbrev(locations2[ToGo]), 
                "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
/*          free(locations2);
            free(G_reach);
            free(S_reach);
            free(H_reach);
            free(M_reach);  */
            return;
        }
        // if in castle, try to move to port city or hide to gain hp
        if (current_place == CASTLE_DRACULA) {
            
            bool galgatz = false;
            for (int i = 0; i < numReturnedMoves; i++) {
                if (moves[i] == HIDE && safe_castle) {
                    registerBestPlay("HI", 
                        "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                    return;
                }
                if (moves[i] == GALATZ) {
                    galgatz = true;
                }
            }  
            // generate a random place from possible moves
            if (galgatz) {
                registerBestPlay("GA", 
                    "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                return;
            }
            srandom(time(NULL));
            registerBestPlay(placeIdToAbbrev(locations[random() % numReturnedLocs]), 
                "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
            return;
        }
        // try to move to port city
        if (current_place == GALATZ) {
            for (int i = 0; i < numReturnedMoves; i++) {
                if (moves[i] == CONSTANTA) {
                    registerBestPlay("CN", 
                        "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                    return;
                }
            }
            srandom(time(NULL));
            registerBestPlay(placeIdToAbbrev(locations[random() % numReturnedLocs]), 
                "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
            return; 
            
        }
      

        // generate a random place from possible locations
        for (int i = 0; i < numReturnedLocs; i++) {
            if (locations[i] == CASTLE_DRACULA) {
                registerBestPlay("CD", "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                return;
            }
        }
        for (int i = 0; i < numReturnedLocs; i++) {
            for (int j = 0; j < 29; j++) {
                if(locations[i] == port_city[j]) {
                    registerBestPlay(placeIdToAbbrev(locations[i]), 
                        "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                    return;
                }
            }
        }
        
        for (int i = 0; i < numReturnedLocs; i++) {     
            if(placeIdToType(locations[i]) == LAND) {
                if (locations[i] != BERLIN && locations[i] != PRAGUE && 
                    locations[i] != VIENNA && locations[i] != BUDAPEST) {
                    registerBestPlay(placeIdToAbbrev(locations[i]), 
                        "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
                    return;
                }
                
            }
            
        }
        
        registerBestPlay(placeIdToAbbrev(locations[0]), 
            "MUDA MUDA MUDA MUDA MUDA!\nWRYYYYYYYY!");
        free(moves);
        return;
    }
}




// all the possible places of hunters
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




