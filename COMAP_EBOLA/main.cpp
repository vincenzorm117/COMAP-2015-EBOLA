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
#include <random>

#include "country.h"

// Input files
#define INPUT_GUINEA        "guinea.txt"
#define INPUT_GUINEA_BIG    "guineaREAL.txt"
#define INPUT_LIBERIA       "liberia.txt"
#define INPUT_SIERRA_LEONE  "sierra_leone.txt"
#define INPUT_WEST_AFRICA   "west_africa.txt"

#define INPUT_FILE INPUT_WEST_AFRICA

// Output files
#define MARKOV_RESULTS "results.txt"

// Simulation Parameters
#define RATE_OF_PRODUCTION 200
#define MARKOV_ITERATIONS 10

using namespace std;



typedef struct {
    double vStep;
    double mStep;
    int mWaitStep;
    int vWaitStep;
    int mSaveStep;
    int vSaveStep;
} vec6;


unsigned long int simulation(vec params, string fileName){
    
    /* Currentry is created with:
     * Given file name
     * Fixed rate of production
     * The Markov model parameters 
     */
    country p(fileName, RATE_OF_PRODUCTION, params);
    
    // Country's cities go through cycles
    for (int i = 0; i < CYCLES; i++) {
        p.updateCycle();
        p.moveBetweenCities();
        p.moveWithinCities();
        p.administerTreatment();
        p.moveTreatment();
        p.produceTreatment();
        p.receiveTreatment();
        
    }
    
    // Number of deaths in simulation are calculated and returned
    unsigned long int deaths = 0;
    for(city *c : p.cities)
        deaths += c->D;
    
    return deaths;
}


vector<vec> MCMC(vec params, int iter, vec6 step, string fileName){
    int invalidParams;
    unsigned long int dold, dnew, dbest, accepted, outside = 0;
    double quality, randNum;
    
    // Used to adjust parameters and store results respectively
    vec newparams, bestparams;
    vector<vec> histogram;
    

    // Run simulation and store death count in dold
    dold = simulation(params, fileName);
    dbest = dold;
    bestparams = params;

    // Initialize accepted to 0 (it keeps track of how many times the Markov Chain takes a step)
    accepted = 0;
    
    // Setup distributions and random number generator for varying step sizes
    std::uniform_real_distribution<double> qualityDistribution(0.0,1.0);
    std::normal_distribution<double> vWeightStepDistribution(0.0, step.vStep);
    std::normal_distribution<double> mWeightStepDistribution(0.0, step.mStep);
    std::normal_distribution<double> mWaitStepDistribution(0.0, step.mWaitStep);
    std::normal_distribution<double> vWaitStepDistribution(0.0, step.vWaitStep);
    std::normal_distribution<double> mSaveStepDistribution(0.0, step.mSaveStep);
    std::normal_distribution<double> vSaveStepDistribution(0.0, step.vSaveStep);
    std::default_random_engine generator;
    generator.seed((unsigned int)time(0));
    
    
    for (int i = 0; i < iter; i++) {

        // Make a copy of old parameters to alter with step
        newparams = params;
        invalidParams = 0;
        
        // Add a real normal randomly generator step size to the medicine weights
        for(map<int, double>::iterator it = newparams.medicineWeights.begin(); it != newparams.medicineWeights.end(); it++){
            it->second += mWeightStepDistribution(generator);
            if (it->second < 0){
                invalidParams++;
            }
        }
        
        // Add a real normal randomly generator step size to the vaccine weights
        for(map<int, double>::iterator it = newparams.vaccineWeights.begin(); it != newparams.vaccineWeights.end(); it++){
            it->second += vWeightStepDistribution(generator);
            if (it->second < 0){
                invalidParams++;
            }
        }
        

        
        // Add a discrete normal randomly generator step size to each of the integer parameters
        newparams.medicineDaysToWait += floor(mWaitStepDistribution(generator));
        newparams.vaccineDaysToWait  += floor(vWaitStepDistribution(generator));
        newparams.medicineSave       += floor(mSaveStepDistribution(generator));
        newparams.vaccineSave        += floor(vSaveStepDistribution(generator));
        
        if (newparams.medicineDaysToWait < 0 || newparams.vaccineDaysToWait < 0 || newparams.medicineSave < 0 || newparams.vaccineSave < 0){
            invalidParams++;
        }
        
        if (invalidParams){
            outside++;
            continue;
        }
        
        // Perform simulation
        dnew = simulation(newparams, fileName);

        
        // Compare old death count to new death count
        quality = (double)dold/dnew;
        
        // Set randNum a real uniformly distributed number between 0 and 1
        randNum = qualityDistribution(generator);
        
        // Check quality:
        // keep newparams if death count is better than old keep newparams
        // OR if quality is less than the real uniformly distributed random variable randNum
        if( quality <= randNum || 1.0 <= quality){
            
            if(dbest > dnew){
                dbest = dnew;
                bestparams = newparams;
            }
            
            // Set old settings to the new ones and increment the acceptance count
            params = newparams;
            dold = dnew;
            accepted++;
            
            // Store new guesses in histogram
            histogram.push_back(params);
        }
    }
    
    // Show acceptance rate
    cout << "Acceptance rate: " << 100.0 * accepted / (double)iter << " %" << endl;
    cout << "Lowest # of Deaths: " << dbest << endl;
    
    
    for (int k = 0; k < bestparams.vaccineWeights.size(); k++) {
        cout << bestparams.vaccineWeights[k] << " ";
    }
    
    for (int k = 0; k < bestparams.medicineWeights.size(); k++) {
        cout << bestparams.medicineWeights[k] << " ";
    }
    
    cout << bestparams.medicineDaysToWait << " ";
    cout << bestparams.vaccineDaysToWait << " ";
    cout << bestparams.medicineSave << " ";
    cout << bestparams.vaccineSave << " ";
    cout << std::endl;
    cout << outside << " out of bounds" << std::endl;
    
    
    return histogram;
}


int main(int argc, const char * argv[]) {
    
    // Start sequence of random numbers
    srand((unsigned int)time(0));
    
    // Initial parameters of estimation and step parameters for MCMC
    vec6 steps = (vec6){1.0,1.0,1,1,10,10};
    vec params;
    
    // Take in number of cities
    ifstream IN(INPUT_FILE);
    int numCities;
    IN >> numCities;
    IN.close();
    
    // Initial values assigned to estimation parameter
    for (int i = 0; i < numCities; i++) {
        params.vaccineWeights[i] = 100.0;
        params.medicineWeights[i] = 100.0;
    }
    
    params.vaccineDaysToWait = 5;
    params.medicineDaysToWait = 5;
    
    params.medicineSave = 1000;
    params.vaccineSave = 1000;
    
    // MCMC is ran and results retrieved
    vector<vec> results = MCMC(params, MARKOV_ITERATIONS, steps, INPUT_FILE);
    
    
    // File is created with contents of results
    ofstream OUT(MARKOV_RESULTS);
    
    // Output results to output.txt file 
    for (int i = 0; i < results.size(); i++) {
        for (int k = 0; k < results[i].vaccineWeights.size(); k++) {
            OUT << results[i].vaccineWeights[k] << " ";
        }
        
        for (int k = 0; k < results[i].medicineWeights.size(); k++) {
            OUT << results[i].medicineWeights[k] << " ";
        }
        
        OUT << results[i].medicineDaysToWait << " ";
        OUT << results[i].vaccineDaysToWait << " ";
        OUT << results[i].medicineSave << " ";
        OUT << results[i].vaccineSave << " ";
        OUT << std::endl;
    }
    
    OUT.close();
    
    return 0;
}






