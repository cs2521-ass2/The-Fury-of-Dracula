////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01   v1.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01   v1.1    Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31   v2.0    Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <time.h>
#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"

static int ShortestToVam(HunterView hv, PlaceId VamLoc);
static int otherHuntersNearby(HunterView hv, PlaceId currPlace, Player currPlayer);


void decideHunterMove(HunterView hv)
{

    int currPlayer = HvGetPlayer(hv);
    int round = HvGetRound(hv);
    
    // When the game starts, each hunter will randomly pick a place
    if (round == 0) {
        srandom(time(NULL));
        int randomPlace = random() % MAX_REAL_PLACE;
        char *ram = placeIdToAbbrev(randomPlace);
        return registerBestPlay(ram, "Random place right now.");
    }
    
    
    int draLastRound = -1;
    int currPlace = HvGetPlayerLocation(hv, currPlayer);
    
    int numReturnedLocs;
    PlaceId *reachable = HvWhereCanIGo(hv, &numReturnedLocs);
    srandom(time(NULL));
    int randomIndex = random() % numReturnedLocs;
    
    char *currPlaceStr = placeIdToAbbrev(currPlace);
    PlaceId LastKnown = HvGetLastKnownDraculaLocation(hv, &draLastRound);
    
    // Rest to gain life points
    if (HvGetHealth(hv, currPlayer) <= 3) {
        return registerBestPlay(currPlaceStr, "Rest.");
    }

    // Check vampire location
    PlaceId vamLoc = HvGetVampireLocation(hv);
    if (vamLoc == CITY_UNKNOWN) {
        return registerBestPlay(currPlaceStr, "Rest to search for vampire.");
    } else if (vamLoc != NOWHERE) {
        int pathLength;
        int player = ShortestToVam(hv, vamLoc);
        if (player == currPlayer) {
            PlaceId *shortestVam = HvGetShortestPathTo(hv, player, vamLoc,
                                 &pathLength);
            
            return registerBestPlay(placeIdToAbbrev(shortestVam[0]), "Chasing for vampire now.");
        }
    }
    
    
    // Chasing for Dracula
    if (LastKnown == NOWHERE) {
        if (round >= 6) {
            return registerBestPlay(currPlaceStr, "Rest.");
        } else {
            int i;
            srandom(time(NULL));
            int value = random() % numReturnedLocs;
            for (i = 0; i < numReturnedLocs; i++) {
                if (!otherHuntersNearby(hv, reachable[value], currPlayer)) {
                    break;
                }
                value++;
                value = value % numReturnedLocs;
            }

            if (i == numReturnedLocs) {
                return registerBestPlay(placeIdToAbbrev(reachable[randomIndex]), "Next random place.");
            } else {
                return registerBestPlay(placeIdToAbbrev(reachable[value]), "To somewhere don't have a hunter.");
            }
        }
        
    } else {
        int pathLength;
        if (round - draLastRound < 6) {
            // Shortest Path
            PlaceId *shortestToDracula = HvGetShortestPathTo(hv, currPlayer, LastKnown,
                             &pathLength);
            
            if (!otherHuntersNearby(hv, shortestToDracula[0], currPlayer)) {
                return registerBestPlay(placeIdToAbbrev(shortestToDracula[0]), 
                    "Chase for Dracula if no other hunters nearby.");
            } else {
                // Random Place
                int i;
                srandom(time(NULL));
                int value = random() % numReturnedLocs;
                for (i = 0; i < numReturnedLocs; i++) {
                    if (!otherHuntersNearby(hv, reachable[value], currPlayer)) {
                        break;
                    }
                    value++;
                    value = value % numReturnedLocs;
                }
                if (i == numReturnedLocs) {
                    return registerBestPlay(placeIdToAbbrev(reachable[randomIndex]), "Next random place.");
                } else {
                    return registerBestPlay(placeIdToAbbrev(reachable[value]), "To somewhere don't have a hunter.");
                }
            }
            
            
        } else if (round - draLastRound > 6 && round - draLastRound < 10) {
            // Random place where no other hunter's there
            srandom(time(NULL));
            int value = random() % numReturnedLocs;
            int i;
            for (i = 0; i < numReturnedLocs; i++) {
                if (!otherHuntersNearby(hv, reachable[value], currPlayer)) {
                    break;
                }
                value++;
                value = value % numReturnedLocs;
            }

            if (i == numReturnedLocs) {
                return registerBestPlay(placeIdToAbbrev(reachable[randomIndex]), "Next random place.");
            } else {
                return registerBestPlay(placeIdToAbbrev(reachable[value]), "To somewhere don't have a hunter.");
            }
            
        } else {
            return registerBestPlay(currPlaceStr, "Rest.");
        }
        
    }  
}


static int ShortestToVam(HunterView hv, PlaceId VamLoc) 
{
    int playerLG = PLAYER_LORD_GODALMING;
    int playerDS = PLAYER_DR_SEWARD;
    int playerMH = PLAYER_MINA_HARKER;
    int playerVH = PLAYER_VAN_HELSING;
    
    int shortestLength;
    int length;
    int playerIndex = PLAYER_LORD_GODALMING;
    
    HvGetShortestPathTo(hv, playerLG, VamLoc, &shortestLength);
    HvGetShortestPathTo(hv, playerDS, VamLoc, &length);
    if (length < shortestLength) {
        playerIndex = playerDS;
        shortestLength = length;
    }
    
    HvGetShortestPathTo(hv, playerMH, VamLoc, &length);
    
    if (length < shortestLength) {
        playerIndex = playerMH;
        shortestLength = length;
    }
    
    HvGetShortestPathTo(hv, playerVH, VamLoc, &length);
    
    if (length < shortestLength) {
        playerIndex = playerVH;
        shortestLength = length;
    }
    
    return playerIndex;

}



// Check if the surrounding places have other hunters
static int otherHuntersNearby(HunterView hv, PlaceId currPlace, Player currPlayer) 
{

    int hunterPlaces[3]; 

    int i = 0;
    int j = 0;
    while (i < 4) {
        if (i == currPlayer) {
            i++;
            continue;
        }
        hunterPlaces[j] = HvGetPlayerLocation(hv, i);
        j++;
        i++;
    }

    i = 0;
    while (i < 3) {
        if (currPlace == hunterPlaces[i]) return 1;
        i++;
    }
    return 0;
}
