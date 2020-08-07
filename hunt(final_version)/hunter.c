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
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"
#include "Map.h"
#include <assert.h>
#define maximunPlace 100


typedef int Item;
typedef struct QueueRep *Queue;
struct QueueRep {
	ConnList head;
	ConnList tail;
};

static ConnList createNode(Item item);
Queue createQueue(void);
void dropQueue(Queue q);
void enterQueue(Queue q, Item it);
Item leaveQueue(Queue q);
void search(HunterView hv, int radius, PlaceId currPlace, PlaceId *poss, int currPlayer);



static int ShortestToVam(HunterView hv, PlaceId VamLoc);
static int otherHuntersNearby(HunterView hv, PlaceId currPlace, Player currPlayer);

int lastHpLG = 9;
int lastHpDs = 9;
int lastHpHM = 9;
int lastHpVH = 9;


void decideHunterMove(HunterView hv)
{
    PlaceId *poss = malloc(maximunPlace * sizeof(PlaceId));
    for (int i = 0; i < maximunPlace; i++) {
        poss[i] = NOWHERE;
    }
    
    
    int currPlayer = HvGetPlayer(hv);
    
    int round = HvGetRound(hv);
    
    
    // When the game starts, each hunter will randomly pick a place
    if (round == 0) {
        if (currPlayer == PLAYER_LORD_GODALMING) {
            return registerBestPlay("CD", "I'm coming.....");
        }
        
        // Random Place
        int i;
        srandom(time(NULL));
        int randomPlace = random() % MAX_REAL_PLACE;
        for (i = 0; i < 4; i++) {
            if (!otherHuntersNearby(hv, randomPlace, currPlayer)) {
                break;
            }
            randomPlace++;
            randomPlace = randomPlace % MAX_REAL_PLACE;
        }
        
        return registerBestPlay(placeIdToAbbrev(randomPlace), "Random place and no other hunters here at round 0.");
        
    }
    
    
    int draLastRound = -1;
    int currPlace = HvGetPlayerLocation(hv, currPlayer);
    
    int numReturnedLocs;
    PlaceId *reachable = HvWhereCanIGo(hv, &numReturnedLocs);
    srandom(time(NULL));
    int randomIndex = random() % numReturnedLocs;
    
    char *currPlaceStr = placeIdToAbbrev(currPlace);
    PlaceId LastKnown = HvGetLastKnownDraculaLocation(hv, &draLastRound);
    int pathLength;
    
    
    
    int currHp = HvGetHealth(hv, currPlayer);
    if ((round - draLastRound > 3) && currPlayer == PLAYER_LORD_GODALMING && lastHpLG - currHp > 0) {
        LastKnown = currPlace;
        lastHpLG = currHp;
    }
    
    if ((round - draLastRound > 3) && currPlayer == PLAYER_DR_SEWARD && lastHpDs - currHp > 0) {
        LastKnown = currPlace;
        lastHpDs = currHp;
    }
    
    if ((round - draLastRound > 3) && currPlayer == PLAYER_VAN_HELSING && lastHpVH - currHp > 0) {
        LastKnown = currPlace;
        lastHpVH = currHp;
    }
    
    if ((round - draLastRound > 3) && currPlayer == PLAYER_MINA_HARKER && lastHpHM - currHp > 0) {
        LastKnown = currPlace;
        lastHpHM = currHp;
    }
    
   
    // Rest to gain life points
    if (HvGetHealth(hv, currPlayer) < 3) {
        return registerBestPlay(currPlaceStr, "Rest.");
    }
    
    // If Dracula is only one move away and it is revealed, go to that place
    PlaceId draculaLoc = HvGetPlayerLocation(hv, PLAYER_DRACULA);
    if (draculaLoc == SEA_UNKNOWN) {
        return registerBestPlay(currPlaceStr, "Rest.");
    }
    
    
    if (draculaLoc != CITY_UNKNOWN && draculaLoc != SEA_UNKNOWN && draculaLoc != NOWHERE) {
        HvGetShortestPathTo(hv, currPlayer, draculaLoc, &pathLength);
        if (pathLength <= 1) {
            return registerBestPlay(placeIdToAbbrev(draculaLoc), "Gotcha~");
        }
        LastKnown = draculaLoc;
    }
    
    

    // Assign PLAYER_LORD_GODALMING to CD
    if (currPlayer == PLAYER_LORD_GODALMING) {
        PlaceId *shortestToCD = HvGetShortestPathTo(hv, currPlayer, placeAbbrevToId("CD"), &pathLength);
        
        
        if (round >= 5 && LastKnown == NOWHERE) {
            return registerBestPlay(currPlaceStr, "Rest.");
        } else if (round - draLastRound > 10) {
            return registerBestPlay(currPlaceStr, "Rest.");
        }
        
        // If tp to hospital
        if (currPlace != placeAbbrevToId("KL") && currPlace != placeAbbrevToId("CD") && currPlace != placeAbbrevToId("GA")) {
            return registerBestPlay(placeIdToAbbrev(shortestToCD[0]), "Mwahahahahaha");
        }
        
        
        // If hp less than 9
        if (currHp < 9) {
            return registerBestPlay(currPlaceStr, "LG is resting because he's dying 9>_<9.");
        }
        
        
        if (currPlace == placeAbbrevToId("KL")) {
            return registerBestPlay("CD", "I'm watching u~");
        }
        
        if (currPlace == placeAbbrevToId("CD")) {
            return registerBestPlay("GA", "I'm watching u~");
        }
        
        if (currPlace == placeAbbrevToId("GA")) {
            return registerBestPlay("KL", "I'm watching u~");
        }
        
    }



    // Check vampire location
    PlaceId vamLoc = HvGetVampireLocation(hv);
    if (vamLoc == CITY_UNKNOWN && round - draLastRound >= 6) {
        
        return registerBestPlay(currPlaceStr, "Rest to search for dracula's trail and see if there is a vam.");
        
    } else if (vamLoc != NOWHERE && vamLoc != CITY_UNKNOWN) {
        int player = ShortestToVam(hv, vamLoc);
        if (player == currPlayer ) {
            PlaceId *shortestVam = HvGetShortestPathTo(hv, player, vamLoc,
                                 &pathLength);
            
            return registerBestPlay(placeIdToAbbrev(shortestVam[0]), "Chasing for vampire now.");
        }
    }
    
    
    // Chasing for Dracula
    if (LastKnown == NOWHERE) {
        if (round >= 5) {
            return registerBestPlay(currPlaceStr, "Rest.");
        } else {
            
            int i;
            srandom(time(NULL));
            int value = random() % numReturnedLocs;
            for (i = 0; i < numReturnedLocs; i++) {
                if (!otherHuntersNearby(hv, reachable[value], currPlayer)) {
                    if (reachable[value] != currPlace) break;
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
        if (round - draLastRound <= 10) {
            if (round - draLastRound <= 2 && round - draLastRound > 0) {
                search(hv, (round - draLastRound), LastKnown, poss, currPlayer); 
                
                int num = 0;
                while (poss[num] != NOWHERE) {
                    num++;
                }   
                srandom(time(NULL)); 
                int value = random() % num;
                LastKnown = poss[value];  
            }      
            // Shortest Path
            PlaceId *shortestToDracula = HvGetShortestPathTo(hv, currPlayer, LastKnown,
                             &pathLength);
            if (pathLength == 0) {
               
                // Random Place
                int i;
                srandom(time(NULL));
                int value = random() % numReturnedLocs;
                for (i = 0; i < numReturnedLocs; i++) {
                    if (!otherHuntersNearby(hv, reachable[value], currPlayer)) {
                        if (reachable[value] != currPlace) break;
                    }
                    value++;
                    value = value % numReturnedLocs;
                }
                if (i == numReturnedLocs) {
                    return registerBestPlay(placeIdToAbbrev(reachable[randomIndex]), "Next random place since I'm already here.");
                } else {
                    return registerBestPlay(placeIdToAbbrev(reachable[value]), "To somewhere don't have a hunter since I'm already here.");
                }
                
            }
            
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
                        if (reachable[value] != currPlace) break;
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
            return registerBestPlay(currPlaceStr, "Rest.");
        }
        
    }  
}


static int ShortestToVam(HunterView hv, PlaceId VamLoc) 
{
    //int playerLG = PLAYER_LORD_GODALMING;
    int playerDS = PLAYER_DR_SEWARD;
    int playerMH = PLAYER_MINA_HARKER;
    int playerVH = PLAYER_VAN_HELSING;
    
    int shortestLength;
    int length;
    int playerIndex = PLAYER_DR_SEWARD;
    
    //HvGetShortestPathTo(hv, playerLG, VamLoc, &shortestLength);
    HvGetShortestPathTo(hv, playerDS, VamLoc, &shortestLength);
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


void search(HunterView hv, int radius, PlaceId currPlace, PlaceId *poss, int currPlayer) {
    Map new = MapNew();
    ConnList head = MapGetConnections(new, currPlace);
    Queue place = createQueue();
    Queue index = createQueue();
    enterQueue(place, currPlace);
    enterQueue(index, radius);
    int counter = 0;
    int i;
    while (head != NULL) {
        while (place->head != NULL) {
            PlaceId fromplace = leaveQueue(place);
            PlaceId railcheck = leaveQueue(index);
            
            // Check duplicates
            int dup = 0;
            i = 0;
            while (poss[i] != NOWHERE) {
                if (poss[i] == fromplace) {
                    dup = 1;
                    break;
                }
                i++;
            }
            
            if (railcheck < 0 || dup == 1) {
                continue;
            }
            poss[counter] = fromplace;
            counter++;
            
            // Add surrounding places into the array
            for (ConnList curr = MapGetConnections(new, fromplace); curr != NULL; curr = curr->next) {
                if (curr->type != RAIL && ! otherHuntersNearby(hv, curr->p, currPlayer)) {
                    enterQueue(place, curr->p);
                    enterQueue(index, railcheck - 1);
                }
            }
        }
        head = head->next;
    }
    dropQueue(place);
    dropQueue(index);
    return;
}


static ConnList createNode(Item item)
{
	ConnList n = malloc(sizeof(ConnList));
	assert (n != NULL);
	n->p = item;
	n->next = NULL;
	return n;
}

// create an initially empty Queue
Queue createQueue(void)
{
	Queue q = malloc(sizeof(struct QueueRep));
	assert (q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free all memory used by the Queue
void dropQueue(Queue q)
{
	ConnList curr;
	ConnList next;
	assert(q != NULL);
	curr = q->head;
	while (curr != NULL) {
		next = curr->next;
		curr = next;
	}
	free (q);
}

// add new Item to the tail of the Queue
void enterQueue (Queue q, Item it)
{
	assert(q != NULL);
	ConnList n = createNode(it);
	if (q->head == NULL) {
		q->head = n;
	} else {
	    q->tail->next = n;
	}
	q->tail = n;
}

// remove Item from head of Queue; return it
Item leaveQueue (Queue q)
{
	assert(q != NULL);
	Item it = q->head->p;
	ConnList delNode = q->head;
	q->head = q->head->next;
	free(delNode);
	return it;
}


      


