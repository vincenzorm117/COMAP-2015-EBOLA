//
//  country.h
//  COMAP_EBOLA
//
//  Created by Vincenzo on 2/7/15.
//  Copyright (c) 2015 Vincenzo and ASDFoster. All rights reserved.
//

#ifndef COMAP_EBOLA_country_h
#define COMAP_EBOLA_country_h

#include <fstream>
#include <map>

#include "city.h"

#define INPUT_WEST_AFRICA   "west_africa.txt"


class country {

public:
    // Country's variables
    int cycle;
    vec params;
    
    std::vector<city*> cities;
    
    unsigned long int mfactory;
    unsigned long int vfactory;
    
    const int rateOfProduction;
//    const int mProductionLimit;
//    const int vProductionLimit;
    
    
//    unsigned long int *medicine;
//    unsigned long int *vaccine;
    
    // Contructor
    country(std::string, const int, vec params);
    
    
    // Function prototypes
    void moveBetweenCities();
    void moveWithinCities();
    void administerTreatment();
    void moveTreatment();
    void updateCycle();
    void produceTreatment();
    void receiveTreatment();
    void weightCities();
};




country::country(std::string inputFileName, const int rateOfProduction, vec params):
//        vProductionLimit(params.vaccineDaysToWait),
//        mProductionLimit(params.medicineDaysToWait),
        rateOfProduction(rateOfProduction),
        params(params){
        
            
        // Read in information from files and save into country's variables
        std::ifstream IN(inputFileName);
        
        mfactory = 0;
        vfactory = 0;
        cycle = 0;
        city *curr;
            
        unsigned long int pop,neighborCount,val, numCities;
        unsigned int key, name;
            
        std::map<int,unsigned long int> dist;
        std::map<int,city*> nameForAdress;
        
        IN >> numCities;
        
        int myCountry;
        bool isPort, beginSick;
            
        for(int i = 0; i < numCities; i++){
            IN >> name;
            
            IN >> isPort;
            IN >> beginSick;
            
            myCountry = 0;
            if(inputFileName == INPUT_WEST_AFRICA){
                IN >> myCountry;
            }
            
            IN >> pop;
            IN >> neighborCount;
            
            while (neighborCount-- > 0) {
                IN >> key;
                IN >> val;
                dist[key-1] = val;
            }
            
            curr = new city(pop, dist, name, params,isPort, beginSick, myCountry);
            nameForAdress[curr->name] = curr;
            cities.push_back(curr);
            dist.clear();
        }
            
        IN.close();
            
        // Create medicine shelves and set neighbors for each city
        for(city *c : cities){
            
            // Give each city a place to store medicine and vaccines
            c->createMedicineShelf((int)numCities);
            
            // Neighbor references are assigned to each city
            for(auto it = c->dist.begin(); it != c->dist.end(); ++it){
                c->neighbors.push_back(nameForAdress[(*it).first]);
            }
        }
        
        /*
         * Calculate Shortest Paths with Floyd Warshall
         * Works for edges with weight one, but its here just in case it is extended
         *     better method is used to simulate medicine or people moving around
         *     cities in a country
         */
            
            
        // Setup distances matrix
        char **floydDist = new char*[numCities];
        for (int i = 0; i < numCities; i++)
            floydDist[i] = new char[numCities]();
        
        for(int i = 0; i < numCities; i++){
            for(int j = 0; j < numCities; j++){
                floydDist[i][j] = 100;  //! Change needed with weighted edges
            }
        }
        
        // Setup next city matrix
        char **next = new char*[numCities];
        for (int i = 0; i < numCities; i++)
            next[i] = new char[numCities]();

        for (int i = 0; i < numCities; i++) {
            for (int j = 0; j < numCities; j++) {
                next[i][j] = j;
            }
        }
        
        // Set distance to neighbors to be one
        for(city *ct : cities){
            for(city *nei : ct->neighbors){
                floydDist[ct->name][nei->name] = 1; //! Change needed with weighted edges
            }
        }
        
        // Run Floyd Warshall's shortest path finding procedure
        for (int k = 0; k < numCities; k++) {
            for (int i = 0; i < numCities; i++) {
                for (int j = 0; j < numCities; j++) {
                    if(floydDist[i][k] + floydDist[k][j] < floydDist[i][j]){
                        floydDist[i][j] = floydDist[i][k] + floydDist[k][j];
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
        
        // Store neighbor city to travel to get closer
        //      (in the shortest path manner) to get to destination
        for (int i = 0; i < numCities; i++) {
            for(int j = 0; j < numCities; j++){
                int pp = next[i][j];
                cities[i]->next[j] = nameForAdress[pp];
            }
        }
        
        // Cleanup data structures
        for (int i = 0; i < numCities; i++)
            delete [] floydDist[i];
        delete [] floydDist;
        
        for (int i = 0; i < numCities; i++)
            delete [] next[i];
        delete [] next;
}


void country::moveBetweenCities(){
    
    // Simulate people moving across country
    //      (i.e. city to city)
    for (city *c : cities) {
        c->moveIndividuals();
    }
}


void country::moveWithinCities() {
    
    // Simulate people changing state in city
    //      (i.e. becoming sick or recovered)
    for (city *c : cities) {
        c->performCycle(cycle);
    }
}



void country::produceTreatment(){
    
    // Increase amount of medicine produced or
    //      imported into the country
    mfactory += rateOfProduction;
    vfactory += rateOfProduction;
}



void country::receiveTreatment(){
    
    // Assign weights to cities based on current situation
    weightCities();
    
    
    // Hand country medicine to port city
    for(city *c : cities){
        
        if (c->isPort){
            for(int i = 0; i < cities.size(); i++){
                
                // Distribute medicine and vaccine based on weight
                c->inMedicine[i] += cities[i]->mweight * mfactory;
                c->inVaccine[i]  += cities[i]->vweight * vfactory;
                
                // Clear country's government's medicine stock
                mfactory = 0;
                vfactory = 0;
            }
        }
    }
}


void country::weightCities(){

    // Create variables and vectors to calculate new medicine and vaccine weights
    double sumbasevweight = 0.0, sumbasemweight = 0.0;
    double *basevweight = new double[ cities.size() ];
    double *basemweight = new double[ cities.size() ];
    
    // Store weights for each city and calculate sum of all weights
    for (int i = 0; i < cities.size(); i++){
        
        basevweight[i] = params.vaccineWeights[i];// * cities[i]->I * cities[i]->S / cities[i]->N;
        sumbasevweight += basevweight[i];
        
        basemweight[i] = params.medicineWeights[i];// * cities[i]->I * cities[i]->S / cities[i]->N;
        sumbasemweight += basemweight[i];
        
//        printf("%lu %lu %lu %lf %lf \n", cities[i]->I, cities[i]->S, cities[i]->N, params.medicineWeights[i], basevweight[i]);
    }
    
    // Normalize all weights
    for (int i = 0; i < cities.size(); i++){
        cities[i]->mweight = basemweight[i] / sumbasemweight;
        cities[i]->vweight = basevweight[i] / sumbasevweight;
//        printf("%lf %lf %lf %lf \n", cities[i]->mweight, cities[i]->vweight, sumbasemweight, sumbasevweight);
    }

    // Clean up memory
    delete [] basemweight;
    delete [] basevweight;
}


void country::updateCycle(){
    // Simulate a day going by
    this->cycle++;
}


void country::administerTreatment(){
    // Simulate administering treatment to cities in country
    for(city *c : cities){
        c->administerTreatment();

    }
}


void country::moveTreatment(){
    // Simulate moving medicine across country
    //      (i.e. city to city)
    for (city *c : cities) {

        c->moveMedicine();
   
    }
}

#endif










