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

#define PRODUCED_MEDICINE 500


class province {

public:
    std::vector<city*> cities;
    int cycle;
    unsigned long int mfactory;
    unsigned long int vfactory;
    const int rateOfProduction;
    const int productionLimit;
    
    unsigned long int *medicine;
    unsigned long int *vaccine;
    
    province(std::ifstream&, const int, const int);
    
    void moveBetweenCities();
    void moveWithinCities();
    void administerTreatment();
    void moveTreatment();
    void updateCycle();
    void produceMedicine();
    void receiveMedicine();
    void weightCities();
};




province::province(std::ifstream &IN, const int rateOfProduction, const int productionLimit):
    productionLimit(productionLimit), rateOfProduction(rateOfProduction){
    
    mfactory = 0;
    vfactory = 0;
    cycle = 0;
    city *curr;
    unsigned long int pop,neighborCount,val, numCities;
    unsigned int key, name;
    std::map<int,unsigned long int> dist;
    std::map<int,city*> nameForAdress;
    
    IN >> numCities;
    
    for(int i = 0; i < numCities; i++){
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
        c->createMedicineShelf((int)numCities);
        for(auto it = c->dist.begin(); it != c->dist.end(); ++it){
            c->neighbors.push_back(nameForAdress[(*it).first]);
        }
    }
    
        ++numCities;
    // Calculate Shortest Paths with Floyd Warshall
        char **floydDist = new char*[numCities];
        for (int i = 0; i < numCities; i++)
            floydDist[i] = new char[numCities]();
        
        for(int i = 0; i < numCities; i++){
            for(int j = 0; j < numCities; j++){
                floydDist[i][j] = 100;
            }
        }
        
        char **next = new char*[numCities];
        for (int i = 0; i < numCities; i++)
            next[i] = new char[numCities]();
    
        for (int i = 0; i < numCities; i++) {
            for (int j = 0; j < numCities; j++) {
                next[i][j] = j;
            }
        }
        
        for( city *ct : cities){
            for(city *nei : ct->neighbors){
                floydDist[ct->name][nei->name] = 1;
            }
        }
        
        for (int k = 1; k < numCities; k++) {
            for (int i = 1; i < numCities; i++) {
                for (int j = 1; j < numCities; j++) {
                    if(floydDist[i][k] + floydDist[k][j] < floydDist[i][j]){
                        floydDist[i][j] = floydDist[i][k] + floydDist[k][j];
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
        
        for (int i = 0; i < numCities-1; i++) {
            for(int j = 0; j < numCities-1; j++){
                int pp = next[i+1][j+1];
                cities[i]->next[j] = nameForAdress[pp];
            }
        }
        
        
        for (int i = 0; i < numCities; i++)
            delete [] floydDist[i];
        delete [] floydDist;
        
        for (int i = 0; i < numCities; i++)
            delete [] next[i];
        delete [] next;
}


void province::moveBetweenCities(){

    // WISH: Maybe later make this random
    for (city *c : cities) {
        c->moveIndividuals();
    }
}

void province::moveWithinCities() {
    
    for (city *c : cities) {
        c->performCycle(cycle);
    }
}

void province::produceMedicine(){
    mfactory += rateOfProduction;
    vfactory += rateOfProduction;
}

void province::receiveMedicine(){
    
    //assign weights to cities based on current situation
//    weightCities();
    
    static int dice = 0;
    dice = (rand() + 1) % 8;
    
    cities[0]->inMedicine[dice] += mfactory;
    dice = (rand() + 1) % 8;
    cities[0]->inVaccine[dice] += vfactory;
    
//    //find port city
//    for(city *c : cities){
//        if (c->isPort){
//            for(unsigned long i = 0; i < cities.size(); i++){
//                //distribute medicine and vaccine based on weight
////                c->medicine[i] += cities[i]->mweight * mfactory;
////                c->vaccine[i]  += cities[i]->vweight * vfactory;
//                //clear factory
//                mfactory = 0;
//                vfactory = 0;
//            }
//        }
//    }
}


void province::weightCities(){
    //TODO but use c->mweight and c->vweight to keep track of weights using genetic algorithm
}

void province::updateCycle(){
    this->cycle++;
}


void province::administerTreatment(){
    for(city *c : cities){
        c->administerTreatment();
    }
}

// TODO
void province::moveTreatment(){
    // If dice is thrown correctly produce medicine
    for (city *c : cities) {
        c->moveMedicine();
    }
    // Move medicine in province
}

#endif