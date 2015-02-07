//
//  main.cpp
//  COMAP_EBOLA
//
//  Created by Vincenzo on 2/6/15.
//  Copyright (c) 2015 Vincenzo. All rights reserved.
//

#include <iostream>
#include <queue>
#include <climits>
#include <cstdio>
#include <deque>
#include <cmath>

#define ALPHA   .20
#define BETA    .25
#define GAMMA   .15
#define CYCLES  (365)
#define GAMMApALPHA ALPHA+GAMMA
#define BEGIN_SICK .1

using namespace std;


typedef struct {
    int S;
    int I;
    queue<int> E;
    int R;
    int D;
    int N;
} city;



int main(int argc, const char * argv[]) {

    // Create city and set group (S,I,R) and population sizes
    city boke[CYCLES];
    int Ei,buff, transfer, deaths, recoveries;
    queue<int> temp;
    
    
    boke[0].S = INT_MAX / 2;
    boke[0].I = BEGIN_SICK * boke[0].S;
    boke[0].D = boke[0].R = 0;
    boke[0].N = boke[0].S + boke[0].I + boke[0].D + boke[0].R;
    
    // Fill E (the queue)
    for (int i = 0; i < 7; i++) {
        transfer = BETA * boke[0].I / boke[0].N * boke[0].S;
        boke[0].E.push(transfer);
        boke[0].S -= transfer;
    }
    
    int boke0 = boke[0].N;
    
    printf("%5d) %7d %7d |  ",0, boke[0].S,boke[0].I);
    temp = boke[0].E;
    
    //        while(!temp.empty()) {
    //            printf("%8f ",(double)temp.front());
    //            temp.pop();
    //        }
    
    buff = boke[0].S+boke[0].I+boke[0].R;
    while (!temp.empty()) {
        buff += temp.front();
        temp.pop();
    }
    
    
    printf("| %7d %7d %7d %7d\n",boke[0].R,boke[0].D,boke[0].N, buff);
    
    
    // Start Loop
    for (int i = 1; i < CYCLES; i++) {
        
        boke[i] = boke[i-1];
        
        
        transfer = BETA * boke[i].I / boke[i].N * boke[i].S;
        // Enqueue new exposed people
        boke[i].E.push(transfer);
        // Update S
        boke[i].S -= (transfer);
        
        
        // Take Incubated group out
        Ei = boke[i].E.front();
        boke[i].E.pop();
        
        deaths = ceil(ALPHA*boke[i].I);
        recoveries = GAMMA*boke[i].I;
        
        // Update D
        boke[i].D += deaths;
        
        // Update R
        boke[i].R += recoveries;
        
        // Update I
        boke[i].I += Ei -(recoveries + deaths);
        
        // Update N
        boke[i].N += -(deaths);
        
        
        printf("%5d) %10d %7d |  ",i,boke[i].S,boke[i].I);
        temp = boke[i].E;
        
        while(!temp.empty()) {
            printf("%8d ",temp.front());
            temp.pop();
        }
        
//        buff = boke[i].S+boke[i].I+boke[i].R;
//        while (!temp.empty()) {
//            buff += temp.front();
//            temp.pop();
//        }
        
        
        printf("| %7.5lf %7.5lf %10d\n",(double)boke[i].R/boke0,(double)boke[i].D/boke0,boke[i].N);
       

    }
    
    
    return 0;
}
