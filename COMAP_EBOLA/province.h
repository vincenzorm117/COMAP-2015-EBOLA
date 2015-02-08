//
//  province.h
//  COMAP_EBOLA
//
//  Created by Vincenzo on 2/7/15.
//  Copyright (c) 2015 Vincenzo. All rights reserved.
//

#ifndef COMAP_EBOLA_province_h
#define COMAP_EBOLA_province_h

#include "city.h"
#include <fstream>
#include <map>




class province {

public:
    std::vector<city*> cities;
    int cycle;
    unsigned long int factory;
    const int rateOfProduction;
    const int productionLimit;
    
    province(std::ifstream&, const int, const int);
    
    void moveBetweenCities();
    void moveWithinCities();
    void administerTreatment();
    void moveTreatment();
    void updateCycle();
    void produceMedicine();
};




province::province(std::ifstream &IN, const int rateOfProduction, const int productionLimit):
    productionLimit(productionLimit), rateOfProduction(rateOfProduction){
    
    factory = 0;
    cycle = 0;
    city *curr;
    unsigned long int pop,neighborCount,val, numCities;
    unsigned int key, name;
    std::map<int,unsigned long int> dist;
    std::map<int,city*> nameForAdress;
    
    IN >> numCities;
    
    while(numCities-- > 0){
        IN >> name;
        IN >> pop;
        IN >> neighborCount;
        while (neighborCount-- > 0) {
            IN >> key;
            IN >> val;
            dist[key] = val;
        }
        curr = new city(pop, dist,name);
        nameForAdress[curr->name] = curr;
        cities.push_back(curr);
        dist.clear();
    }
    
    for(city *c : cities){
        c->createMedicineShelf((int)cities.size());
        for(auto it = c->dist.begin(); it != c->dist.end(); ++it){
            c->neighbors.push_back(nameForAdress[(*it).first]);
        }
    }
    
}


void province::moveBetweenCities(){

    // WISH: Maybe later make this random
    for (int i = 0; i < cities.size(); i++) {
        cities[i]->moveIndividuals();
    }
}

void province::moveWithinCities() {
    
    for (int i = 0; i < cities.size(); i++) {
        cities[i]->performCycle(cycle);
    }
}

void province::produceMedicine(){
    factory += rateOfProduction;
}

void province::updateCycle(){
    this->cycle++;
}


// TODO
void province::administerTreatment(){
    
}

// TODO
void province::moveTreatment(){
    // If dice is thrown correctly produce medicine
    
    // Move medicine in province
}




#endif
