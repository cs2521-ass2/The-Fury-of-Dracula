Map new = MapNew();
    ConnList head = MapGetConnections(new, from);
    
    if (player == PLAYER_DRACULA) {
        bool canFree = 0;
        PlaceId *draculaPastRoute = GvGetLastMoves(gv, player, 6,
                            numReturnedLocs, &canFree);
        //printf("trail: %s\n", draculaTrail);
        int i = 0;
        int isDoubleBack = 0;
        int isHide = 0;
        while (i < 6) {
            if (draculaPastRoute[i] == HIDE) {
                isHide = i;
            }
            
            if (draculaPastRoute[i] >= DOUBLE_BACK_1 && draculaPastRoute[i] <= DOUBLE_BACK_5) {
                isDoubleBack = i;
            }
                
            i++;
        }
        //printf("doubleBack: %d, i: %d\n", draculaTrail[i], i);
        PlaceId *draculaTrail = GvGetLastLocations(gv, player, 6,
                            numReturnedLocs, &canFree);
        int doubleBackPlace = NOWHERE;
        int hidePlace = NOWHERE;
        if (isDoubleBack != 0) {// DoubleBack used and hide used
            ConnList curr = head;
            int j = 0;
            int last = 0;
            int check = 0;
            if (isHide == 0) {
                last = 5
                check = 4;
            } else {
                last = 6;
                check = 5;
            }
            for (int i = 0; i < last && curr != NULL; i++) {
                if (curr->p == draculaTrail[i] || curr->type != RAIL || curr->p == ST_JOSEPH_AND_ST_MARY) {
                    curr = curr->next;
                    i = 0;
                }
                if (i == check) {
                    draculaReachable[j] = curr->p;
                    j++;
                }
            }
            
            //printf("DoubleBackPlace: %d\n", doubleBackPlace);
        }
                       
       
        
        
        ConnList curr = head;
        int counter0 = 0; // counter0 = *numReturnedLocs;
        while (curr != NULL) {
            printf("curr->p: %d, numLocs: %d\n", curr->p, counter0);
            if (curr->p != doubleBackPlace && curr->p != hidePlace) {
                counter0++;
            }
            curr = curr->next;
        } 
        printf("line562: %d\n", counter0);
        if (isDoubleBack == 0) counter0 += 5;
        if (isHide == 0) counter0 += 1;
        
        PlaceId *draculaReachable = malloc((counter0 + 100) * sizeof(PlaceId));
        
        curr = head;
        int counter = 0;
        while (curr != NULL && counter < counter0) {
            if (curr->p != doubleBackPlace && curr->p != hidePlace) {
                draculaReachable[counter] = curr->p;
            }
            counter++;
            curr = curr->next;
        }
        
        if (isDoubleBack == 0) { // DoubleBack not used
            for (int i = 1; i < 6; i++) {
                draculaReachable[counter] = draculaTrail[i];
                counter++;
            }
        }
        
        if (isHide == 0) { // Hide not used
            draculaReachable[counter] = draculaTrail[0];
        }  
        //if (canFree) free(draculaTrail);
        *numReturnedLocs = counter0;
        return draculaReachable;
    }
