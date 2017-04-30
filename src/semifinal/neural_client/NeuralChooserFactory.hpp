#ifndef NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP
#define NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP

#include <IChooser.hpp>
#include <RandomChooser.hpp>
#include <BestChooser.hpp>
#include <MaxReachableChooser.hpp>
#include <PrincessMovingChooser.hpp>
#include <MonitorDefendingChooser.hpp>
#include <LookaheadChooser.hpp>

#include <memory>

//============================================================================//

class NeuralChooserFactory {
public:
    explicit NeuralChooserFactory(std::mt19937& rng) : rng(rng) {
    }

    std::unique_ptr<IChooser> operator()() {
        int lookaheadDepth = 1;
        int lookaheadWeight = 4;
        int monitorReachabilityWeight = 10;
        int areaReachabilityWeight = 2;
        int princessMovingWeight = 10;
        bool princessMovingOverride = false;
        int maxReachableWeight = 2;
        return std::make_unique<LookaheadChooser>(
                c<MonitorDefendingChooser>(
                        c<PrincessMovingChooser>(
                                c<MaxReachableChooser>(
                                        c<BestChooser>(c<RandomChooser>(rng)),
                                        maxReachableWeight),
                                princessMovingWeight, princessMovingOverride),
                        monitorReachabilityWeight, areaReachabilityWeight),
                lookaheadDepth, lookaheadWeight);
    }

private:
    template<typename T, typename...Args>
    std::shared_ptr<T> c(Args&&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    std::mt19937& rng;
};

//============================================================================//

#endif // NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP
