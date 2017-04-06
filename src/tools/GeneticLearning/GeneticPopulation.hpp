#ifndef TOOLS_GENETICLEARNING_GENETICPOPULATION_HPP
#define TOOLS_GENETICLEARNING_GENETICPOPULATION_HPP

#include "LearningParameters.hpp"
#include "NeuralNetwork.hpp"
#include "Genome.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <vector>

//This class implements Genetic algorithms to mutate its population
class GeneticPopulation {
public:

    GeneticPopulation() = default;
    GeneticPopulation(unsigned populationSize, unsigned numberOfWeights);

    GeneticPopulation(const GeneticPopulation&) = default;
    GeneticPopulation(GeneticPopulation&&) = default;
    GeneticPopulation& operator=(const GeneticPopulation&) = default;
    GeneticPopulation& operator=(GeneticPopulation&&) = default;

    const Genomes& getPopulation() const;
    Genomes& getPopulation();

    void evolve();

private:
    void mutate(Weights& weights) const;

    Genome pickRoulette() const;
    void pickBest(unsigned topN, unsigned copies, Genomes& newPopulation);

    void crossover(
        const Weights& parent1,
        const Weights& parent2,
        Weights& child1,
        Weights& child2) const;

    void calculateStats();
    void randomize();

    Genomes population;

    unsigned bestFitnessIndex; //updated by calculateStats()
    unsigned worstFitnessIndex; //updated by calculateStats()
    float totalFitness; //updated by calculateStats()

    //constants
    static constexpr float mutationRate = 0.1;
    static constexpr float crossoverRate = 0.7;
    static constexpr float maxPerturbation = 0.3;
    static constexpr unsigned bestTopN = 8;
    static constexpr unsigned bestCopies = 1;
};

struct LearningParameters;

void loadPopulation(const LearningParameters& parameters,
        GeneticPopulation& population);
void savePopulation(const LearningParameters& parameters,
        const GeneticPopulation& population);

template<typename NeuralNetwork>
void saveNeuralNetwork(const LearningParameters& parameters,
        const Genome& genome) {
    //TODO we are reconstucting the same network as above
    NeuralNetwork network(parameters.hiddenLayerCount,
            parameters.neuronPerHiddenLayer,
            parameters.inputNeuronCount,
            parameters.outputNeuronCount);
    // setNeuralNetworkExternalParameters(parameters.commonParameters, network);

    network.setWeights(genome.weights);

    std::ofstream ofs(parameters.bestAIFile);
    boost::archive::text_oarchive oa(ofs);
    oa << network;
}

#endif // TOOLS_GENETICLEARNING_GENETICPOPULATION_HPP
