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
#include <vector>
#include <fstream>
#include <cinttypes>

#include "province.h"
#include "city.h"

#define INPUT_FILE "guinea.txt"


using namespace std;



int main(int argc, const char * argv[]) {
    
    ifstream in;
    in.open(INPUT_FILE);
    
    province p(in);
    
    for (int i = 0; i < CYCLES; i++) {

        p.updateCycle();
        
        p.moveBetweenCities();
        
        p.moveWithinCities();
        
    }
    
    
    in.close();
    
    return 0;
}
