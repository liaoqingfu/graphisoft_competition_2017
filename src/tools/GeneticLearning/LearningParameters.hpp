#ifndef TOOLS_GENETICLEARNING_LEARNINGPARAMETERS_HPP
#define TOOLS_GENETICLEARNING_LEARNINGPARAMETERS_HPP

#include <string>

struct LearningParameters {
    unsigned inputNeuronCount = 0;
    unsigned outputNeuronCount = 0;

    //Neural network parameters
    unsigned hiddenLayerCount = 1;
    unsigned neuronPerHiddenLayer = 60;

    unsigned populationSize = 80;
    unsigned generationLimit = 1000000;
    unsigned printoutFrequency = 1;

    //place to save the file for the best AI trained
    std::string bestAIFile = "best.ai";

    //place to save/load the current population
    std::string populationOutputFile;
    std::string populationInputFile;

    unsigned startingPopulations = 1;
    unsigned populationCutoff = 10;
};

#endif // TOOLS_GENETICLEARNING_LEARNINGPARAMETERS_HPP
