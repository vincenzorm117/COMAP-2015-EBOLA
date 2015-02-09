//
//  city.h
//  COMAP_EBOLA
//
//  Created by Vincenzo on 2/7/15.
//  Copyright (c) 2015 Vincenzo. All rights reserved.
//

#ifndef COMAP_EBOLA_city_h
#define COMAP_EBOLA_city_h

#include <cstdio>
#include <iostream>
#include <queue>
#include <climits>
#include <cstdio>
#include <deque>
#include <cmath>
#include <map>
#include <fstream>
#include <iomanip>

// Simulation Parameters
#define ALPHA   .15
#define BETA    .5
#define GAMMA   .2
#define CYCLES  (365)
#define BEGIN_SICK .001
#define INCUBATED_NUM   1
#define INCUBATED_BOT 1500
#define INCUBATED_TOP 3000
#define MOVE_MEDICINE 94
#define MIGRATION_K 10000

// Display Parameters
#define WIDTH 10
#define PRECISION 6
#define PERCENTAGES false
#define SHOW_INCUBATED false

typedef struct {
    unsigned long int amount;
    int destination;
    int cyclesLeftBeforeMoving;
} package;


class city{
public:
    std::ofstream OUT;
    unsigned long int S;
    unsigned long int I;
    std::vector<unsigned long int> E;
    std::vector<city*> neighbors;
    std::map<int, unsigned long int> dist;
    unsigned long int R;
    unsigned long int D;
    unsigned long int N;
    unsigned long int N0;
    unsigned long int V;
    int name;
    int numCities;
    
    std::map<int,city*> next;
    
    bool isPort;
    
    float mweight;
    float vweight;

    unsigned long int *outMedicine;
    unsigned long int *inMedicine;
    
    unsigned long int *outVaccine;
    unsigned long int *inVaccine;
    
    ~city();
    city(unsigned long int, std::map<int,unsigned long int>,int);
    void performCycle(int);
    void display(int);
    void moveIndividuals();
    void administerTreatment();
    void moveMedicine();
    void createMedicineShelf(const int);
};


city::~city(){
    OUT.close();
    delete [] outMedicine;
    delete [] inMedicine;
    delete [] outVaccine;
    delete [] inVaccine;
}


city::city(unsigned long int size, std::map<int,unsigned long int> distances, int name){
    
    OUT.open(std::to_string(name)+".txt");
    N = N0 = size;
    if (name == 5){
        I = BEGIN_SICK * N;
        S = (1 - BEGIN_SICK) * N;
    }else{
        I = 0;
        S = N;
    }
    R = D = 0;
    dist = distances;
    this->name = name;
    
    E.resize(INCUBATED_NUM);
    unsigned long int expose;
    for (int i = 0; i < INCUBATED_NUM; i++) {
        expose = BETA * BEGIN_SICK * S;
        if (expose <= S){
            S -= expose;
            E[i] = expose;
        } else {
            E[i] = S;
            S = 0;
        }
    }
    
    display(0);
}

void city::createMedicineShelf(const int size){
    inMedicine   = new unsigned long int[size];
    outMedicine  = new unsigned long int[size];
    inVaccine    = new unsigned long int[size];
    outVaccine   = new unsigned long int[size];
    
    numCities = size;
}


void city::display(int cycle){
    
    if(PERCENTAGES){
        
        OUT << std::left << std::fixed << std::setw(4) << cycle << ") ";
        OUT << "S= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)S/N0;
        OUT << "I= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)I/N0;
        OUT << "R= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)R/N0;
        OUT << "D= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)D/N0;
        OUT << "N= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)N/N0;
        
    } else {
        
        OUT << std::left << std::fixed << std::setw(4) << cycle << ") ";
        OUT << "S= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << S;
        OUT << "I= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << I;
        OUT << "R= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << R;
        OUT << "D= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << D;
        OUT << "N= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << N;
        
        if(SHOW_INCUBATED){
            OUT << " | ";
            for(int i = 0; i < INCUBATED_NUM; i++){
                OUT << std::left << S << " ";
            }
        }
    }
    OUT << std::endl;
}


void city::performCycle(int cycle){
    // Expose people
    unsigned long int incubate = BETA * I * S / N;
    
    if (incubate > S){
        incubate = S;
    }
    
    S -= incubate;
    
    E.push_back(incubate);
    
    // Infect People
    unsigned long int Ei = E.front();
    E.erase(E.begin());
    
    unsigned long int deaths = ceil(ALPHA * I);
    unsigned long int recoveries = ceil(GAMMA * I);
    
    if(I < deaths + recoveries){
        recoveries = I - deaths;
    }
    
    // Update Death Count
    D += deaths;
    
    //Update Recovery Count
    R += recoveries;
    
    // Update Infected
    I += Ei - recoveries - deaths;
    
    // Update N
    N -= deaths;
    
    
    
    display(cycle);
}


void city::moveIndividuals(){
    unsigned long int dsdt,dedt,drdt,d;
    long double Mo;
    
    for(city *p : neighbors){
        // Calculate parameters to update group sizes
        d = (long)dist[p->name];
        
        if(p->I == 0)
            Mo = (double)(N - I) * (p->N - p->I) / (double)(d*d*MIGRATION_K);
        else
            Mo = (double)(N - I) * (p->N - p->I) * (double)I/p->I / (double)(d*d*MIGRATION_K);
        
        // Update S
        dsdt = Mo * S / (N - I);
        
        if (dsdt > S)
            dsdt = S;
        
        S -= dsdt;
        N -= dsdt;
        p->S += dsdt;
        p->N += dsdt;
        
        // Update E
        for (int i = 0; i < INCUBATED_NUM; i++) {
            dedt = Mo * E[i] / (N - I);
            
            if (dedt > E[i])
                dedt = E[i];
            
            E[i] -= dedt;
            N -= dedt;
            p->E[i] += dedt;
            p->N += dedt;
        }
        
        // Update R
        drdt = (double)Mo * R / (N - I);
        
        if (drdt > R)
            drdt = R;
        
        R -= drdt;
        N -= drdt;
        p->R += drdt;
        p->N += drdt;
        
    }
    

}


void city::administerTreatment(){

    if (inVaccine[name-1]){
        if(S < inVaccine[name-1]){
            V += S;
            R += S;
            inVaccine[name-1] -= S;
            S = 0;
        } else {
            S -= inVaccine[name-1];
            V += inVaccine[name-1];
            R += inVaccine[name-1];
            inVaccine[name-1] = 0;
        }
        
    }

    if (inMedicine[name-1]){
        if(I < inMedicine[name-1]){
            R += I;
            inMedicine[name-1] -= I;
            I = 0;
        } else {
            I -= inMedicine[name-1];
            R += inMedicine[name-1];
            inMedicine[name-1] = 0;
        }
        
    }

    
}


void city::moveMedicine(){
    
    for(int i = 0; i < numCities; i++){
        
        if(i != name-1){
            next[i]->inMedicine[i] += outMedicine[i];
            outMedicine[i] = 0;
        
        
            next[i]->inVaccine[i] += outVaccine[i];
            outVaccine[i] = 0;
        
        
            outMedicine[i] = inMedicine[i];
            inMedicine[i] = 0;
        
            outVaccine[i] = inVaccine[i];
            inVaccine[i] = 0;
        }
        
    }
}

#endif








