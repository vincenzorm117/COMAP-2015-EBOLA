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
#define BETA    .35
#define GAMMA   .10
#define CYCLES  (365)
#define BEGIN_SICK .1
#define INCUBATED_NUM   21
#define INCUBATED_BOT 1500
#define INCUBATED_TOP 3000

// Display Parameters
#define WIDTH 23
#define PRECISION 4
#define PERCENTAGES true
#define SHOW_INCUBATED false


typedef struct{
    unsigned long int amount;
    int dest;
} medicine;

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
    int name;
    // TODO:
    std::map<int, city*> shortestPathList;
    std::vector<medicine> stock;
    
    ~city();
    city(unsigned long int, std::map<int,unsigned long int>,int);
    void performCycle(int);
    void display(int);
    void moveIndividuals();
    void administerTreatment();
    void moveMedicine();
};


city::~city(){
    OUT.close();
}


city::city(unsigned long int size, std::map<int,unsigned long int> distances, int name){
    OUT.open(std::to_string(name)+".txt");
    N = N0 = size;
    I = BEGIN_SICK * N;
    S = (1 - BEGIN_SICK) * N;
    R = D = 0;
    dist = distances;
    this->name = name;
    
    E.resize(INCUBATED_NUM);
    unsigned long int expose;
    for (int i = 0; i < INCUBATED_NUM; i++) {
        expose = BETA * BEGIN_SICK * S;
        S -= expose;
        E[i] = expose;
    }
    
    display(0);
}


void city::display(int cycle){
    
    if(PERCENTAGES){
        
        OUT << cycle << ") ";
        OUT << "S= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)S/N0;
        OUT << "I= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)I/N0;
        OUT << "R= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)R/N0;
        OUT << "D= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)D/N0;
        OUT << "N= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)N/N0;
        
    } else {
        
        OUT << cycle << ") ";
        OUT << "S= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << S;
        OUT << "I= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << I;
        OUT << "R= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << R;
        OUT << "D= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << D;
        OUT << "N= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << N;
        
        if(SHOW_INCUBATED){
            printf(" | ");
            for(int i = 0; i < INCUBATED_NUM; i++){
                OUT << std::left << S << " ";

            }
        }
    }
    OUT << std::endl;
}


void city::performCycle(int cycle){
    
    // Expose people
    unsigned long int incubate = BETA * I *S / N;
    E.push_back(incubate);
    S -= incubate;
    
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
            Mo = 0;
        else
            Mo = (double)(N - I) * (p->N - p->I) * (double)I/p->I / (double)(d*d*(N+p->N));
        
        
        // Update S
        dsdt = Mo * S / (N - I);
        S -= dsdt;
        p->S += dsdt;
        
        // Update E
        for (int i = 0; i < INCUBATED_NUM; i++) {
            dedt = E[i] / (N - I) * Mo;
            E[i] -= dedt;
            p->E[i] += dedt;
        }
        
        // Update R
        drdt = R / (N - I) * Mo;
        R -= drdt;
        p->R += drdt;
        
    }
}


void city::administerTreatment(){
    if(!stock.empty()){
        // TODO: administer medicine
    }
}


void city::moveMedicine(){
    
}


#endif








