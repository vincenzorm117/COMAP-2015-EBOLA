//
//  main.cpp
//  COMAP_EBOLA
//
//  Created by Vincenzo on 2/6/15.
//  Copyright (c) 2015 Vincenzo and ASDFoster. All rights reserved.
//

#include <iostream>
#include <queue>
#include <climits>
#include <cstdio>
#include <deque>
#include <cmath>
#include <vector>
#include <fstream>
#include <cinttypes>

#include "province.h"
#include "city.h"

#define INPUT_FILE "guinea.txt"

// Simulation Parameters
#define RATE_OF_PRODUCTION 200
#define PRODUCTION_LIMIT 1

using namespace std;


int main(int argc, const char * argv[]) {
    
    
    srand((unsigned int)time(0));
    
    ifstream in;
    in.open(INPUT_FILE);
    
    province p(in, RATE_OF_PRODUCTION, PRODUCTION_LIMIT);
    
    
    for (int i = 0; i < CYCLES; i++) {
        
        p.updateCycle();
        
        p.moveBetweenCities();
        
        p.moveWithinCities();
        
        // examine these
        p.administerTreatment();
        
        p.moveTreatment();
        
        p.produceMedicine();
        
        p.receiveMedicine();
    }
    
    
    in.close();
    
    return 0;
}






