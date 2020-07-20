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

struct gameView {
	// TODO: ADD FIELDS HERE
    int player_hp[5]; // player_hp[0] = LG's health point
    
    Round round; // which round it is
    struct city* cities_with_trap[6];
    int city_with_vam; // the city ID with unmatured vampire, if not, set value = -1
    //int * *cities_with_trap //array of cities with may include traps;if empty, set value = -1
    int score; // score of game
    int inhospital[4] // dracula cannot be in hosptial.not in hosptial is 0, in hosptial = 1 (0 1 2 3)
    int **past_route // store the past route for each player
    Message *message // store messages for each round.
    int turn;
     
};

struct city {
    PlaceId city_ID;
    int trap_num;
};


////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	/*new->rounds = 0;
	new->turn = 1;
	new->score = 366;
	new->hp_LG = 9;
	new->hp_DS = 9;
	new->hp_VH = 9;
	new->hp_MH = 9;
	new->hp_Dracula = 40;
	new->traps = malloc(sizeof(PlaceId*));
	new->vampire = NOWHERE;
	new->placeid_LG = NOWHERE;
	new->placeid_DS = NOWHERE;
	new->placeid_VH = NOWHERE;
	new->placeid_MH = NOWHERE;
	new->placeid_Dracula = NOWHERE;
	
	
	new->m = MapNew();
	
	
	//new->LG_view = malloc(sizeof(Message*));
	//new->DS_view = malloc(sizeof(Message*));
	//new->VH_view = malloc(sizeof(Message*));
	//new->MH_view = malloc(sizeof(Message*));
	//new->Dracula_view = malloc(sizeof(Message*));*/
	int i = 0;
	while (i <= 3) {
	    new->player_hp[i] = 9;
	    i++;
	}
	new->player_hp[i] = 40;
	
	new->round = 0;
	
	new->city_with_vam = -1;
	
	new->cities_with_trap = malloc(6 * sizeof(struct *city));
	for (i = 0; i < 6; i++) {
	    new->cities_with_trap[i] = malloc(sizeof(struct city));
	}
	
	new->city_with_vam = -1;
	
	new->score = 366;
	
	for (i = 0; i < 4; i++) {
	    new->hospital[i] = -1;
	}
	
	new->past_route = malloc(5 *sizeof(int *))
    for (i = 0; i < 5; i++) {
        new->past_route[i] = malloc(366 * sizeof(int));
    }	
    
    new->message = malloc(100 * sizeof(Message));
    
    
    // not sure
    
    for (i = 0; i < rounds; i++) {
        new->message[i] = malloc(1024 * sizeof(char));
    }
	
	
	int turn = 0;
	
	char *step = strtok(pastPlays, " ");
	while (step != NULL) {
	
	
	
	    step = strtok(NULL, " ");
	}
	
	
	
    /*for (int i = 0; pastPlays[i] != '\0'; i++) {
	    if (pastPlays[i] == ' ') turn++;
    }
    new->turn = turn + 1;
    */
   

	
	
	
	
	    
	
	
	
	
	
	
	
	
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void GvFree(GameView gv)
{
    //free(gv->LG_view);
    //free(gv->DS_view);
    //free(gv->VH_view);
    //free(gv->MH_view);
    //free(gv->Dracula_view);
    MapFree(gv->m);
    free(gv);
    
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	return gv->rounds;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int value = gv->turn % 5;
	
	if (value == 1) {
	    return PLAYER_LORD_GODALMING;
	} else if (value == 2) {
	    return PLAYER_DR_SEWARD;
	} else if (value == 3) {
	    return PLAYER_VAN_HELSING;
	} else if (value == 4) {
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
	if (player == 0) {
	    return gv->hp_LG;
	} else if (player == 1) {
	    return gv->hp_DS;
	} else if (player == 2) {
        return gv->hp_VH;
    } else if (player == 3) {
        return gv->hp_MH;
    } else {
        return gv->hp_Dracula;
    }
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	if (player == 0) {
	    return gv->placeid_LG;
	} else if (player == 1) {
	    return gv->placeid_DS;
	} else if (player == 2) {
        return gv->placeid_VH;
    } else if (player == 3) {
        return gv->placeid_MH;
    } else if (player == 4) {
        return gv->placeid_Dracula;
    }
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->vampire;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
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
