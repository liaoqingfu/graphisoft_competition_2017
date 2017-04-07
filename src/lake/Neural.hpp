#ifndef LAKE_NEURAL_HPP
#define LAKE_NEURAL_HPP

#include "Lake.hpp"
#include "MultiNeuralNetwork.hpp"
#include "LearningParameters.hpp"

#include <iomanip>
#include <ostream>
#include <random>

constexpr float scaleFactor = 10.0;
constexpr float shortScaleFactor = 1.0;

using Network = MultiNeuralNetwork<2>;

class NeuralFerryChooser {
public:
    NeuralFerryChooser(
            const LearningParameters& parameters,
            std::ostream& debug) :
            parameters(parameters),
            debug(debug),
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

    const Network& getNeuralNetwork() const {
        return networks;
    }

    template<typename FerrySet>
    std::size_t chooseFerryToAdd(
            const FerrySet& usedFerries, const FerrySet& usableFerries,
            int bikeTime, int totalTime) {
        debug << "Choosing ferry to add\n";
        return chooseFerry(networks.get(0), usableFerries,
                usedFerries.size(), usableFerries.size(), bikeTime, totalTime,
                -1);
    }

    template<typename FerrySet>
    std::size_t chooseFerryToRemove(
            const FerrySet& usedFerries, const FerrySet& usableFerries,
            int bikeTime, int totalTime) {
        debug << "Choosing ferry to remove\n";
        return chooseFerry(networks.get(1), usedFerries,
                usedFerries.size(), usableFerries.size(), bikeTime, totalTime,
                1);
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

    static constexpr unsigned inputNeuronCount = 21;
    static constexpr unsigned outputNeuronCount = 1;

private:
    template<typename FerrySet>
    std::size_t chooseFerry(NeuralNetwork& network,
            const FerrySet& ferries, std::size_t usedFerryCount,
            std::size_t usableFerryCount, int bikeTime, int totalTime,
            int bikeTimeMultiplier) {
        std::vector<float> values(ferries.size());
        std::transform(ferries.begin(), ferries.end(), values.begin(),
                [&](const Ferry* ferry) {
                    float result = callNeuralNetwork(network, *ferry,
                            usedFerryCount,
                            usableFerryCount, bikeTime, totalTime,
                            bikeTimeMultiplier);
                    debug << "  " << ferry->from << "->" << ferry->to << ": "
                            << result << "\n";
                    return result;
                });
        return std::distance(values.begin(),
                std::max_element(values.begin(), values.end()));
    }

    float callNeuralNetwork(NeuralNetwork& network,
            const Ferry& ferry, std::size_t usedFerryCount,
            std::size_t usableFerryCount, int bikeTime, int totalTime,
            int bikeTimeMultiplier) {
        const Problem& problem = solver->getProblem();
        Weights inputs{
                ferry.time * totalTimeFactor,
                ferry.time * totalTimeShortFactor,
                ferry.skippedBikeTime * bikeTimeFactor,
                ferry.skippedBikeTime * bikeTimeShortFactor,
                ferry.from * cityNumberFactor,
                ferry.to * cityNumberFactor,
                (ferry.to - ferry.from) * cityNumberFactor,
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
                // std::uniform_real_distribution<float>(
                //         -scaleFactor, scaleFactor)(rng)
        };
        assert(inputs.size() == inputNeuronCount);
        debug << std::setprecision(8);
        debug << "    inputs: ";
        for (float input : inputs) {
            debug << input << " ";
        }
        debug << "\n";
        return network.evaluateInput(inputs, debug)[0];
    }

    const Solver<NeuralFerryChooser>* solver;
    LearningParameters parameters;
    std::ostream& debug;
    Network networks;
    float totalTimeFactor;
    float totalTimeShortFactor;
    float bikeTimeFactor;
    float bikeTimeShortFactor;
    float ferryNumberFactor;
    float cityNumberFactor;
    std::mt19937 rng{std::random_device{}()};
    // std::mt19937 rng{3245346};
};

#endif // LAKE_NEURAL_HPP
