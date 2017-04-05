#ifndef TOOLS_GENETICLEARNING_LEARNINGCONTROLLER_HPP
#define TOOLS_GENETICLEARNING_LEARNINGCONTROLLER_HPP

#include "LearningParameters.hpp"
#include "PopulationRunner.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/tss.hpp>

#include <unistd.h>

#include <iostream>
#include <sstream>
#include <type_traits>

class GeneticPopulation;
class Genome;

template<typename GameInfo>
class LearningController {
public:
    LearningController(const LearningParameters& parameters,
            std::vector<GameInfo> gameInfos,
            boost::asio::io_service& ioService) :
        ioService(ioService),
        parameters(parameters),
        gameInfos(std::move(gameInfos))
    {}


    template<typename NeuralNetwork, typename GameManagerFactory>
    void run(const GameManagerFactory& gameManagerFactory) {
        using GameManager = typename std::result_of<
                typename std::decay<GameManagerFactory>::type(GameInfo)>::type;
        using PopulationRunner = ::PopulationRunner<GameManager>;

        std::vector<PopulationRunner, NeuralNetwork> populations;
        populations.reserve(parameters.startingPopulations);

        for (std::size_t i = 0; i < parameters.startingPopulations; ++i) {
            populations.emplace_back(parameters, gameManagerFactory, gameInfos,
                    ioService);
            loadPopulation(parameters, populations.back().getPopulation());
        }

        float bestFitness = 0.f;

        for (unsigned generation = 1; generation <= parameters.generationLimit;
                ++generation) {
            std::vector<float> populationAverages;
            for (auto& populationData : populations) {
                populationData.runIteration();
                populationAverages.push_back(
                        populationData.getAverageFitness());
            }

            auto& bestPopulation = *boost::max_element(populations,
                    compareBestFitnesses<PopulationRunner>);
            savePopulation(parameters, bestPopulation.getPopulation());
            if (bestPopulation.getBestFitness() > bestFitness) {
                bestFitness = bestPopulation.getBestFitness();
                saveNeuralNetwork(parameters, bestPopulation.getBestGenome());
            }

            if (populations.size() > 1 &&
                    generation % parameters.populationCutoff == 0) {
                auto worstPopulation = boost::min_element(
                        populations, compareBestFitnesses<PopulationRunner>);
                populations.erase(worstPopulation);
            }
            if (generation % parameters.printoutFrequency == 0) {
                printInfo(generation, bestFitness, populationAverages,
                        bestPopulation.getBestGenome().debugInfo);
            }
        }
    }

private:
    boost::asio::io_service& ioService;
    LearningParameters parameters;
    std::vector<GameInfo> gameInfos;

    template<typename PopulationRunner>
    static bool compareBestFitnesses(
            const PopulationRunner& lhs, const PopulationRunner& rhs) {
        return lhs.getBestFitness() < rhs.getBestFitness();
    }

    static void printInfo(unsigned generation, float bestFitness,
            const std::vector<float>& populationAverages,
            const std::string& debugInfo) {
        std::ostringstream ss;
        ss << "Generation: " << generation << ", ";
        ss << "Current best fitness: " << bestFitness << ", ";
        ss << "Population averages: ";
        ss << boost::algorithm::join(populationAverages | boost::adaptors::transformed(
                    boost::lexical_cast<std::string, float>), ", ") << ", ";
        ss << debugInfo;
        std::cerr << ss.str() << std::endl;
    }
};

#endif // TOOLS_GENETICLEARNING_LEARNINGCONTROLLER_HPP

