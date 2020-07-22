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
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

/*typedef struct city *City;

typedef struct city {
	PlaceId cityID;
	int trap_num;
} city;*/

static void trapEncountered(GameView new, int placeID, int playerIndex);
void draculaEncounter(GameView new, int playerIndex);


struct gameView {
	// TODO: ADD FIELDS HERE

	int player_hp[5]; // player_hp[0] = LG's health point
					// player_hp[1] = DS's health point
					// player_hp[2] = VH's health point
					// player_hp[3] = MH's health point
					// player_hp[4] = Dracula's health point

	Round round; // which round it is
	int city_with_vam; // the city ID with unmatured vampire, if not, set value = -1
	//City cities_with_trap[TRAIL_SIZE];
	int *trapLocations; // int array of trap locations
	//array of cities with may include traps;if empty, set value = -1
	int score; // score of game
	int inhospital[4]; // dracula cannot be in hosptial.not in hosptial is 0, in hosptial = 1 (0 1 2 3)
	int **past_route; // store the past route for each player
	char *message[100]; // store messages for each round.
	int playerPlace[5]; // PlaceId of the current Player
	//Player currPlayer;
	int turn;
	int trail[366]; // The trail of Dracula
};



////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	
	
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	
	int i = 0;
	
	int length = strlen(pastPlays);
	char str[length];
	for (i = 0; pastPlays[i] != '\0'; i++) {
	    str[i] = pastPlays[i];
	}
	
	i = 0;
	while (i <= 3) {
	    new->player_hp[i] = 9;
	    i++;
	}
	new->player_hp[i] = 40;
	
	new->round = 0;
	new->city_with_vam = NOWHERE;
	
	new->trapLocations = malloc(6 * sizeof(int));
	for (i = 0; i < 6; i++) {
	    new->trapLocations[i] = NOWHERE;
	}
	
	
	new->score = 366;
	
	for (i = 0; i < 4; i++) {
	    new->inhospital[i] = 0;
	}

	new->past_route = malloc(5 *sizeof(int *));
    for (i = 0; i < 5; i++) {
        new->past_route[i] = malloc(366 * sizeof(int));
        for (int j = 0; j < 366; j++) {
            new->past_route[i][j] = NOWHERE;
        } 
    }	
    
    

     // not sure
    for (i = 0; i < new->round; i++) {
        new->message[i] = malloc(1024 * sizeof(char));
    }
	
	for(i = 0; i < 5; i++) new->playerPlace[i] = NOWHERE;
	
	for (i = 0; i < 366; i++) new->trail[i] = NOWHERE;

	new->turn = 0;
	char *step = strtok(str, " ");
	while (step != NULL) {

	    int is_DOUBLE_BACK_HIDE = 0;
	    for (i = 0; i < 4; i++) {
	        if (new->inhospital[i] == 1)
	            new->player_hp[i] = GAME_START_HUNTER_LIFE_POINTS;
	    }
	    new->turn++;    
		char place[3];
		for(int i = 1; i <= 2; i++) {
		    place[i - 1] = step[i];
	    }
	    
	    place[2] = '\0';
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
		    i = 0;
		    while (new->trail[i] != NOWHERE) {
		        i++;
		    }
		    new->trail[i] = placeID;
		    i = 0;
		    while (new->trail[i] != NOWHERE) {
		        i++;
		    } // i = trail size
		    if (placeID >= DOUBLE_BACK_1  && placeID <= DOUBLE_BACK_5) {
			    is_DOUBLE_BACK_HIDE = placeID;
		        int trackBack = placeID % 103;

	            placeID = new->trail[i - 2 - trackBack];
		        
		    } else if (placeID == HIDE) {
		        is_DOUBLE_BACK_HIDE = placeID;
		        placeID = new->trail[i - 2];

		    }
			new->playerPlace[PLAYER_DRACULA] = placeID;
		    new->trail[i - 1] = placeID;
			playerIndex = PLAYER_DRACULA;
			
		    if (placeID == CASTLE_DRACULA) new->player_hp[playerIndex] += 10;
		}
        i = 0;
        while (new->past_route[playerIndex][i] != NOWHERE) {
            i++;
        }
        if (is_DOUBLE_BACK_HIDE != 0) {
            
            new->past_route[playerIndex][i] = is_DOUBLE_BACK_HIDE;
        } else {
            new->past_route[playerIndex][i] = placeID;
        }


		
		// If hunters
		if (step[0] != 'D') {
		
			// Trap
			if (step[3] == 'T') {
				trapEncountered(new, placeID, playerIndex);
			} else if (step[3] == 'V') {
			    new->city_with_vam = NOWHERE;
			} else if (step[3] == 'D') {
			    draculaEncounter(new, playerIndex);
			}
			    
			
			// Vampire
			if (new->inhospital[playerIndex] == 0 && step[4] == 'T') {
			    trapEncountered(new, placeID, playerIndex);
			} else if (new->inhospital[playerIndex] == 0 && step[4] == 'V') {
				new->city_with_vam = NOWHERE;
			} else if (new->inhospital[playerIndex] == 0 && step[4] == 'D') {
			    draculaEncounter(new, playerIndex);
				// maybe reveal the Dracula's position??
			}
			
			if (new->inhospital[playerIndex] == 0 && step[5] == 'T') {
			    trapEncountered(new, placeID, playerIndex);
			} else if (new->inhospital[playerIndex] == 0 && step[5] == 'V') {
				new->city_with_vam = NOWHERE;
			} else if (new->inhospital[playerIndex] == 0 && step[5] == 'D') {
			    draculaEncounter(new, playerIndex);
				// maybe reveal the Dracula's position??
			}
			if (new->inhospital[playerIndex] == 0 && step[6] == 'D') {
			    draculaEncounter(new, playerIndex);
				// maybe reveal the Dracula's position??
			}

			
			
			
		} else { // If Dracula
		    
		    // If Dracula is at sea
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

	    step = strtok(NULL, " ");
	}
	new->round = new->turn / 5;
    
	return new;
}

void GvFree(GameView gv)
{
    int i;
    
    
    free(gv->past_route);
    
    
    for (i = 0; i < 5; i++) {
        free(gv->past_route[i]);
    }
    
    free(gv->trapLocations);
	
    
    
    for (i = 0; i < gv->round; i++) {
        free(gv->message[i]);
    }
    
    
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION	
		
	return gv->player_hp[player];
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	
	return gv->playerPlace[player];
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->city_with_vam;
}


PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int j = 0;
	for (int i = 0; i < 6;i++) {
	    if (gv->trapLocations[i] != NOWHERE) {
	        j++;
	    }
    }

	*numTraps = j;
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	*canFree = false;
	int i = 0;
	while (gv->past_route[player][i] != NOWHERE) {
	    i++;
	}
	*numReturnedMoves = i;
	return gv->past_route[player];
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int i = 0;
	PlaceId *array = malloc(numMoves * sizeof(PlaceId));
	while (gv->past_route[player][i] != NOWHERE) {
	    i++;
	}
	i--;
	int counter = 0;
	if (numMoves <= i + 1) {
	    while (counter < numMoves) {
	        array[counter] = gv->past_route[player][i];
	        i--;
	        counter++;
	    }
	    *numReturnedMoves = numMoves;
	    *canFree = true;
	    printf("lastMoves: %d\n", array[0]);
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	if (player == PLAYER_DRACULA) {
	    int i = 0;
	    while (gv->trail[i] != NOWHERE) {
	        i++;
	    } // array size
	    if (numLocs > i) {
	        numLocs = i;
	        *canFree = false;
	        return GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	    }
        PlaceId *array = malloc(sizeof(PlaceId) * i);
        int counter = 0;
        while (counter < numLocs) {
            array[counter] = gv->trail[i - 1 - counter];
            counter++;
        }
        *canFree = true;
        *numReturnedLocs = counter;
	    return array;
	        
	    
	} else {
	    int i = 0;
	    while (gv->past_route[player][i] != NOWHERE) {
	        i++;
	    } // array size
	    if (numLocs > i) {
	        numLocs = i;
	        *canFree = false;
	        return GvGetLocationHistory(gv, player, numReturnedLocs, canFree);
	    }
        PlaceId *array = malloc(sizeof(PlaceId) * i);
        int counter = 0;
        while (counter < numLocs) {
            array[counter] = gv->past_route[player][i - 1 - counter];
            counter++;
        }
        // not sure
        *canFree = false;
        *numReturnedLocs = counter;
	    return array;
	}
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	/*Map new = MapNew();
	ConnList head = MapGetConnections(m, currPlace);
	
	
	if (player == PLAYER_DRACULA) {
	    
	}*/
	
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
static void trapEncountered(GameView new, int placeID, int playerIndex) 
{
    for (int i = 0; i < TRAIL_SIZE; i++) {
		if(placeID == new->trapLocations[i] && new->trapLocations[i] != NOWHERE) {
		    
			new->player_hp[playerIndex] -= 2;
			if (new->player_hp[playerIndex] < 0) new->player_hp[playerIndex] = 0;
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

void draculaEncounter(GameView new, int playerIndex)
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


