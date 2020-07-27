////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
#include "testUtils.h"

#define NUM_HUNTERS 4

typedef int Item;
typedef struct QueueRep *Queue;

static void trapEncountered(GameView new, int placeID, int playerIndex);
static void draculaEncounter(GameView new, int playerIndex);
static ConnList createNode(Item item);
Queue createQueue(void);
void dropQueue(Queue q);
void enterQueue(Queue q, Item it);
Item leaveQueue(Queue q);


struct QueueRep {
	ConnList head;
	ConnList tail;
};

struct gameView {

	int player_hp[5];    // player_hp[0] = LG's health point
					     // player_hp[1] = DS's health point
					     // player_hp[2] = VH's health point
					     // player_hp[3] = MH's health point
					     // player_hp[4] = Dracula's health point

	Round round;         // Which round it is
	int city_with_vam;   // the city ID with unmatured vampire, if not, set value = -1
	int *trapLocations;  // int array of trap locations
	                     //array of cities with may include traps;if empty, set value = -1
	int score;           // score of game
	int inhospital[4];   // dracula cannot be in hosptial.not in hosptial is 0, in hosptial = 1 (0 1 2 3)
	int **past_route;    // store the past route for each player
	char **message;      // store messages for each turn.
	int playerPlace[5];  // PlaceId of the current Player
	int turn;            // The number of turns
	int trail[366];      // The trail of Dracula
};



////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
    // Initialise the gameview struct
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	
	// Covert the pointer pastPlays to an char array
	int i = 0;
	int length = strlen(pastPlays);
	char str[length];
	for (i = 0; pastPlays[i] != '\0'; i++) {
	    str[i] = pastPlays[i];
	}
	
	// Initialise each player's life points
	i = 0;
	while (i < NUM_HUNTERS) {
	    new->player_hp[i] = GAME_START_HUNTER_LIFE_POINTS;
	    i++;
	}
	new->player_hp[i] = GAME_START_BLOOD_POINTS;
	
	// Initialise round number and city_with_vam
	new->round = 0;
	new->city_with_vam = NOWHERE;
	
	// Initialise trapLocations to NOWHERE
	new->trapLocations = malloc(6 * sizeof(int));
	for (i = 0; i < 6; i++) {
	    new->trapLocations[i] = NOWHERE;
	}
	
	// Initialise game score
	new->score = GAME_START_SCORE;
	
	// All hunters are not in the hospital at the start of the game
	for (i = 0; i < NUM_HUNTERS; i++) {
	    new->inhospital[i] = 0;
	}

    // Initialise the past route of all players to NOWHERE
	new->past_route = malloc(5 *sizeof(int *));
    for (i = 0; i < 5; i++) {
        new->past_route[i] = malloc(366 * sizeof(int));
        for (int j = 0; j < 366; j++) {
            new->past_route[i][j] = NOWHERE;
        } 
    }
    
    // Copy the messages to gv->messages
    int totalMsg = 0;
    new->message = malloc(sizeof(char *) * 366);
    for (i = 0; i < 366; i++) {
        new->message[i] = NULL;
    }
    
    // While the first character of the message is printable
    while (messages[totalMsg][0] >= '!' && messages[totalMsg][0] <= '~') {
        i = 0;
        while (messages[totalMsg][i] != '\0') {
            i++;
        }
        
        if (i < 100) {
            new->message[totalMsg] = strdup(messages[totalMsg]);
            totalMsg++;
        }
    }
    
	
	
	// Initialise every place each player passed
	for(i = 0; i < 5; i++) new->playerPlace[i] = NOWHERE;
	
	for (i = 0; i < 366; i++) new->trail[i] = NOWHERE;

	new->turn = 0;
	
	// Extract each turn out from pastPlays
	char *step = strtok(str, " ");
	while (step != NULL) {

	    int is_DOUBLE_BACK_HIDE = 0;
	    
	    // If the hunter is in hospital, regain their full life points
	    for (i = 0; i < 4; i++) {
	        if (new->inhospital[i] == 1) {
	            new->player_hp[i] = GAME_START_HUNTER_LIFE_POINTS;
	            new->inhospital[i] = 0;
            }
	    }
	    
	    new->turn++;  
	    
	    // Extract the placeId from each turn  
		char place[3];
		for(i = 1; i <= 2; i++) {
		    place[i - 1] = step[i];
	    }
	    
	    place[2] = '\0';
	    
	    // Store the placeId
		int placeID = -1;
		if (place[0] == 'S' && place[1] == '?') {
		    placeID = SEA_UNKNOWN;
		} else if (place[0] == 'C' && place[1] == '?') {
		    placeID = CITY_UNKNOWN;
		} else {
		    placeID = placeAbbrevToId(place);
		}

		// Store the curr position of each player
		int playerIndex = 0;
		if (step[0] == 'G') {
			new->playerPlace[PLAYER_LORD_GODALMING] = placeID;
			playerIndex = PLAYER_LORD_GODALMING;
		} else if (step[0] == 'S') {
			new->playerPlace[PLAYER_DR_SEWARD] = placeID;
			playerIndex = PLAYER_DR_SEWARD;
		} else if (step[0] == 'H') {
			new->playerPlace[PLAYER_VAN_HELSING] = placeID;
			playerIndex = PLAYER_VAN_HELSING;
		} else if (step[0] == 'M') {
			new->playerPlace[PLAYER_MINA_HARKER] = placeID;
			playerIndex = PLAYER_MINA_HARKER;
		} else if (step[0] == 'D') {
		
		    // Add current place to dracula's trail
		    i = 0;
		    while (new->trail[i] != NOWHERE) {
		        i++;
		    }
		    new->trail[i] = placeID;
		    
		    // Store the real placeId if dracula ever used DB or HIDE
		    i = 0;
		    while (new->trail[i] != NOWHERE) {
		        i++;
		    }
		    if (placeID >= DOUBLE_BACK_1  && placeID <= DOUBLE_BACK_5) {
			    is_DOUBLE_BACK_HIDE = placeID;
		        int trackBack = placeID % DOUBLE_BACK_1;

	            placeID = new->trail[i - 2 - trackBack];
		        
		    } else if (placeID == HIDE) {
		        is_DOUBLE_BACK_HIDE = placeID;
		        placeID = new->trail[i - 2];

		    }
			new->playerPlace[PLAYER_DRACULA] = placeID;
		    new->trail[i - 1] = placeID;
			playerIndex = PLAYER_DRACULA;
			
			// Increase dracula's life point by 10
		    if (placeID == CASTLE_DRACULA) {
		        new->player_hp[playerIndex] += 10;
	        }
		}
		
		
		// Store DB or HIDE info in array past_route
        i = 0;
        while (new->past_route[playerIndex][i] != NOWHERE) {
            i++;
        }
        if (is_DOUBLE_BACK_HIDE != 0) {
            new->past_route[playerIndex][i] = is_DOUBLE_BACK_HIDE;
        } else {
            new->past_route[playerIndex][i] = placeID;
        }


		
		// If the current player is a hunter
		if (step[0] != 'D') {
		    
		    // If a hunter decides to rest, he can gain three life points
		    //printf("PlayerIndex: %d, Placeid: %d, lastRound: %d\n", playerIndex, placeID, new->past_route[playerIndex][new->round]);
		    if (new->round >= 1 
		        && placeID == new->past_route[playerIndex][new->round]) {
		        new->player_hp[playerIndex] += 3;
		        if (new->player_hp[playerIndex] > 9) {
		            new->player_hp[playerIndex] = 9;
		        }
		    }   
		
			// Encounter surprises
			if (step[3] == 'T') {
				trapEncountered(new, placeID, playerIndex);
			} else if (step[3] == 'V') {
			    new->city_with_vam = NOWHERE;
			} else if (step[3] == 'D') {
			    draculaEncounter(new, playerIndex);
			}


			if (new->inhospital[playerIndex] == 0 && step[4] == 'T') {
			    trapEncountered(new, placeID, playerIndex);
			} else if (new->inhospital[playerIndex] == 0 && step[4] == 'V') {
				new->city_with_vam = NOWHERE;
			} else if (new->inhospital[playerIndex] == 0 && step[4] == 'D') {
			    draculaEncounter(new, playerIndex);
			}
			
			
			if (new->inhospital[playerIndex] == 0 && step[5] == 'T') {
			    trapEncountered(new, placeID, playerIndex);
			} else if (new->inhospital[playerIndex] == 0 && step[5] == 'V') {
				new->city_with_vam = NOWHERE;
			} else if (new->inhospital[playerIndex] == 0 && step[5] == 'D') {
			    draculaEncounter(new, playerIndex);
			}
			
			if (new->inhospital[playerIndex] == 0 && step[6] == 'D') {
			    draculaEncounter(new, playerIndex);
			}
			
			
		} else {
		    // If the current player is dracula
		    
		    // If dracula is at sea
		    if (step[2] == '?') {
		        if (step[1] == 'S') {
		            new->player_hp[playerIndex] -= LIFE_LOSS_SEA;
		        }
		    } else if (placeIdToType(placeID) == SEA) {
		        new->player_hp[playerIndex] -= LIFE_LOSS_SEA;
		    }
		    
			// If a trap left the trail
			int index = 1;
			while (index < 6) {
				new->trapLocations[index - 1] = new->trapLocations[index];
				index++;
			}
			new->trapLocations[index - 1] = NOWHERE;

			// If place a trap
			if (step[3] == 'T') {
				new->trapLocations[5] = placeID;
			}

			// If place a vampire
			if (step[4] == 'V') {
				new->city_with_vam = placeID;
			} 

			// If a vampire matures
			if (step[5] == 'V') {
				new->city_with_vam = NOWHERE;
				new->score -= 13;
			}

			new->score--;
		}

        // Extract next turn
	    step = strtok(NULL, " ");
	    new->round = new->turn / 5;
	}
    
	return new;
}


// Free everything malloced in GvNew
void GvFree(GameView gv)
{
    int i;
    
    for (i = 0; i < 5; i++) {
        free(gv->past_route[i]);
    }
    free(gv->past_route);
    
    free(gv->trapLocations);
    
    for (i = 0; gv->message[i] != NULL && i < gv->turn; i++) {
        free(gv->message[i]);
    }
    
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	int value = gv->turn % 5;
	if (value == 0) {
	    return PLAYER_LORD_GODALMING;
	} else if (value == 1) {
	    return PLAYER_DR_SEWARD;
	} else if (value == 2) {
	    return PLAYER_VAN_HELSING;
	} else if (value == 3) {
	    return PLAYER_MINA_HARKER;
	} else {
	    return PLAYER_DRACULA;
	}	
}

int GvGetScore(GameView gv)
{
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	return gv->player_hp[player];
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	return gv->playerPlace[player];
}

PlaceId GvGetVampireLocation(GameView gv)
{
	return gv->city_with_vam;
}


PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
    // Get the number of traps in the rail
	int j = 0;
	for (int i = 0; i < 6;i++) {
	    if (gv->trapLocations[i] != NOWHERE) {
	        j++;
	    }
    }
    
	*numTraps = j;
	
	// Put the placeId where there is a trap into an array
	// The array traps should be freed manually
	PlaceId *traps = malloc(6 * sizeof(int));
	j = 0;
	for (int i = 0; i < 6;i++) {
	    if (gv->trapLocations[i] != NOWHERE) {
	        traps[j] = gv->trapLocations[i];
	        j++;
	    }
	}
	return traps;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	*canFree = false;
	int i = 0;
	while (gv->past_route[player][i] != NOWHERE) {
	    //printf("Round: %d, player: %d, place: %d\n", i, player, gv->past_route[player][i]);
	    i++;
	}
	*numReturnedMoves = i;
	return gv->past_route[player];
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	int i = 0;
	PlaceId *array = malloc(numMoves * sizeof(PlaceId));
	while (gv->past_route[player][i] != NOWHERE) {
	    i++;
	}
	i--;
	int counter = 0;
	
	// If the required number of moves
	// is less than the number of moves the player has made so far.
	if (numMoves <= i + 1) {
	
	    while (counter < numMoves) {
	        array[counter] = gv->past_route[player][i];
	        i--;
	        counter++;
	    }
	    *numReturnedMoves = numMoves;
	    *canFree = true;
	    
	} else {
	
	    *numReturnedMoves = i;
	    *canFree = false;
	    
	    array = GvGetMoveHistory(gv, player,numReturnedMoves, canFree);
	    return array;
    } 
	
	return array;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	if (player == PLAYER_DRACULA) {
	    *canFree = false; 
	    int i = 0; 
	    while (gv->trail[i] != NOWHERE) {
	        i++;
        }
	    *numReturnedLocs = i;
	    return gv->trail;
	} else {
	    return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	}
	
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	if (player == PLAYER_DRACULA) {
	
	    int i = 0;
	    while (gv->trail[i] != NOWHERE) {
	        i++;
	    }
	    
	    // If the required number of locations
	    // is greater than the number of locations the player has made so far.
	    if (numLocs > i) {
	        numLocs = i;
	        *canFree = false;
	        return GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	    }
	    
	    // Store the placeId into a new array
        PlaceId *draculaLastLocs = malloc(sizeof(PlaceId) * i);
        int counter = 0;
        while (counter < numLocs) {
            draculaLastLocs[counter] = gv->trail[i - 1 - counter];
            counter++;
        }
        *canFree = true;
        *numReturnedLocs = counter;
	    return draculaLastLocs;
	    
	} else {
	
	    int i = 0;
	    while (gv->past_route[player][i] != NOWHERE) {
	        i++;
	    }
	    
	    // If the required number of locations
	    // is greater than the number of locations the player has made so far.
	    if (numLocs > i) {
	        numLocs = i;
	        *canFree = false;
	        return GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	    }
	    
	    // Store the placeId into a new array
        PlaceId *hunterLastLocs = malloc(sizeof(PlaceId) * i);
        int counter = 0;
        while (counter < numLocs) {
            hunterLastLocs[counter] = gv->past_route[player][i - 1 - counter];
            counter++;
        }
        // not sure
        *canFree = false;
        *numReturnedLocs = counter;
	    return hunterLastLocs;
	}
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{

    Map new = MapNew();
	ConnList head = MapGetConnections(new, from);
	int railLength = (round + player) % 4;
	
	// Allocate enough space
	PlaceId *reachable = malloc(sizeof(PlaceId) * 1024);
	int i;
	int counter = 0;
	
	// Initialise reachable to NOWHERE
	for (i = 0; i < 1024; i++) reachable[i] = NOWHERE;
	
    // Reachable by rail
	if (player != PLAYER_DRACULA) {
	
        Queue place = createQueue();
        Queue index = createQueue();
        enterQueue(place, from);
        enterQueue(index, railLength);
        
	    while (head != NULL) {
            while (place->head != NULL) {
                PlaceId fromplace = leaveQueue(place);
                PlaceId railcheck = leaveQueue(index);
                
                // Check duplicate locations
                int dup = 0;
                i = 0;
                while (reachable[i] != NOWHERE) {
                    if (reachable[i] == fromplace) {
                        dup = 1;
                        break;
                    }
                    i++;
                }
                
                if (railcheck < 0 || dup == 1) {
                    continue;
                }
                reachable[counter] = fromplace;
                counter++;
                
                ConnList curr = MapGetConnections(new, fromplace);
                while (curr != NULL) {
                    if (curr->type == RAIL) {
                        enterQueue(place, curr->p);
                        enterQueue(index, railcheck - 1);
                    }
                    curr = curr->next;
                }
            }
            head = head->next;
        }
        dropQueue(place);
        dropQueue(index);
    }
    
    ConnList curr = MapGetConnections(new, from);
    while (curr != NULL) {
        if (curr->type == ROAD) {
            
            // Check duplicate locations
            int dup = 0;
            i = 0;
            while (reachable[i] != NOWHERE) {
                if (reachable[i] == curr->p) {
                    dup = 1;
                    break;
                }
                i++;
            }

            if (player == PLAYER_DRACULA && curr->p == ST_JOSEPH_AND_ST_MARY) {
                curr = curr->next;
                continue;
            }
            
            if (dup == 0) {
                reachable[counter] = curr->p;
                counter++;
            }

        }
        curr = curr->next;
    }
        
   
    curr = MapGetConnections(new, from);
    while (curr != NULL) {
        if (curr->type == BOAT) {
            reachable[counter] = curr->p;
            counter++;
        }
        curr = curr->next;
    }
    
    
	i = 0;
	int currPlaceIncl = 0;
	while (i < counter) {
	    if (reachable[i] == from) {
	        currPlaceIncl = 1;
	    }
	    i++;
	}
	if (currPlaceIncl == 0) {
	    reachable[counter] = from;
	    counter++;    
    }
	*numReturnedLocs = counter;
	return reachable;
	
	return 0;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{

	Map new = MapNew();
	ConnList head = MapGetConnections(new, from);
	int railLength = (round + player) % 4;
	
	PlaceId *reachable = malloc(sizeof(PlaceId) * 1024);
	int i;
	int counter = 0;
	
	for (i = 0; i < 1024; i++) reachable[i] = NOWHERE;
	
	

	if (player != PLAYER_DRACULA && rail == true) {
        Queue place = createQueue();
        Queue index = createQueue();
        enterQueue(place, from);
        enterQueue(index, railLength);
	    while (head != NULL) {
            while (place->head != NULL) {
                PlaceId fromplace = leaveQueue(place);
                PlaceId railcheck = leaveQueue(index);
                
                int dup = 0;
                i = 0;
                while (reachable[i] != NOWHERE) {
                    if (reachable[i] == fromplace) {
                        dup = 1;
                        break;
                    }
                    i++;
                }
                
                if (railcheck < 0 || dup == 1) {
                    continue;
                }
                reachable[counter] = fromplace;
                counter++;
                
                for (ConnList curr = MapGetConnections(new, fromplace); curr != NULL; curr = curr->next) {
                    if (curr->type == RAIL) {
                        enterQueue(place, curr->p);
                        enterQueue(index, railcheck - 1);
                    }
                }
            }
            head = head->next;
        }
        dropQueue(place);
        dropQueue(index);
    }
    
    if (road == true) {
        ConnList curr = head;
        while (curr != NULL) {
            if (curr->type == ROAD) {
                int dup = 0;
                i = 0;
                while (reachable[i] != NOWHERE) {
                    if (reachable[i] == curr->p) {
                        dup = 1;
                        break;
                    }
                    i++;
                }
  
                if (player == PLAYER_DRACULA && curr->p == ST_JOSEPH_AND_ST_MARY) {
                    curr = curr->next;
                    continue;
                }
                if (dup == 0) {
                
                    reachable[counter] = curr->p;
                    counter++;
                }

            }
            curr = curr->next;
        }
    }
        
    if (boat == true) {
        ConnList curr = head;
        while (curr != NULL) {
            if (curr->type == BOAT) {
                reachable[counter] = curr->p;
                counter++;
            }
            curr = curr->next;
        }
    }
    
    
    
	i = 0;
	int currPlaceIncl = 0;
	while (i < counter) {
	    if (reachable[i] == from) {
	        currPlaceIncl = 1;
	    }
	    i++;
	}
	if (currPlaceIncl == 0) {
	    reachable[counter] = from;
	    counter++;    
    }
	*numReturnedLocs = counter;
	return reachable;
}

////////////////////////////////////////////////////////////////////////
////////////////////  Helper Function  /////////////////////////////////
////////////////////////////////////////////////////////////////////////

// If a hunter encountered a trap, deduct the corresponding life points
static void trapEncountered(GameView new, int placeID, int playerIndex) 
{
    for (int i = 0; i < TRAIL_SIZE; i++) {
		if(placeID == new->trapLocations[i] && new->trapLocations[i] != NOWHERE) {
		    
			new->player_hp[playerIndex] -= 2;
			if (new->player_hp[playerIndex] < 0) {
			    new->player_hp[playerIndex] = 0;
		    }
			new->trapLocations[i] = NOWHERE;
			
			// Send to the hospital
			if (new->player_hp[playerIndex] <= 0) {
				new->playerPlace[playerIndex] = ST_JOSEPH_AND_ST_MARY;
				new->inhospital[playerIndex] = 1;
				new->score -= 6;
				break;
			}
			
		}
	}
	
}


// If a hunter encountered dracula, deduct their life points correspondingly
static void draculaEncounter(GameView new, int playerIndex)
{
    new->player_hp[playerIndex] -= 4;
    if (new->player_hp[playerIndex] < 0) new->player_hp[playerIndex] = 0;
	new->player_hp[PLAYER_DRACULA] -= 10;
	if (new->player_hp[playerIndex] <= 0) {
		new->playerPlace[playerIndex] = ST_JOSEPH_AND_ST_MARY;
		new->inhospital[playerIndex] = 1;
		new->score -= 6;
	}
}


// Queue.c ... list implementation of a queue

// private function for creating list nodes
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



