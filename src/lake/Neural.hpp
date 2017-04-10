#ifndef LAKE_NEURAL_HPP
#define LAKE_NEURAL_HPP

#include "Lake.hpp"
#include "MultiNeuralNetwork.hpp"
#include "LearningParameters.hpp"

#include <random>

constexpr float scaleFactor = 10.0;
constexpr float shortScaleFactor = 1.0;

using Network = MultiNeuralNetwork<2>;

class NeuralFerryChooser {
public:
    NeuralFerryChooser(
            const LearningParameters& parameters) :
            parameters(parameters),
            networks{
                    parameters.hiddenLayerCount,
                    parameters.neuronPerHiddenLayer,
                    parameters.inputNeuronCount,
                    parameters.outputNeuronCount} {
        assert(parameters.inputNeuronCount == inputNeuronCount);
        assert(parameters.outputNeuronCount == outputNeuronCount);
    }

    NeuralFerryChooser(const NeuralFerryChooser&) = default;
    NeuralFerryChooser(NeuralFerryChooser&&) = default;
    NeuralFerryChooser& operator=(const NeuralFerryChooser&) = default;
    NeuralFerryChooser& operator=(NeuralFerryChooser&&) = default;

    void setNeuralNetwork(Network network) {
        this->networks = std::move(network);
    }

    template<typename FerrySet>
    std::size_t chooseFerryToAdd(
            const FerrySet& usedFerries, const FerrySet& usableFerries,
            int bikeTime, int totalTime, float hash) {
        return chooseFerry(networks.get(0), usableFerries,
                usedFerries.size(), usableFerries.size(), bikeTime, totalTime,
                hash, -1);
    }

    template<typename FerrySet>
    std::size_t chooseFerryToRemove(
            const FerrySet& usedFerries, const FerrySet& usableFerries,
            int bikeTime, int totalTime, float hash) {
        return chooseFerry(networks.get(1), usedFerries,
                usedFerries.size(), usableFerries.size(), bikeTime, totalTime,
                hash, 1);
    }

    void initialize(const Solver<NeuralFerryChooser>& solver) {
        this->solver = &solver;
        const Problem& problem = solver.getProblem();
        int maximumBikeTime = std::accumulate(problem.bikePaths.begin(),
                problem.bikePaths.end(), 0);

        totalTimeFactor = scaleFactor / problem.timeLimit;
        totalTimeShortFactor = shortScaleFactor / problem.timeLimit;
        bikeTimeFactor = scaleFactor / maximumBikeTime;
        bikeTimeShortFactor = shortScaleFactor / maximumBikeTime;
        ferryNumberFactor = scaleFactor / problem.ferries.size();
        cityNumberFactor = scaleFactor / problem.bikePaths.size();
    }

    static constexpr unsigned inputNeuronCount = 22;
    static constexpr unsigned outputNeuronCount = 1;

private:
    template<typename FerrySet>
    std::size_t chooseFerry(NeuralNetwork& network,
            const FerrySet& ferries, std::size_t usedFerryCount,
            std::size_t usableFerryCount, int bikeTime, int totalTime,
            float hash, int bikeTimeMultiplier) {
        std::vector<float> values(ferries.size());
        std::transform(ferries.begin(), ferries.end(), values.begin(),
                [&](const Ferry* ferry) {
                    return callNeuralNetwork(network, *ferry, usedFerryCount,
                            usableFerryCount, bikeTime, totalTime, hash,
                            bikeTimeMultiplier);
                });
        return std::distance(values.begin(),
                std::max_element(values.begin(), values.end()));
    }

    float callNeuralNetwork(NeuralNetwork& network,
            const Ferry& ferry, std::size_t usedFerryCount,
            std::size_t usableFerryCount, int bikeTime, int totalTime,
            float hash, int bikeTimeMultiplier) {
        const Problem& problem = solver->getProblem();
        auto edge = std::make_pair(ferry.from, ferry.to);
        Weights inputs{
                ferry.time * totalTimeFactor,
                ferry.time * totalTimeShortFactor,
                ferry.skippedBikeTime * bikeTimeFactor,
                ferry.skippedBikeTime * bikeTimeShortFactor,
                source(edge, problem) * cityNumberFactor,
                target(edge, problem) * cityNumberFactor,
                (target(edge, problem) - source(edge, problem))
                        * cityNumberFactor,
                (totalTime - problem.timeLimit) * totalTimeFactor,
                (totalTime - problem.timeLimit) * totalTimeShortFactor,
                (totalTime - problem.timeLimit + ferry.time) * totalTimeFactor,
                (totalTime - problem.timeLimit + ferry.time)
                        * totalTimeShortFactor,
                bikeTime * bikeTimeFactor,
                bikeTime * bikeTimeShortFactor,
                (bikeTime + bikeTimeMultiplier * ferry.skippedBikeTime)
                        * bikeTimeFactor,
                (bikeTime + bikeTimeMultiplier * ferry.skippedBikeTime)
                        * bikeTimeShortFactor,
                solver->getBestBikeTime() * bikeTimeFactor,
                solver->getBestBikeTime() * bikeTimeShortFactor,
                solver->getBestTotalTime() * totalTimeFactor,
                solver->getBestTotalTime() * totalTimeShortFactor,
                usedFerryCount * ferryNumberFactor,
                usableFerryCount * ferryNumberFactor,
                hash * scaleFactor
        };
        assert(inputs.size() == inputNeuronCount);
        return network.evaluateInput(inputs)[0];
    }

    const Solver<NeuralFerryChooser>* solver;
    LearningParameters parameters;
    Network networks;
    float totalTimeFactor;
    float totalTimeShortFactor;
    float bikeTimeFactor;
    float bikeTimeShortFactor;
    float ferryNumberFactor;
    float cityNumberFactor;
};

#endif // LAKE_NEURAL_HPP
