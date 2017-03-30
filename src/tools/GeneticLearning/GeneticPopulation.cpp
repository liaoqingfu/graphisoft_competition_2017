#include "GeneticPopulation.hpp"
#include "LearningParameters.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/numeric.hpp>

#include <cassert>
#include <algorithm>
#include <fstream>
#include <random>

namespace {

thread_local std::mt19937 rng{std::random_device{}()};

float randomReal(float min, float max) {
    return std::uniform_real_distribution<float>{min, max}(rng);
}

int randomInt(int min, int max) {
    return std::uniform_int_distribution<int>{min, max - 1}(rng);
}

void randomizeWeights(Weights& weights) {
    for (Weight& weight : weights) {
        weight = randomReal(-1.0f, 1.0f);
    }
}

} // unnamed namespace

GeneticPopulation::GeneticPopulation(unsigned populationSize, unsigned numberOfWeights) {
    for (unsigned i = 0; i < populationSize; ++i) {
        Weights weights(numberOfWeights);
        for (Weight& weight : weights) {
            weight = randomReal(-1, 1);
        }
        population.emplace_back(std::move(weights));
    }
}

const Genomes& GeneticPopulation::getPopulation() const {
    return population;
}

Genomes& GeneticPopulation::getPopulation() {
    return population;
}

void GeneticPopulation::evolve() {

    std::sort(population.begin(), population.end());

    calculateStats();
    Genomes newPopulation;

    assert((bestTopN * bestCopies) % 2 == 0);
    assert(population.size() % 2 == 0);

    pickBest(bestTopN, bestCopies, newPopulation);

    while (newPopulation.size() < population.size()) {
        Genome parent1 = pickRoulette();
        Genome parent2 = pickRoulette();

        Weights child1, child2;

        crossover(parent1.weights, parent2.weights, child1, child2);

        mutate(child1);
        mutate(child2);

        newPopulation.emplace_back(std::move(child1));
        newPopulation.emplace_back(std::move(child2));
    }

    population = newPopulation;
}

void GeneticPopulation::mutate(Weights& weights) const {
    for (Weight& weight : weights) {
        if (randomReal(0, 1) < mutationRate) {
            weight += (randomReal(-1, 1) * maxPerturbation);
        }
    }
}

Genome GeneticPopulation::pickRoulette() const {
    float slice = randomReal(0, totalFitness);

    float fitnessSoFar = 0;

    for (const Genome& genome : population) {
        fitnessSoFar += genome.fitness;
        if (genome.fitness >= slice) {
            return genome;
        }
    }
    //we shouldn't ever get here (only if rounding error occurs)
    return population.back();
}

void GeneticPopulation::crossover(
    const Weights& parent1,
    const Weights& parent2,
    Weights& child1,
    Weights& child2) const {

    assert(parent1.size() == parent2.size());

    if (randomReal(0, 1) > crossoverRate || parent1 == parent2) {
        child1 = parent1;
        child2 = parent2;
        return;
    }

    unsigned crossoverPoint = static_cast<unsigned>(randomInt(0, parent1.size()));

    child1.clear();
    child2.clear();

    //create the offspring
    for (unsigned i = 0; i < crossoverPoint; ++i) {
        child1.push_back(parent1[i]);
        child2.push_back(parent2[i]);
    }

    for (unsigned i = crossoverPoint; i < parent1.size(); ++i) {
        child1.push_back(parent2[i]);
        child2.push_back(parent1[i]);
    }
}

void GeneticPopulation::pickBest(unsigned topN, unsigned copies, Genomes& newPopulation) {
    for (unsigned i = 0; i < topN; ++i) {
        for (unsigned j = 0; j < copies; ++j) {
            newPopulation.push_back(population[population.size() - 1 - i]);
        }
    }
}

void GeneticPopulation::randomize() {
    for (Genome& genome : population) {
        randomizeWeights(genome.weights);
    }
}

void GeneticPopulation::calculateStats() {
    assert(population.size() > 0);

    auto minmax = std::minmax_element(population.begin(), population.end());
    worstFitnessIndex = minmax.first - population.begin();
    bestFitnessIndex = minmax.second - population.end();

    totalFitness = boost::accumulate(population, 0.f,
            [](float sum, const Genome& genome) { return sum + genome.fitness; });
}

void loadPopulation(const LearningParameters& parameters,
        GeneticPopulation& population) {
    if (!parameters.populationInputFile.empty() &&
            boost::filesystem::exists(parameters.populationInputFile)) {
        std::ifstream ifs(parameters.populationInputFile);
        boost::archive::text_iarchive ia(ifs);
        ia >> population.getPopulation();
    }
}

void savePopulation(const LearningParameters& parameters,
        const GeneticPopulation& population) {
    if (!parameters.populationOutputFile.empty()) {
        std::ofstream ofs(parameters.populationOutputFile);
        boost::archive::text_oarchive oa(ofs);
        oa << population.getPopulation();
    }
}

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
