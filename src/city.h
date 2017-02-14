//
//  city.h
//  COMAP_EBOLA
//
//  Created by Vincenzo on 2/7/15.
//  Copyright (c) 2015 Vincenzo and ASDFoster. All rights reserved.
//

#ifndef COMAP_EBOLA_city_h
#define COMAP_EBOLA_city_h

#include <cstdio>
#include <iostream>
#include <cmath>
#include <map>
#include <fstream>
#include <iomanip>

// Simulation Parameters
#define ALPHA   .1062    // 0.1062
#define BETA    .26     // .285
#define GAMMA   .0708    // .0708
#define CYCLES  (365*2)
#define BEGIN_SICK .001
#define INCUBATED_NUM  10
#define MIGRATION_K    100000
#define MIGRATION_FUDGE 500
#define MIGRATION_FUDGE_STD 100

// Display Parameters
#define WIDTH 10
#define PRECISION 6
#define PERCENTAGES false
#define SHOW_INCUBATED false

// Used to store parameters
typedef struct {
    std::map<int, double> vaccineWeights;
    std::map<int, double> medicineWeights;
    long medicineDaysToWait;
    long vaccineDaysToWait;
    long medicineSave;
    long vaccineSave;
} vec;


class city{
public:
    
    std::ofstream OUT;
    
    int name;
    int numCities;
    bool isPort;
    vec params;
    
    unsigned long int S;
    unsigned long int I;
    std::vector<unsigned long int> E;
    unsigned long int R;
    unsigned long int D;
    unsigned long int N;
    unsigned long int N0;
    unsigned long int V;

    const bool beginSick;
    
    double mweight;
    double vweight;
    
    const int myCountry;
    
    std::vector<city*> neighbors;
    std::map<int, unsigned long int> dist;
    std::map<int,city*> next;
    
    unsigned long int *outMedicine;
    unsigned long int *inMedicine;
    
    unsigned long int *outVaccine;
    unsigned long int *inVaccine;
    
    // Constructor and destructor
    city(unsigned long int, std::map<int,unsigned long int>,int,vec,bool,bool,int);
    ~city();
    
    // Function Prototypes
    void performCycle(int);
    void display(int);
    void moveIndividuals();
    void administerTreatment();
    void moveMedicine();
    void createMedicineShelf(const int);
};


// Destructor
city::~city(){
    
    // Close file output stream
    OUT.close();
    
    // Clean memory
    delete [] outMedicine;
    delete [] inMedicine;
    delete [] outVaccine;
    delete [] inVaccine;
}


// Constructor
city::city(unsigned long int size, std::map<int,unsigned long int> distances, int name,
           vec params, bool isPort, bool beginSick, int myCountry):
    beginSick(beginSick), myCountry(myCountry){
    
    // Open file output stream to write
    OUT.open(std::to_string(name)+".txt");
    
    // Keep a copy of the simulation parameters and assign
    //      inputName and isPort condition for this city
    this->params = params;
    this->name = name;
    this->isPort = isPort;
    
    // Initialize variables
    N = N0 = size;
    
    // Could add bool for startsInfected and that property can be manipulated through file ?????????????????????????????????????????
    if (name == 12){
        I = BEGIN_SICK * N;
        S = (1 - BEGIN_SICK) * N;
    } else{
        I = 0;
        S = N;
    }
    
    R = D = V = 0;
    dist = distances;
    
    
    E.resize(INCUBATED_NUM);
    unsigned long int expose;
    
    for (int i = 0; i < INCUBATED_NUM; i++) {
        expose = BETA * I * S / N;
        if (expose <= S){
            S -= expose;
            E[i] = expose;
        } else {
            E[i] = S;
            S = 0;
        }
    }

    
    // Show initial values
    display(0);
}


// Create treatment shelves for city to store incoming and outgoing vaccins/treatments
void city::createMedicineShelf(const int size){
   
    inMedicine   = new unsigned long int[size];
    outMedicine  = new unsigned long int[size];
    
    inVaccine    = new unsigned long int[size];
    outVaccine   = new unsigned long int[size];
    
    bzero(inMedicine,  sizeof(inMedicine )*size);
    bzero(outMedicine, sizeof(outMedicine)*size);
    bzero(inVaccine,   sizeof(inVaccine  )*size);
    bzero(outVaccine,  sizeof(outVaccine )*size);
    
    numCities = size;
}


// Displays the state of the city to a file based on the citie's name
void city::display(int cycle){
    
    if(PERCENTAGES){
        
        OUT << std::left << std::fixed << std::setw(4) << cycle << ") ";
        OUT << "S= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)S/N0;
        OUT << "I= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)I/N0;
        OUT << "R= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)R/N0;
        OUT << "D= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)D/N0;
        OUT << "N= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)N/N0;
        OUT << "V= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << (double)V/N0;
        
    } else {
        
        OUT << std::left << std::fixed << std::setw(4) << cycle << ") ";
        OUT << "S= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << S;
        OUT << "I= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << I;
        OUT << "R= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << R;
        OUT << "D= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << D;
        OUT << "N= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << N;
        OUT << "V= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << V;
        OUT << "P= " << std::left << std::fixed << std::setfill(' ') << std::setw(WIDTH) << std::setprecision(PRECISION) << N+D;
        
        if(SHOW_INCUBATED){
            OUT << " | ";
            for(int i = 0; i < INCUBATED_NUM; i++){
                OUT << std::left << S << " ";
            }
        }
    }
    OUT << std::endl;
}


// Simulate moving people within city
void city::performCycle(int cycle){
    
    unsigned long int incubate = BETA * I * S / N;
    
    //! Used to prevent underflow
    if (incubate > S){
        incubate = S;
    }
    
    // Move incubate amount of people out of susceptible
    // and into the incubation queue to make them exposed
    S -= incubate;
    E.push_back(incubate);
    
    // Extract person that has gone incubation period
    unsigned long int Ei = E.front();
    E.erase(E.begin());
    
    // Calculate amount of individuals that will die
    unsigned long int deaths = ceil(ALPHA * I);
    unsigned long int recoveries = GAMMA * I;
    
    //! Used to prevent underflow
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
    
    
    // Display new values of city
    display(cycle);
}


// Simulate incoming and outgoing migration
void city::moveIndividuals(){
    
    unsigned long int dsdt,dedt,drdt,d;
    long double Mo;
    
    

    
    // Move people in and out of neighboring cities
    for(city *p : neighbors){
        
        std::normal_distribution<double> migrationDist(MIGRATION_FUDGE, MIGRATION_FUDGE_STD);
        std::default_random_engine generator;
        generator.seed((unsigned int)time(0));
        
        // Calculate parameters to update group sizes
        d = (long)dist[p->name];
        
        
        //! Used to prevent underflow
        if(true){//p->I == 0){
            
            // Calculate coefficient of migration out using (N1-I1)(N2-I2) / (d^2 * migrationConstant)
            Mo = (double)(N - I) * (p->N - p->I) / (double)(d*d*MIGRATION_K) + abs(migrationDist(generator));
        } else {
            
            // Calculate coefficient of migration out using (N1-I1)(N2-I2) * (I1/I2) / (d^2 * migrationConstant)
            Mo = (double)(N - I) * (p->N - p->I) * (double)I/p->I / (double)(d*d*MIGRATION_K);
        }
        
        // Calculates the amount of change of S
        dsdt = Mo * S / (N - I);
        
        //! Used to prevent underflow
        if (S < dsdt)
            dsdt = S;
        
        // Move individuals in and out of the cities from susceptible and total population
        S -= dsdt;
        N -= dsdt;
        p->S += dsdt;
        p->N += dsdt;
        
        
        // Update E (exposed individuals)
        for (int i = 0; i < INCUBATED_NUM; i++) {
            dedt = Mo * E[i] / (N - I);
            
            if (E[i] < dedt)
                dedt = E[i];
            
            E[i] -= dedt;
            N -= dedt;
            p->E[i] += dedt;
            p->N += dedt;
        }
        
        // Calculates the amount of change of R
        drdt = (double)Mo * R / (N - I);
        
        //! Used to prevent underflow
        if (R < drdt)
            drdt = R;
        
        // Move individuals in and out of the cities from recovered and total population
        R -= drdt;
        N -= drdt;
        p->R += drdt;
        p->N += drdt;
        
    }
    

}

// Some of the vaccines go to people that appear healthy but are incubating the disease
//      this vaccine is wasted. We calculate how much is wasted first
void city::administerTreatment(){

    unsigned long totalExposed = 0, totalVaccinatable;
    
    //Calculate total number of exposed
    for (int i = 0; i < INCUBATED_NUM; i++){
        totalExposed += E[i];
    }
    
    // Calculate total number or people that might get vaccines
    totalVaccinatable = totalExposed + S;
    
    // Throw out vaccines used on people that are already incubating
    if (totalVaccinatable != 0){
        
        
        inVaccine[name]  -= ceil(inVaccine[name]  * (double) totalExposed / totalVaccinatable);
        

    }
    

    
    // If we are out of infected people or vaccinatable people, save some just in case and trade the rest
    if (S == 0 || I == 0){
        
        // Check amount of incoming vaccine to weight of amount to save
        //      If passes vaccines are given to most infected and susceptible neighboring city
        if (params.vaccineSave < inVaccine[name]){


            
            int nodeToSendVaccine = name;
            unsigned long int buff, max = 0;
            
            // Find the city with the largest amount of susceptible and infected individuals
            for(city *c: neighbors){
                buff = c->I*c->S;
                if(max < buff){
                    max = buff;
                    nodeToSendVaccine = c->name;
                }
            }
            // Vaccine amount is transfered to other selected city
            outVaccine[nodeToSendVaccine] = inVaccine[name] - params.vaccineSave;

            // Keey vaccineSave amount in stock
            inVaccine[name] = params.vaccineSave;
        }
    }
    
    //If we are out of infected people, save some medicine and trade the rest
    if (I == 0){
       
        // Check amount of incoming medicine to weight of amount to save
        //      If passes medicine are given to most infected neighboring city
        if (params.medicineSave < inMedicine[name]){

            
            int nodeToSendMedicine = name;
            unsigned long int buff, max = 0;
            
            // Find the city with the largest amount of infected individuals
            for(city *c: neighbors){
                buff = c->I;
                if(max < buff){
                    max = buff;
                    nodeToSendMedicine = c->name;
                }
            }

            // Medicine amount is transfered to other selected city
            outMedicine[nodeToSendMedicine] = inMedicine[name] - params.medicineSave;
            
            // Keey medicineSave amount in stock
            inMedicine[name] = params.medicineSave;

        }
    }
        
    // Apply vaccines if in stock in city
    if (inVaccine[name]){
        //! Used to prevent underflow
        if(S < inVaccine[name]){
            V += S;
            R += S;
            inVaccine[name] -= S;
            S = 0;

        } else {
            S -= inVaccine[name];
            V += inVaccine[name];
            R += inVaccine[name];
            inVaccine[name] = 0;

        }
        
    }

    // Apply Medicine if in stock in city
    if (inMedicine[name]){
        //! Used to prevent underflow
        if(I < inMedicine[name]){
            R += I;
            inMedicine[name] -= I;
            I = 0;

        } else {
            I -= inMedicine[name];
            R += inMedicine[name];
            inMedicine[name] = 0;

        }
    }

}


void city::moveMedicine(){
    // Move medicine to neighboring cities
    for(int i = 0; i < numCities; i++){

        // Move medicine to it's destination city
        if(i != name){
            
            // Move medicine out of city
            next[i]->inMedicine[i] += outMedicine[i];
            outMedicine[i] = 0;
            
            next[i]->inVaccine[i] += outVaccine[i];
            outVaccine[i] = 0;

            
            // Turn incoming medicine into outgoing medicine
            //      so it can be sent out next cycle
            outMedicine[i] = inMedicine[i];
            inMedicine[i] = 0;

            outVaccine[i] = inVaccine[i];
            inVaccine[i] = 0;
            
        }
    }
}

#endif








