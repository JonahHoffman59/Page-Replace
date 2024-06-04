/*
Name: Kieran Gilpin, Jonah Hoffman
Date:05/10/2024 
Description: pageReplace program
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_PAGE_REFERENCE_LENGTH 100
#define MAX_PAGE_NUMBER 9
#define DEBUG 1



int FIFO(int framenum, int ref[]){
    //where the number will be replaced in the case of a pagefault
    int replace_space = 0;

    //how many pagefaults have occured
    int pagefaults = 0;

    //the frame holding the page reference numbers
    int frame[framenum];

    //iterate through whole reference string
    for(int i = 0; i < 100; i++){

        //counts how many pages are different than the number being added
        int diffnum = 0;

        //the page that we want to add to the frame
        int added_num = ref[i];

        for(int j = 0; j < framenum; j++){

            //no page fault in case when number is already in frame
            if (frame[j] == added_num){

                break;

            }

            else{

                diffnum++;

            }
        }
        if (diffnum == (framenum)){

            pagefaults++;

            frame[replace_space] = added_num;

            if(replace_space < (framenum-1)){

                replace_space++;
            }
            else{
                //catch overflow for spot where page fault occurs
                replace_space = 0;
            }
        }

    }


    return pagefaults;
}

int LRU(int framenum, int ref[]){
    //how many pagefaults have occured
    int pagefaults = 0;


    int frame[framenum];

    int ages[framenum];

    int max_age_index;

    bool inFrame = false;
    
    //initalize the frame and their ages
    for(int i = 0; i < framenum; i++){
        frame[i] = -1;
        ages[i] = -1;
    }

    //fill up first [framenum] spots with numbers initially
    int i = 0;
    int index = 0;
    while (frame[framenum-1] == -1){
        inFrame = false;
        int num_added = ref[i];

        for(int j = 0; j <= index; j++){
            if(frame[j] == num_added){
                inFrame = true;
                ages[j] = 0;
                break;
            }
        }
        //pagefault if value is not already in array
        if(inFrame == false){
            frame[index] = ref[i];
            ages[index]++;
            index++;
            pagefaults++;
        }
        i++;

        //age up every number
        for(int k = 0; k < framenum; k++){
            if(ages[k] >= 0){
                ages[k]++;
            }
        }

    }
        

    //go through whole ref array, starting where we left off
    for(int j = i; j < 100; j++){
        inFrame = false;

        int num_added = ref[j];

        int max_age = 0;

        //get the current max age
        for(int k = 0; k < framenum; k++){
            if(ages[k] > max_age){
                max_age = ages[k];
                max_age_index = k;
            }
        }

        //check if value being added is already in frame
        for(int k = 0; k < framenum; k++){
            if (frame[k] == num_added){
                inFrame = true;
                ages[k] = 0;
                break;
            }
        }


        
        //begin checking LRU (index of oldest number) and swap there
        if(inFrame == false){
            frame[max_age_index] = num_added;
            ages[max_age_index] = 0;
            pagefaults++;
            
        }
        //age up every number
        for(int k = 0; k < framenum; k++){
            ages[k]++;
        }
    }

    return pagefaults;
}


int OPT(int numframes, int ref[]) {
    // initializing variables
    int frames[numframes], next_used_index[numframes], furthest, faults = 0, is_in_frames, is_frame_used, is_next_used_index, index;

    // initialize frames 
    for(int i = 0; i < numframes; i++) {
        frames[i] = -1;
    }

    for(int i = 0; i < 100; i++) {
        is_in_frames = is_frame_used = 0;

        // check if any frames match page from reference string
        for(int j = 0; j < numframes; j++) {
            if(frames[j] == ref[i]) {
                // if a frame matches the needed page, then the page is in frame and therefore is not initialized and is marked used
                is_in_frames = is_frame_used = 1;
                // break off, we found the page already in frame
                break;
            }
        }

        // if the page is not in frame, but there are some frames still not used, assign page to that frame
        if (is_in_frames == 0) {
            for(int j = 0; j < numframes; j++) {
                if (frames[j] == -1) {
                    frames[j] = ref[i];
                    faults++;
                    is_frame_used = 1;
                    // break off, a free frame was found
                    break;
                }
            }
        }

        // if page is not in frame, and all frames are used, find the frame hat wont be used for the longest and replace it with the page
        if (is_frame_used == 0) {
            // variable that is set to 0 if any frames will not be used again/have a next use
            is_next_used_index = 1;

            for (int j = 0; j < numframes; j++) {

                // initialize the index of next use of the frame to -1 for each frame
                next_used_index[j] = -1;

                for(int k = i+1; k < 100; k++) {
                    // iterate through from current point until next use of frame is found
                    if (frames[j] == ref[k]) {
                        next_used_index[j] = k;
                        break;
                    }
                }
            }

            // check if any of the frames are not used again, if they aren't, then replace that one
            for(int j = 0; j < numframes; j++) {
                if (next_used_index[j] == -1) {
                    index = j;
                    is_next_used_index = 0;
                    break;
                }
            }

            // if all frames will be used again, replace the one that wont be used the longest
            if(is_next_used_index == 1) {
                furthest = next_used_index[0];
                // index used to keep track of what frame to replace
                index = 0;

                // find which frame won't be used the longest
                for(int j = 1; j < numframes; j++) {
                    if(next_used_index[j] > furthest) {
                        furthest = next_used_index[j];
                        index = j;
                    }
                }
            }
        // now that furthest frame is found, set frame to new page and increase the number of faults
        frames[index] = ref[i];
        faults++;


        }
    }

    return faults;

}




int main(int argc, char *argv[]) {
    int testPages[] = {8,3,8,7,4,7,8,2,9,0,3,2,6,3,8,1,3,7,3,5,1,1,8,9,7,0,9,6,6,8,8,4,4,9,1,0,7,3,8,0,5,8,8,4,4,4,4,8,4,5,9,9,5,1,4,3,0,8,0,0,7,1,2,3,8,3,4,9,4,1,6,8,2,8,1,6,6,9,2,2,0,3,6,6,5,1,3,0,8,7,6,1,9,1,6,6,1,3,7,0};
    
    int FIFOCount = 0;
    int LRUCount = 0;
    int OPTCount = 0;

    unsigned int seed = 0;

    int framenumber = 0;

    if(argv[1] != NULL){
        if(atoi(argv[1]) > 100){
            printf("Please enter a valid Number of frames (min = 10, max = 100) when running.\n" );
            return 0;
        }
        framenumber = atoi(argv[1]);
    }
    else{
        printf("Please enter a valid Number of frames (min = 10, max = 100) when running.\n");
        return 0;
    }

    if(argv[2] != 0){
        seed = atoi(argv[2]);
    }

    

    if(seed != 0){
        srand(seed);
    

        for (int i = 0; i < 100; i++){
            testPages[i] = rand()%MAX_PAGE_NUMBER + 0;
        }
    }
    if(DEBUG == 1){
    printf("Seed: %d\n", seed);
    printf("Reference String :");
    for(int i = 0; i < 100; i++){
        printf("%d, ",testPages[i]);
    }
    printf("\n\n");
    }

    FIFOCount = FIFO(framenumber, testPages);
    LRUCount = LRU(framenumber, testPages);
    OPTCount = OPT(framenumber, testPages);

    printf("Algorithm\t#Page faults\n");
    printf("FIFO\t\t%d\n", FIFOCount);
    printf("LRU\t\t%d\n", LRUCount);
    printf("OPT\t\t%d\n", OPTCount);

    

    return 0;
}
 