#ifndef TOOLS_GENETICLEARNING_POPULATIONRUNNER_HPP
#define TOOLS_GENETICLEARNING_POPULATIONRUNNER_HPP

#include "LearningParameters.hpp"
#include "GeneticPopulation.hpp"
#include "Genome.hpp"

#include <boost/asio/io_service.hpp>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <vector>

template<typename GameManager, typename NeuralNetwork>
class PopulationRunner {
public:
    template<typename GameManagerFactory, typename GameInfo>
    PopulationRunner(const LearningParameters& parameters,
            const GameManagerFactory& gameManagerFactory,
            const std::vector<GameInfo>& gameInfos,
            boost::asio::io_service& ioService) :
        ioService(&ioService),
        population{parameters.populationSize,
                NeuralNetwork::getWeightCountForNetwork(
                        parameters.hiddenLayerCount,
                        parameters.neuronPerHiddenLayer,
                        parameters.inputNeuronCount,
                        parameters.outputNeuronCount)} {
        controllerDatas.reserve(parameters.populationSize);
        for (std::size_t i = 0; i < parameters.populationSize; ++i) {
            controllerDatas.push_back(LearningControllerData{
                {
                    parameters.hiddenLayerCount,
                    parameters.neuronPerHiddenLayer,
                    parameters.inputNeuronCount,
                    parameters.outputNeuronCount
                },
                {}
            });

            auto& controllerData = controllerDatas.back();
            // setNeuralNetworkExternalParameters(parameters.commonParameters,
            //         controllerData.network);
            controllerData.managers.reserve(gameInfos.size());
            for (const auto& gameInfo : gameInfos) {
                controllerData.managers.push_back(gameManagerFactory(gameInfo));
            }
        }
    }


    PopulationRunner(const PopulationRunner&) = delete;
    PopulationRunner& operator=(const PopulationRunner&) = delete;
    PopulationRunner(PopulationRunner&&) = default;
    PopulationRunner& operator=(PopulationRunner&&) = default;

    void runIteration() {
        Genomes& genomes = population.getPopulation();
        assert(genomes.size() == controllerDatas.size());

        std::condition_variable conditionVariable;
        std::mutex mutex;
        std::size_t tasksLeft{controllerDatas.size()};

        for (std::size_t i = 0; i < controllerDatas.size(); ++i) {
            auto& genome = genomes[i];
            auto& data = controllerDatas[i];

            ioService->post(
                    [this, &genome, &data, &tasksLeft,
                            &conditionVariable, &mutex]() {
                        runSimulation(genome, data);
                        std::cerr << ".";
                        {
                            std::unique_lock<std::mutex> lock{mutex};
                            if (--tasksLeft == 0) {
                                conditionVariable.notify_all();
                            }
                        }
                    });
        }

        {
            std::unique_lock<std::mutex> lock{mutex};
            while (tasksLeft != 0) {
                conditionVariable.wait(lock);
            }
            std::cerr << "\n";
        }

        updateBestFitness();
        population.evolve();
    }

    float getBestFitness() const { return bestFitness; }
    float getAverageFitness() const {
        return fitnessSum / population.getPopulation().size();
    }
    const Genome& getBestGenome() const { return bestGenome; }
    const GeneticPopulation& getPopulation() const { return population; }
    GeneticPopulation& getPopulation() { return population; }
private:
    struct LearningControllerData {
        NeuralNetwork network;
        std::vector<GameManager> managers;
    };

    boost::asio::io_service* ioService;

    GeneticPopulation population;
    std::vector<LearningControllerData> controllerDatas;
    float fitnessSum = 0.f; // Updated by updateBestFitness
    float bestFitness = 0.f; // Updated by updateBestFitness
    Genome bestGenome;

    void runSimulation(Genome& genome, LearningControllerData& data) {
        data.network.setWeights(genome.weights);

        for (auto& manager: data.managers) {
            manager.setNeuralNetwork(data.network);
            manager.init();
            manager.run();
        }

        genome.fitness = 0.0f;
        genome.debugInfo.clear();
        for (const GameManager& manager : data.managers) {
            genome.fitness += manager.getFitness();
            genome.debugInfo += manager.getDebugInfo() + "  ";
        }
    }

    void updateBestFitness() {
        fitnessSum = 0.f;
        for (Genome& genome : population.getPopulation()) {
            fitnessSum += genome.fitness;
            if (genome.fitness > bestFitness) {
                // std::cerr << "New best fitness: " << bestFitness << "\n";
                bestFitness = genome.fitness;
                bestGenome = genome;
            }
        }
    }
};

#endif // TOOLS_GENETICLEARNING_POPULATIONRUNNER_HPP
