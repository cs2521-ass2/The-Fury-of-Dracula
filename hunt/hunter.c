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
#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"

static int ShortestToVam(HunterView hv, PlaceId VamLoc);


void decideHunterMove(HunterView hv)
{
    // TODO: Replace this with something better!
    int currPlayer = HvGetPlayer(hv);
    int round = HvGetRound(hv);
    
    if (round == 0) {
        int randomPlace = random() % MAX_REAL_PLACE;
        char *ram = placeIdToAbbrev(randomPlace);
        return registerBestPlay(ram, "Random place right now.");
    }
    
    
    
    int draLastRound = -1;
    int currPlace = HvGetPlayerLocation(hv, currPlayer);
    
    int numReturnedLocs;
    PlaceId *reachable = HvWhereCanIGo(hv, &numReturnedLocs);
    int randomIndex = random() % numReturnedLocs;
    
    char *currPlaceStr = placeIdToAbbrev(currPlace);
    
    PlaceId LastKnown = HvGetLastKnownDraculaLocation(hv, &draLastRound);
    
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
            int i = 0;
            while(shortestVam[i] != NOWHERE) {
                i++;
            }
            return registerBestPlay(placeIdToAbbrev(shortestVam[i - 1]), "Chasing for vampire now.");
        }
    }
    
    
    // Chasing for Dracula
    if (LastKnown == NOWHERE) {
        if (round > 6) {
            return registerBestPlay(currPlaceStr, "Rest.");
        } else {
            return registerBestPlay(placeIdToAbbrev(reachable[randomIndex]), "Next random place.");
        }
        
    } else {
        int pathLength;
        if (round - draLastRound < 6) {
            // Shortest Path
            HvGetShortestPathTo(hv, currPlayer, LastKnown,
                             &pathLength);
            HvGetPlayerLocation(hv, currPlayer);
            
        } else if (round - draLastRound > 6 && round - draLastRound < 10) {
            return registerBestPlay(placeIdToAbbrev(reachable[randomIndex]), "Next random place.");
        } else {
            return registerBestPlay(currPlaceStr, "Rest.");
        }
        
    }
    
    
    
    
        
        
    
    
    
    
    
    //registerBestPlay("TO", "Have we nothing Toulouse?");
}


static int ShortestToVam(HunterView hv, PlaceId VamLoc) {
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
