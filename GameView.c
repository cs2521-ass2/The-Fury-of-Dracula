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

typedef struct city *City;

typedef struct city {
	PlaceId cityID;
	int trap_num;
} city;


struct gameView {
	// TODO: ADD FIELDS HERE

	int player_hp[5]; // player_hp[0] = LG's health point
					// player_hp[1] = DS's health point
					// player_hp[2] = VH's health point
					// player_hp[3] = MH's health point
					// player_hp[4] = Dracula's health point

	Round round; // which round it is
	int city_with_vam; // the city ID with unmatured vampire, if not, set value = -1
	City cities_with_trap[TRAIL_SIZE]; 
	//array of cities with may include traps;if empty, set value = -1
	int score; // score of game
	int inhospital[4]; // dracula cannot be in hosptial.not in hosptial is 0, in hosptial = 1 (0 1 2 3)
	int **past_route; // store the past route for each player
	char *message[100]; // store messages for each round.
	int playerPlace[5]; // PlaceId of the current Player
	//Player currPlayer;
	int turn;
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

	
	
	//char *p = "Hello this is Theresa";
	//printf("%s\n", p);
	int i = 0;
	
	int length = strlen(pastPlays);
	char str[length];
	for (i = 0; pastPlays[i] != '\0'; i++) {
	    str[i] = pastPlays[i];
	    //printf("%c", str[i]);
	}
	str[i - 1] = '\0';
	//printf("string is: %s\n", str);
	//char *out = strtok(str, " ");
	//printf("%s abcd\n", out);
	//printf("before assertion!\n");
	//strcpy(str, pastPlays);
	//printf("%s", str);
	//printf("\n");
	
	/*char *step = strtok(str, " ");
	while (step != NULL) {
	    printf("%s\n", step);
	    step = strtok(NULL, " ");
	}*/
	
	
	     
	//printf("%s", str);
	
	
	i = 0;
	while (i <= 3) {
	    new->player_hp[i] = 9;
	    i++;
	}
	new->player_hp[i] = 40;
	
	new->round = 0;
	new->city_with_vam = NOWHERE;
	
	//new->cities_with_trap = malloc(6 * sizeof(struct city*));
	for (i = 0; i < 6; i++) {
	    new->cities_with_trap[i] = malloc(sizeof(struct city));
	}
	
	
	new->score = 366;
	
	for (i = 0; i < 4; i++) {
	    new->inhospital[i] = 0;
	}
	
	new->past_route = malloc(5 *sizeof(int *));
    for (i = 0; i < 5; i++) {
        new->past_route[i] = malloc(366 * sizeof(int));
    }	
    
    //new->message = malloc(100 * sizeof(Message));
     // not sure
    for (i = 0; i < new->round; i++) {
        new->message[i] = malloc(1024 * sizeof(char));
    }
	
	for(i = 0; i < 5; i++) new->playerPlace[i] = NOWHERE;

	new->turn = 0;
	//char str[] = "abc def ghi";
	//char *step = strtok(pastPlays, " ");
	char *step = strtok(str, " ");
	while (step != NULL) {
	    new->turn++;	    
		char place[2];
		for(int i = 1; i <= 2; i++) place[i - 1] = step[i];
		int placeID = placeAbbrevToId(place);


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
			new->playerPlace[PLAYER_DRACULA] = placeID;
			playerIndex = PLAYER_DRACULA;
			
			
			
		}

		// If hunters
		if (step[0] != 'D') {
			// Trap
			if (step[3] == 'T') {
				for (i = 0; i < TRAIL_SIZE; i++) {
					if(new->cities_with_trap[i]->cityID == placeID) break;
				}
				int traps = new->cities_with_trap[i]->trap_num;
				for (i = 0; i < traps; i++) {
					new->player_hp[playerIndex] -= 2;
					new->cities_with_trap[i]->trap_num--;
					if (new->player_hp[playerIndex] <= 0) {
						new->playerPlace[playerIndex] = ST_JOSEPH_AND_ST_MARY;
						new->inhospital[playerIndex] = 1;
						new->player_hp[playerIndex] = GAME_START_HUNTER_LIFE_POINTS;
						break;
					}
				}
			}
			// Vampire
			if (new->inhospital[playerIndex] == 0 && step[4] == 'V') {
				new->city_with_vam = -1;
			}

			// Dracula
			if (new->inhospital[playerIndex] == 0 && step[5] == 'D') {
				new->player_hp[playerIndex] -= 4;
				new->player_hp[PLAYER_DRACULA] -= 10;
				if (new->player_hp[playerIndex] <= 0) {
					new->playerPlace[playerIndex] = ST_JOSEPH_AND_ST_MARY;
					new->inhospital[playerIndex] = 1;
					new->player_hp[playerIndex] = GAME_START_HUNTER_LIFE_POINTS;
				}
				// maybe reveal the Dracula's position??
			}
			
		} else { // If Dracula
			if (step[3] == 'T') {
				for (i = 0; i < 6; i++) {
					if (new->cities_with_trap[i]->cityID == placeID) {
						new->cities_with_trap[i]->trap_num++;
                    }
						
				}
			}
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
    
    
    for (i = 0; i < 6; i++) {
	    free(gv->cities_with_trap[i]);
	}
    
    
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
	*numTraps = 0;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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

